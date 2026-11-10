
// @vitest-environment jsdom
import { act, cleanup, renderHook } from "@testing-library/react";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import type { SimulationSnapshot, SimulationState } from "../api/types";
import { useBackendStatus } from "./useBackendStatus";

const mockedGetState = vi.hoisted(() => vi.fn());

vi.mock("../api/client", () => ({
  apiClient: {
    getState: mockedGetState,
    start: vi.fn(),
    pause: vi.fn(),
    resume: vi.fn(),
    reset: vi.fn(),
    updateConfig: vi.fn(),
  },
}));

function makeSnapshot(state: SimulationState): SimulationSnapshot {
  return {
    state,
    tickCount: 0,
    activeApproach: "north",
    activeSignal: "red",
    approaches: [
      { id: "north", name: "North", signal: "red", queueLength: 0, vehicles: [] },
      { id: "east", name: "East", signal: "red", queueLength: 0, vehicles: [] },
      { id: "south", name: "South", signal: "red", queueLength: 0, vehicles: [] },
      { id: "west", name: "West", signal: "red", queueLength: 0, vehicles: [] },
    ],
    config: {
      tickMs: 1000,
      greenTicks: 10,
      yellowTicks: 2,
      arrivalInterval: 3,
      maxQueue: 12,
    },
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
        { id: "east", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
        { id: "south", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
        { id: "west", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
      ],
    },
  };
}

async function flushInitialRequest() {
  await act(async () => {
    await Promise.resolve();
    await Promise.resolve();
    await Promise.resolve();
  });
}

beforeEach(() => {
  vi.useFakeTimers();
  vi.clearAllMocks();
});

afterEach(() => {
  cleanup();
  vi.useRealTimers();
});

describe("useBackendStatus adaptive polling", () => {
  it("polls every 1 second when the backend state is running", async () => {
    mockedGetState.mockResolvedValue(makeSnapshot("running"));

    renderHook(() => useBackendStatus());
    await flushInitialRequest();

    expect(mockedGetState).toHaveBeenCalledTimes(1);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(999);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(1);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(1);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(2);
  });

  it("polls every 5 seconds when the backend state is paused", async () => {
    mockedGetState.mockResolvedValue(makeSnapshot("paused"));

    renderHook(() => useBackendStatus());
    await flushInitialRequest();

    expect(mockedGetState).toHaveBeenCalledTimes(1);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(4999);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(1);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(1);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(2);
  });

  it("polls every 10 seconds when the backend state is stopped", async () => {
    mockedGetState.mockResolvedValue(makeSnapshot("stopped"));

    renderHook(() => useBackendStatus());
    await flushInitialRequest();

    expect(mockedGetState).toHaveBeenCalledTimes(1);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(9999);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(1);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(1);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(2);
  });

  it("changes the polling interval when successive backend states change", async () => {
    mockedGetState.mockResolvedValue(makeSnapshot("running"));

    renderHook(() => useBackendStatus());
    await flushInitialRequest();

    expect(mockedGetState).toHaveBeenCalledTimes(1);

    // Running: the next poll is after 1 second.
    mockedGetState.mockResolvedValue(makeSnapshot("paused"));

    await act(async () => {
      await vi.advanceTimersByTimeAsync(1000);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(2);

    // Paused: the next poll is after 5 seconds, not 1 second.
    mockedGetState.mockResolvedValue(makeSnapshot("stopped"));

    await act(async () => {
      await vi.advanceTimersByTimeAsync(4999);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(2);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(1);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(3);

    // Stopped: the next poll is after 10 seconds.
    await act(async () => {
      await vi.advanceTimersByTimeAsync(9999);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(3);

    await act(async () => {
      await vi.advanceTimersByTimeAsync(1);
    });
    expect(mockedGetState).toHaveBeenCalledTimes(4);
  });
});
