
import { expect, request, test, type APIRequestContext } from "@playwright/test";
import { mkdirSync } from "node:fs";
import { resolve } from "node:path";

const viewportWidths = [390, 768, 1440];

type BackendSnapshot = {
  state: string;
  tickCount: number;
  activeSignal?: string;
  signalStates?: Array<{ state?: string }>;
  approaches?: Array<{
    id: string;
    name: string;
    signal?: string;
    queueLength: number;
    vehicles: Array<{ id: number }>;
  }>;
  config: {
    tickMs: number;
    greenTicks: number;
    yellowTicks: number;
    arrivalInterval: number;
    maxQueue: number;
  };
  statistics: {
    vehiclesGenerated: number;
    vehiclesProcessed: number;
    vehiclesWaiting: number;
  };
};

type ApiErrorEnvelope = {
  success: boolean;
  data: unknown;
  error?: {
    code?: string;
    message?: string;
    details?: Array<{
      field?: string;
      code?: string;
      message?: string;
    }>;
  };
};

async function readBackendSnapshot(
  backend: APIRequestContext,
): Promise<BackendSnapshot> {
  const response = await backend.get("/api/simulation/state");

  if (!response.ok()) {
    throw new Error(`Backend state request failed: ${response.status()}`);
  }

  const payload = (await response.json()) as {
    success: boolean;
    data?: BackendSnapshot;
    error?: unknown;
  };

  if (!payload.success || !payload.data) {
    throw new Error("Backend response did not contain a valid simulation snapshot.");
  }

  return payload.data;
}

test.describe("live C++ simulation browser integration", () => {
  test("controls the real simulation across Control Centre and Simulation Monitor", async ({
    page,
    baseURL,
  }) => {
    // This test drives many real API lifecycle/configuration operations; keep
    // enough headroom for the hosted Chromium runner, which is slower than local.
    test.setTimeout(120_000);

    if (!baseURL) {
      throw new Error("Playwright baseURL must be configured.");
    }

    const backend = await request.newContext({ baseURL });
    const screenshotDirectory = resolve(process.cwd(), "..", "verification-screenshots");
    mkdirSync(screenshotDirectory, { recursive: true });
    const originalSnapshot = await readBackendSnapshot(backend);
    const originalConfig = { ...originalSnapshot.config };
    const testConfig = {
      tickMs: 250,
      greenTicks: 3,
      yellowTicks: 10,
      arrivalInterval: 1,
      maxQueue: 12,
    };

    try {
      await page.goto("/");

      await expect(
        page.getByRole("heading", { name: "Control Centre" }),
      ).toBeVisible();
      await expect(page.getByText("API connected", { exact: true })).toBeVisible();
      await expect(page.getByText("Unavailable", { exact: true })).toHaveCount(0);
      await expect(page.getByRole("heading", { name: "Simulation throughput" })).toBeVisible();
      await expect(page.getByText("Approach summary")).toBeVisible();
      await page.screenshot({ path: resolve(screenshotDirectory, "01-control-centre.png"), fullPage: true });

      const monitorLink = page.getByRole("link", { name: "Simulation Monitor" }).first();
      const controlLink = page.getByRole("link", { name: "Control Centre" }).first();
      const scene = page.getByRole("img", { name: "Live four-way intersection and vehicle queues" });

      // Reset makes the test independent of state left by a previous local run.
      await page.getByRole("button", { name: "Reset" }).click();
      await monitorLink.click();
      await expect(page.getByRole("heading", { name: "Simulation Monitor" })).toBeVisible();
      await expect(scene).toBeVisible();
      await page.screenshot({ path: resolve(screenshotDirectory, "02-monitor-empty.png"), fullPage: true });
      await controlLink.click();

      // The real API must reject Pause while the simulation is stopped.
      // This request goes through the configured Vite proxy to the C++ backend.
      const stoppedPauseResponse = await backend.post(
        "/api/simulation/pause",
      );
      expect(stoppedPauseResponse.status()).toBe(409);

      const stoppedPausePayload =
        (await stoppedPauseResponse.json()) as ApiErrorEnvelope;

      expect(stoppedPausePayload.success).toBe(false);
      expect(stoppedPausePayload.data).toBeNull();
      expect(stoppedPausePayload.error?.code).toBe("INVALID_STATE");
      expect(stoppedPausePayload.error?.message).toMatch(
        /only pause when running/i,
      );

      // Submit all five valid configuration values using the browser's native
      // keyboard form-submission path.
      await page.locator("#tickMs").fill(String(testConfig.tickMs));
      await page.locator("#greenTicks").fill(String(testConfig.greenTicks));
      await page.locator("#yellowTicks").fill(String(testConfig.yellowTicks));
      await page.locator("#arrivalInterval").fill(String(testConfig.arrivalInterval));
      await page.locator("#maxQueue").fill(String(testConfig.maxQueue));
      await page.locator("#maxQueue").press("Enter");

      await expect(page.getByTestId("accepted-tickMs")).toHaveText(String(testConfig.tickMs));
      await expect(page.getByTestId("accepted-greenTicks")).toHaveText(String(testConfig.greenTicks));
      await expect(page.getByTestId("accepted-maxQueue")).toHaveText(String(testConfig.maxQueue));

      // Submit an invalid value through the browser UI and capture the real
      // backend response. Zero is outside the accepted GREEN duration range.
      await page.locator("#greenTicks").fill("0");

      const rejectedConfigResponsePromise = page.waitForResponse((response) => {
        const url = new URL(response.url());
        return (
          url.pathname === "/api/simulation/config" &&
          response.request().method() === "PUT"
        );
      });

      await page
        .getByRole("button", { name: "Apply configuration" })
        .click();

      const rejectedConfigResponse = await rejectedConfigResponsePromise;
      expect(rejectedConfigResponse.status()).toBe(400);

      const rejectedConfigPayload =
        (await rejectedConfigResponse.json()) as ApiErrorEnvelope;

      expect(rejectedConfigPayload.success).toBe(false);
      expect(rejectedConfigPayload.data).toBeNull();
      expect(rejectedConfigPayload.error?.code).toBe("VALIDATION_FAILED");
      expect(rejectedConfigPayload.error?.details).toEqual(
        expect.arrayContaining([
          expect.objectContaining({ field: "greenTicks" }),
        ]),
      );

      // Verify that the UI displays the backend validation feedback and does
      // not replace the last accepted settings with the invalid draft.
      await expect(page.locator("#greenTicks")).toHaveValue("0");
      await expect(page.locator("#greenTicks")).toHaveAttribute(
        "aria-invalid",
        "true",
      );
      await expect(page.locator("#greenTicks-error")).toBeVisible();
      await expect(page.getByRole("alert")).toBeVisible();
      await expect(page.getByTestId("accepted-greenTicks")).toHaveText(String(testConfig.greenTicks));

      // Confirm directly from the backend snapshot that the rejected value
      // did not change the authoritative configuration.
      const snapshotAfterRejectedConfig = await readBackendSnapshot(backend);
      expect(snapshotAfterRejectedConfig.config.greenTicks).toBe(testConfig.greenTicks);

      // Correct the draft and verify that the backend accepts it.
      await page.locator("#greenTicks").fill(String(testConfig.greenTicks));

      const acceptedConfigResponsePromise = page.waitForResponse((response) => {
        const url = new URL(response.url());
        return (
          url.pathname === "/api/simulation/config" &&
          response.request().method() === "PUT"
        );
      });

      await page
        .getByRole("button", { name: "Apply configuration" })
        .click();

      const acceptedConfigResponse = await acceptedConfigResponsePromise;
      expect(acceptedConfigResponse.status()).toBe(200);

      await expect(page.getByTestId("accepted-greenTicks")).toHaveText(String(testConfig.greenTicks));
      await expect(page.locator("#greenTicks")).toHaveAttribute(
        "aria-invalid",
        "false",
      );
      await expect(page.locator("#greenTicks-error")).toHaveCount(0);
      await expect(page.getByRole("alert")).toHaveCount(0);

      // Activate Start from the keyboard, then verify values produced by the
      // real backend rather than a mocked browser response.
      const startButton = page.getByRole("button", { name: "Start" });
      await startButton.focus();
      await page.keyboard.press("Enter");
      await monitorLink.click();
      await expect(scene).toBeVisible();
      await expect(page.getByText("Simulation started")).toBeVisible({ timeout: 8_000 });

      const northQueue = page.getByRole("list", { name: "North FIFO vehicle queue" });
      await expect(page.getByText(/Vehicle #\d+/).first()).toBeVisible({ timeout: 15_000 });
      await expect(northQueue).toBeVisible();
      await page.screenshot({ path: resolve(screenshotDirectory, "03-monitor-active-vehicles.png"), fullPage: true });

      const waitingTicksBeforeNavigation = await page.locator(".header-tick strong").textContent();
      await controlLink.click();
      await expect
        .poll(async () => page.locator(".header-tick strong").textContent(), { timeout: 8_000 })
        .not.toBe(waitingTicksBeforeNavigation);
      await expect(page.getByText("Running", { exact: true }).first()).toBeVisible();
      await monitorLink.click();

      const generatedCount = page
        .locator("dt")
        .filter({ hasText: /^Generated$/ })
        .locator("xpath=following-sibling::dd");
      const processedCount = page
        .locator("dt")
        .filter({ hasText: /^Processed$/ })
        .locator("xpath=following-sibling::dd");
      const waitingCount = page
        .locator("dt")
        .filter({ hasText: /^Waiting$/ })
        .locator("xpath=following-sibling::dd");

      await controlLink.click();
      await expect(generatedCount).toHaveText(/[1-9]\d*/, { timeout: 15_000 });
      await expect(processedCount).toHaveText(/\d+/);
      await expect(waitingCount).toHaveText(/\d+/);
      // Keep the full-queue comparison on a paused snapshot. Waiting for the
      // backend transition avoids relying on a UI poll while the fast-running
      // simulation keeps changing during the browser assertion sequence.

      const initialStats = {
        generated: Number(await generatedCount.textContent()),
        processed: Number(await processedCount.textContent()),
        waiting: Number(await waitingCount.textContent()),
      };

      await expect
        .poll(
          async () => {
            const current = {
              generated: Number(await generatedCount.textContent()),
              processed: Number(await processedCount.textContent()),
              waiting: Number(await waitingCount.textContent()),
            };

            return (
              current.generated !== initialStats.generated ||
              current.processed !== initialStats.processed ||
              current.waiting !== initialStats.waiting
            );
          },
          { timeout: 15_000 },
        )
        .toBe(true);

      await page.getByRole("button", { name: "Pause" }).click();
      await expect(page.getByText("Paused", { exact: true }).first()).toBeVisible();
      const pausedSnapshot = await readBackendSnapshot(backend);
      await expect(generatedCount).toHaveText(String(pausedSnapshot.statistics.vehiclesGenerated));
      await expect(processedCount).toHaveText(String(pausedSnapshot.statistics.vehiclesProcessed));
      await expect(waitingCount).toHaveText(String(pausedSnapshot.statistics.vehiclesWaiting));
      await monitorLink.click();

      // The extended but valid yellow phase gives the one-second UI poll time
      // to render an actual backend state transition in the scene.
      await expect
        .poll(
          () => page.locator(".intersection-map circle[fill='#fbbf24']").count(),
          { timeout: 20_000, intervals: [100, 250, 500] },
        )
        .toBeGreaterThan(0);
      await page.screenshot({ path: resolve(screenshotDirectory, "04-signal-transition.png"), fullPage: true });
      await expect(page.getByText("Simulation paused")).toBeVisible({ timeout: 8_000 });
      await expect(page.getByText(/signal changed/).first()).toBeVisible({ timeout: 8_000 });
      await page.screenshot({ path: resolve(screenshotDirectory, "05-activity-log.png"), fullPage: true });

      await expect(page.getByRole("button", { name: "Resume" })).toBeVisible();
      await expect(page.getByText("Simulation paused")).toBeVisible({ timeout: 8_000 });

      for (const approach of pausedSnapshot.approaches ?? []) {
        const fifo = page.getByRole("list", {
          name: `${approach.name} FIFO vehicle queue`,
        });
        if (approach.vehicles.length === 0) {
          await expect(fifo).toHaveCount(0);
          continue;
        }
        const renderedRows = await fifo.locator("li").allTextContents();
        const renderedIds = renderedRows.map((row) => Number(row.match(/Vehicle #(\d+)/)?.[1]));
        expect(renderedIds).toEqual(approach.vehicles.map((vehicle) => vehicle.id));
      }

      const pausedTick = await page.locator(".header-tick strong").textContent();
      await controlLink.click();
      await page.getByRole("button", { name: "Resume" }).click();
      await expect(page.getByText("Running", { exact: true }).first()).toBeVisible();
      await monitorLink.click();
      await expect
        .poll(async () => page.locator(".header-tick strong").textContent(), { timeout: 8_000 })
        .not.toBe(pausedTick);

      await controlLink.click();
      await page.getByRole("button", { name: "Reset" }).click();
      await expect(page.getByText("Stopped", { exact: true }).first()).toBeVisible();
      await expect(generatedCount).toHaveText("0");
      await expect(processedCount).toHaveText("0");
      await expect(waitingCount).toHaveText("0");

      // Restore the actual configuration observed before this browser run.
      for (const [field, value] of Object.entries(originalConfig)) {
        await page.locator(`#${field}`).fill(String(value));
      }
      await page.getByRole("button", { name: "Apply configuration" }).click();
      for (const [field, value] of Object.entries(originalConfig)) {
        await expect(page.getByTestId(`accepted-${field}`)).toHaveText(String(value));
      }
      expect((await readBackendSnapshot(backend)).config).toEqual(originalConfig);
    } finally {
      await backend.post("/api/simulation/reset").catch(() => undefined);
      await backend.put("/api/simulation/config", { data: originalConfig }).catch(() => undefined);
      await backend.dispose();
    }
  });

  test("keeps the dashboard usable at mobile, tablet and desktop widths", async ({
    page,
  }) => {
    await page.goto("/");
    await expect(
      page.getByRole("heading", { name: "Control Centre" }),
    ).toBeVisible();
    expect(
      await page.getByRole("link", { name: "Control Centre" }).first().getAttribute("aria-current"),
    ).toBe("page");

    for (const width of viewportWidths) {
      await page.setViewportSize({ width, height: 900 });

      await expect(
        page.getByRole("heading", { name: "Control Centre" }),
      ).toBeVisible();

      const hasHorizontalOverflow = await page.evaluate(
        () => document.documentElement.scrollWidth > window.innerWidth,
      );

      expect(hasHorizontalOverflow, `horizontal overflow at ${width}px`).toBe(
        false,
      );

      await expect(
        page.getByRole("heading", { name: "Simulation configuration" }),
      ).toBeVisible();
      await expect(page.getByRole("button", { name: "Start" })).toBeVisible();

      await page.getByRole("link", { name: "Simulation Monitor" }).first().click();
      await expect(page.getByRole("heading", { name: "Simulation Monitor" })).toBeVisible();
      await expect(page.getByRole("img", { name: "Live four-way intersection and vehicle queues" })).toBeVisible();
      expect(
        await page.getByRole("link", { name: "Simulation Monitor" }).first().getAttribute("aria-current"),
      ).toBe("page");
      expect(
        await page.evaluate(() => document.documentElement.scrollWidth > window.innerWidth),
      ).toBe(false);
      if (width === 390) {
        await page.screenshot({ path: resolve(process.cwd(), "..", "verification-screenshots", "06-mobile-monitor.png"), fullPage: true });
      }
      await page.getByRole("link", { name: "Control Centre" }).first().click();
    }
  });

  test("supports keyboard access to the main content", async ({ page }) => {
    await page.goto("/");

    const skipLink = page.getByRole("link", {
      name: /skip to main content/i,
    });

    // The skip link should be the first keyboard focus target.
    await page.keyboard.press("Tab");
    await expect(skipLink).toBeFocused();

    // It should become visible when keyboard-focused.
    await expect(skipLink).toBeVisible();

    // Enter should navigate to the main content.
    await page.keyboard.press("Enter");
    await expect(page).toHaveURL(/#overview$/);
  });
});
