
import { expect, test } from "@playwright/test";

const desktopWidths = [390, 768, 1440];

test.describe("live C++ simulation browser integration", () => {
  test("configures, runs, pauses, resumes and resets the real simulation", async ({ page }) => {
    await page.goto("/");

    await expect(page.getByRole("heading", { name: "Intersection overview" })).toBeVisible();
    const dashboard = page.getByRole("region", { name: "Intersection & approach queues" });
    await expect(dashboard).toBeVisible();

    // Reset makes the test independent of state left by a previous local run.
    await page.getByRole("button", { name: "Reset" }).click();
    await expect(dashboard.getByText("stopped", { exact: true })).toBeVisible();

    // The form submits all five values to the real REST API. Enter exercises
    // the browser's native keyboard form submission path.
    await page.locator("#tickMs").fill("100");
    await page.locator("#greenTicks").fill("10");
    await page.locator("#yellowTicks").fill("2");
    await page.locator("#arrivalInterval").fill("1");
    await page.locator("#maxQueue").fill("50");
    await page.locator("#maxQueue").press("Enter");

    await expect(page.getByTestId("accepted-tickMs")).toHaveText("100");
    await expect(page.getByTestId("accepted-maxQueue")).toHaveText("50");

    // Activate Start from the keyboard, then verify values produced by the
    // backend rather than a mocked browser response.
    const startButton = page.getByRole("button", { name: "Start" });
    await startButton.focus();
    await page.keyboard.press("Enter");
    await expect(dashboard.getByText("running", { exact: true })).toBeVisible();

    const generatedCount = page
      .getByText("Vehicles generated", { exact: true })
      .locator("..")
      .locator("dd");

    await expect(generatedCount).toHaveText(/[1-9]\d*/, { timeout: 15_000 });
    await expect(page.getByText("Vehicles processed", { exact: true })).toBeVisible();
    await expect(page.getByText("Vehicles waiting", { exact: true })).toBeVisible();
    await expect(page.getByText(/Vehicle #\d+/).first()).toBeVisible({ timeout: 15_000 });

    // Signal state is exposed as a descriptive accessible name, not by color
    // alone. At least one approach must expose a valid textual state.
    await expect(
      page.getByRole("img", { name: /approach signal: (RED|YELLOW|GREEN)/ }).first(),
    ).toBeVisible();

    await expect(
      page.getByRole("img", { name: /approach signal: YELLOW/ }).first(),
    ).toBeVisible({ timeout: 10_000 });

    await page.getByRole("button", { name: "Pause" }).click();
    await expect(dashboard.getByText("paused", { exact: true })).toBeVisible();

    await page.getByRole("button", { name: "Resume" }).click();
    await expect(dashboard.getByText("running", { exact: true })).toBeVisible();

    await page.getByRole("button", { name: "Reset" }).click();
    await expect(dashboard.getByText("stopped", { exact: true })).toBeVisible();
    await expect(generatedCount).toHaveText("0");
  });

  test("keeps the dashboard usable at mobile, tablet and desktop widths", async ({ page }) => {
    await page.goto("/");
    await expect(page.getByRole("heading", { name: "Intersection overview" })).toBeVisible();

    for (const width of desktopWidths) {
      await page.setViewportSize({ width, height: 900 });

      await expect(page.getByRole("heading", { name: "Intersection overview" })).toBeVisible();

      const hasHorizontalOverflow = await page.evaluate(
        () => document.documentElement.scrollWidth > window.innerWidth,
      );

      expect(hasHorizontalOverflow, `horizontal overflow at ${width}px`).toBe(false);

      await expect(page.getByRole("heading", { name: "Simulation configuration" })).toBeVisible();
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
