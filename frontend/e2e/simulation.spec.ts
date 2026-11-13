import { expect, request, test, type APIRequestContext } from "@playwright/test";

const viewportWidths = [390, 768, 1440];

type BackendSnapshot = {
  state: string;
  tickCount: number;
  activeSignal?: string;
  signalStates?: Array<{ state?: string }>;
  approaches?: Array<{ signal?: string }>;
  statistics: {
    vehiclesGenerated: number;
    vehiclesProcessed: number;
    vehiclesWaiting: number;
  };
};

async function readBackendSnapshot(backend: APIRequestContext): Promise<BackendSnapshot> {
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
  test("configures, runs, pauses, resumes and resets the real simulation", async ({
    page,
    baseURL,
  }) => {
    if (!baseURL) {
      throw new Error("Playwright baseURL must be configured.");
    }

    const backend = await request.newContext({ baseURL });

    try {
      await page.goto("/");

      await expect(
        page.getByRole("heading", { name: "Intersection overview" }),
      ).toBeVisible();

      const dashboard = page.getByRole("region", {
        name: "Intersection & approach queues",
      });
      await expect(dashboard).toBeVisible();

      // Reset makes the test independent of state left by a previous local run.
      await page.getByRole("button", { name: "Reset" }).click();
      await expect(dashboard.getByText("stopped", { exact: true })).toBeVisible();

      // Submit all five configuration values through the browser's native
      // keyboard form-submission path.
      await page.locator("#tickMs").fill("100");
      await page.locator("#greenTicks").fill("10");
      await page.locator("#yellowTicks").fill("2");
      await page.locator("#arrivalInterval").fill("1");
      await page.locator("#maxQueue").fill("50");
      await page.locator("#maxQueue").press("Enter");

      await expect(page.getByTestId("accepted-tickMs")).toHaveText("100");
      await expect(page.getByTestId("accepted-maxQueue")).toHaveText("50");

      // Activate Start from the keyboard, then verify values produced by the
      // real backend rather than a mocked browser response.
      const startButton = page.getByRole("button", { name: "Start" });
      await startButton.focus();
      await page.keyboard.press("Enter");
      await expect(dashboard.getByText("running", { exact: true })).toBeVisible();

      const generatedCount = dashboard
        .locator("dt")
        .filter({ hasText: /^Vehicles generated$/ })
        .locator("xpath=following-sibling::dd");
      const processedCount = dashboard
        .locator("dt")
        .filter({ hasText: /^Vehicles processed$/ })
        .locator("xpath=following-sibling::dd");
      const waitingCount = dashboard
        .locator("dt")
        .filter({ hasText: /^Vehicles waiting$/ })
        .locator("xpath=following-sibling::dd");

      await expect(generatedCount).toHaveText(/[1-9]\d*/, { timeout: 15_000 });
      await expect(processedCount).toHaveText(/\d+/);
      await expect(waitingCount).toHaveText(/\d+/);
      await expect(page.getByText(/Vehicle #\d+/).first()).toBeVisible({
        timeout: 15_000,
      });

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

      // Observe transient signal phases from direct backend snapshots. The UI's
      // one-second polling interval cannot reliably capture a 200 ms yellow
      // phase, so this assertion does not depend on a rendered frame.
      await expect
        .poll(
          async () => {
            const snapshot = await readBackendSnapshot(backend);
            const states = [
              snapshot.activeSignal?.toLowerCase(),
              ...(snapshot.approaches ?? []).map((approach) =>
                approach.signal?.toLowerCase(),
              ),
              ...(snapshot.signalStates ?? []).map((signal) =>
                signal.state?.toLowerCase(),
              ),
            ];

            return states.includes("yellow");
          },
          { timeout: 15_000, intervals: [25, 40, 60] },
        )
        .toBe(true);

      await expect(
        page
          .getByRole("img", {
            name: /approach signal: (RED|YELLOW|GREEN)/,
          })
          .first(),
      ).toBeVisible();

      await page.getByRole("button", { name: "Pause" }).click();
      await expect(dashboard.getByText("paused", { exact: true })).toBeVisible();

      await page.getByRole("button", { name: "Resume" }).click();
      await expect(dashboard.getByText("running", { exact: true })).toBeVisible();

      await page.getByRole("button", { name: "Reset" }).click();
      await expect(dashboard.getByText("stopped", { exact: true })).toBeVisible();
      await expect(generatedCount).toHaveText("0");
      await expect(processedCount).toHaveText("0");
      await expect(waitingCount).toHaveText("0");
    } finally {
      await backend.dispose();
    }
  });

  test("keeps the dashboard usable at mobile, tablet and desktop widths", async ({
    page,
  }) => {
    await page.goto("/");
    await expect(
      page.getByRole("heading", { name: "Intersection overview" }),
    ).toBeVisible();

    for (const width of viewportWidths) {
      await page.setViewportSize({ width, height: 900 });

      await expect(
        page.getByRole("heading", { name: "Intersection overview" }),
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
