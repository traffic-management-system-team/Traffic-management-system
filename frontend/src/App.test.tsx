// @vitest-environment jsdom
import { cleanup, render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import type { SimulationConfig, SimulationSnapshot, SimulationState } from "./api/types";
import { ApiClientError } from "./api/errors";
import App from "./App";

const mockedApi = vi.hoisted(() => ({
  getState: vi.fn(),
  start: vi.fn(),
  pause: vi.fn(),
  resume: vi.fn(),
  reset: vi.fn(),
  updateConfig: vi.fn(),
}));

vi.mock("./api/client", () => ({ apiClient: mockedApi }));

const defaultConfig: SimulationConfig = {
  tickMs: 1000,
  greenTicks: 10,
  yellowTicks: 2,
  arrivalInterval: 3,
  maxQueue: 12,
};

function makeSnapshot(
  state: SimulationState = "stopped",
  overrides: Partial<SimulationSnapshot> = {},
): SimulationSnapshot {
  return {
    state,
    tickCount: 0,
    activeApproach: "north",
    activeSignal: "green",
    approaches: [
      { id: "north", name: "North", signal: "green", queueLength: 0, vehicles: [] },
      { id: "east", name: "East", signal: "red", queueLength: 0, vehicles: [] },
      { id: "south", name: "South", signal: "red", queueLength: 0, vehicles: [] },
      { id: "west", name: "West", signal: "red", queueLength: 0, vehicles: [] },
    ],
    config: { ...defaultConfig },
    statistics: {
      tickCount: 0,
      vehiclesGenerated: 0,
      vehiclesProcessed: 0,
      vehiclesWaiting: 0,
      totalWaitingTicks: 0,
      averageWaitingTicks: 0,
      processedPerLogicalTick: 0,
      approaches: [
        { id: "north", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
        { id: "south", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
        { id: "east", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
        { id: "west", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
      ],
    },
    ...overrides,
  };
}

async function renderLoaded(_state: SimulationState = "stopped") {
  const user = userEvent.setup();
  render(<App />);
  await screen.findByRole("heading", { name: "Lifecycle controls" });
  await screen.findByRole("heading", { name: "Simulation configuration" });
  return user;
}

beforeEach(() => {
  vi.clearAllMocks();
  mockedApi.getState.mockResolvedValue(makeSnapshot());
  mockedApi.start.mockResolvedValue({ state: "running" });
  mockedApi.pause.mockResolvedValue({ state: "paused" });
  mockedApi.resume.mockResolvedValue({ state: "running" });
  mockedApi.reset.mockResolvedValue({ state: "stopped", config: { ...defaultConfig } });
  mockedApi.updateConfig.mockImplementation(async (config: SimulationConfig) => ({ config }));
});

afterEach(() => cleanup());

describe("simulation controls and configuration", () => {
  it.each([
    { action: "start", initial: "stopped", next: "running" },
    { action: "pause", initial: "running", next: "paused" },
    { action: "resume", initial: "paused", next: "running" },
    { action: "reset", initial: "paused", next: "stopped" },
  ] as const)("calls $action and refreshes authoritative state", async ({ action, initial, next }) => {
    const actionApi = mockedApi[action];
    mockedApi.getState
      .mockResolvedValueOnce(makeSnapshot(initial))
      .mockResolvedValueOnce(makeSnapshot(next, { tickCount: next === "running" ? 2 : 0 }));

    const user = await renderLoaded(initial);
    await user.click(screen.getByRole("button", { name: action[0]!.toUpperCase() + action.slice(1) }));

    await waitFor(() => expect(actionApi).toHaveBeenCalledTimes(1));
    await waitFor(() => expect(screen.getByText(next, { exact: true })).toBeTruthy());
    expect(mockedApi.getState).toHaveBeenCalledTimes(2);
  });

  it.each([
    { state: "stopped", start: false, pause: true, resume: true },
    { state: "running", start: true, pause: false, resume: true },
    { state: "paused", start: true, pause: true, resume: false },
  ] as const)("enables lifecycle controls according to $state", async ({ state, start, pause, resume }) => {
    mockedApi.getState.mockResolvedValue(makeSnapshot(state));
    await renderLoaded(state);
    expect(screen.getByRole("button", { name: "Start" }).hasAttribute("disabled")).toBe(start);
    expect(screen.getByRole("button", { name: "Pause" }).hasAttribute("disabled")).toBe(pause);
    expect(screen.getByRole("button", { name: "Resume" }).hasAttribute("disabled")).toBe(resume);
    expect(screen.getByRole("button", { name: "Reset" }).hasAttribute("disabled")).toBe(false);
  });

  it("submits all five fields and adopts the backend-accepted configuration", async () => {
    const accepted: SimulationConfig = { ...defaultConfig, tickMs: 450 };
    mockedApi.getState
      .mockResolvedValueOnce(makeSnapshot())
      .mockResolvedValueOnce(makeSnapshot("stopped", { config: accepted }));
    mockedApi.updateConfig.mockResolvedValue({ config: accepted });
    const user = await renderLoaded();

    const tick = screen.getByRole("spinbutton", { name: /Tick interval/ });
    await user.clear(tick);
    await user.type(tick, String(accepted.tickMs));
    await user.click(screen.getByRole("button", { name: "Apply configuration" }));

    await waitFor(() => expect(mockedApi.updateConfig).toHaveBeenCalledWith(accepted));
    await waitFor(() => expect(screen.getByTestId("accepted-tickMs").textContent).toBe("450"));
    expect((screen.getByRole("spinbutton", { name: /Tick interval/ }) as HTMLInputElement).value).toBe("450");
  });

  it("shows field validation and preserves accepted values and the draft after rejection", async () => {
    mockedApi.updateConfig.mockRejectedValue(
      new ApiClientError("VALIDATION_FAILED", "Tick interval is outside the accepted range.", 400, [
        { field: "tickMs", code: "OUT_OF_RANGE", message: "Tick interval must be between 100 and 5000." },
      ]),
    );
    const user = await renderLoaded();
    const tick = screen.getByRole("spinbutton", { name: /Tick interval/ });
    await user.clear(tick);
    await user.type(tick, "9999");
    await user.click(screen.getByRole("button", { name: "Apply configuration" }));

    expect(await screen.findByText("Tick interval must be between 100 and 5000.")).toBeTruthy();
    expect((screen.getByRole("spinbutton", { name: /Tick interval/ }) as HTMLInputElement).value).toBe("9999");
    expect(screen.getByTestId("accepted-tickMs").textContent).toBe("1000");
    expect(screen.getByRole("spinbutton", { name: /Tick interval/ }).getAttribute("aria-invalid")).toBe("true");
    expect(mockedApi.getState).toHaveBeenCalledTimes(1);
  });

  it("shows a general backend error when details are absent", async () => {
    mockedApi.updateConfig.mockRejectedValue(
      new ApiClientError("INVALID_REQUEST", "All five configuration fields are required.", 400),
    );
    const user = await renderLoaded();
    await user.click(screen.getByRole("button", { name: "Apply configuration" }));

    const alert = await screen.findByRole("alert");
    expect(alert.textContent).toContain("All five configuration fields are required.");
    expect(screen.getByTestId("accepted-tickMs").textContent).toBe("1000");
  });

  it("refreshes state on lifecycle conflict and displays the backend message", async () => {
    mockedApi.getState
      .mockResolvedValueOnce(makeSnapshot("stopped"))
      .mockResolvedValueOnce(makeSnapshot("running", { tickCount: 1 }));
    mockedApi.start.mockRejectedValue(
      new ApiClientError("INVALID_STATE", "Simulation can only start when stopped.", 409),
    );
    const user = await renderLoaded("stopped");
    await user.click(screen.getByRole("button", { name: "Start" }));

    const alert = await screen.findByRole("alert");
    expect(alert.textContent).toContain("Simulation can only start when stopped.");
    await waitFor(() => expect(screen.getByText("running", { exact: true })).toBeTruthy());
    expect(mockedApi.getState).toHaveBeenCalledTimes(2);
  });

  it("refreshes after a configuration conflict without losing accepted settings or draft", async () => {
    mockedApi.getState
      .mockResolvedValueOnce(makeSnapshot())
      .mockResolvedValueOnce(makeSnapshot("running", { tickCount: 1 }));
    mockedApi.updateConfig.mockRejectedValue(
      new ApiClientError("INVALID_STATE", "Configuration cannot be changed in the current simulation state.", 409),
    );
    const user = await renderLoaded();
    const tick = screen.getByRole("spinbutton", { name: /Tick interval/ });
    await user.clear(tick);
    await user.type(tick, "750");
    await user.click(screen.getByRole("button", { name: "Apply configuration" }));

    const alert = await screen.findByRole("alert");
    expect(alert.textContent).toContain("Configuration cannot be changed");
    await waitFor(() => expect(screen.getByText("running", { exact: true })).toBeTruthy());
    expect((screen.getByRole("spinbutton", { name: /Tick interval/ }) as HTMLInputElement).value).toBe("750");
    expect(screen.getByTestId("accepted-tickMs").textContent).toBe("1000");
    expect(
      (screen.getByRole("spinbutton", { name: /Tick interval/ }).closest("fieldset") as HTMLFieldSetElement).disabled,
    ).toBe(true);
  });

  it("disables configuration when the stopped snapshot shows prior activity", async () => {
    mockedApi.getState.mockResolvedValue(
      makeSnapshot("stopped", {
        tickCount: 4,
        statistics: {
          ...makeSnapshot().statistics,
          tickCount: 4,
          vehiclesGenerated: 1,
        },
      }),
    );
    await renderLoaded("stopped");
    expect(
      (screen.getByRole("spinbutton", { name: /Tick interval/ }).closest("fieldset") as HTMLFieldSetElement).disabled,
    ).toBe(true);
    expect(screen.getByText(/Reset the simulation before applying new settings/)).toBeTruthy();
  });

  it("prevents duplicate actions and announces the loading state", async () => {
    let resolveStart!: (result: { state: "running" }) => void;
    mockedApi.start.mockReturnValue(new Promise((resolve) => { resolveStart = resolve; }));
    mockedApi.getState.mockResolvedValue(makeSnapshot("stopped"));
    const user = await renderLoaded("stopped");
    await user.click(screen.getByRole("button", { name: "Start" }));

    expect((await screen.findByRole("button", { name: "Starting…" })).hasAttribute("disabled")).toBe(true);
    expect(mockedApi.start).toHaveBeenCalledTimes(1);
    resolveStart({ state: "running" });
    await waitFor(() => expect(mockedApi.getState).toHaveBeenCalledTimes(2));
  });

  it("disables the configuration form and prevents resubmission while saving", async () => {
    let resolveUpdate!: (result: { config: SimulationConfig }) => void;
    mockedApi.updateConfig.mockReturnValue(
      new Promise((resolve) => {
        resolveUpdate = resolve;
      }),
    );
    const user = await renderLoaded();
    const submit = screen.getByRole("button", { name: "Apply configuration" });
    await user.click(submit);

    const pendingButton = await screen.findByRole("button", { name: "Please wait…" });
    expect((pendingButton as HTMLButtonElement).disabled).toBe(true);
    expect(
      (screen.getByRole("spinbutton", { name: /Tick interval/ }).closest("fieldset") as HTMLFieldSetElement).disabled,
    ).toBe(true);
    expect(screen.getAllByText("Saving configuration…")).toHaveLength(2);
    await user.click(pendingButton);
    expect(mockedApi.updateConfig).toHaveBeenCalledTimes(1);

    resolveUpdate({ config: { ...defaultConfig } });
    await waitFor(() => expect(mockedApi.getState).toHaveBeenCalledTimes(2));
  });

  it("allows keyboard form submission and exposes labelled fields", async () => {
    const accepted = { ...defaultConfig, maxQueue: 18 };
    mockedApi.updateConfig.mockResolvedValue({ config: accepted });
    mockedApi.getState
      .mockResolvedValueOnce(makeSnapshot())
      .mockResolvedValueOnce(makeSnapshot("stopped", { config: accepted }));
    const user = await renderLoaded();

    for (const label of [
      /Tick interval/,
      /GREEN duration/,
      /YELLOW duration/,
      /Vehicle arrival interval/,
      /Maximum queue capacity/,
    ]) {
      expect(screen.getByRole("spinbutton", { name: label })).toBeTruthy();
    }
    const maxQueue = screen.getByRole("spinbutton", { name: /Maximum queue capacity/ });
    maxQueue.focus();
    await user.clear(maxQueue);
    await user.type(maxQueue, "18{Enter}");
    await waitFor(() => expect(mockedApi.updateConfig).toHaveBeenCalledWith({ ...defaultConfig, maxQueue: 18 }));
  });
});
