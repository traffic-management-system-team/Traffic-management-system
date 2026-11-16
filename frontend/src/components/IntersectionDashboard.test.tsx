// @vitest-environment jsdom
import { act, cleanup, render, screen, within } from "@testing-library/react";
import { afterEach, describe, expect, it } from "vitest";
import type { SimulationSnapshot } from "../api/types";
import { SimulationMonitor } from "./SimulationMonitor";

const sample: SimulationSnapshot = {
  state: "running",
  tickCount: 8,
  activeApproach: "north",
  activeSignal: "green",
  config: {
    tickMs: 1000,
    greenTicks: 10,
    yellowTicks: 2,
    arrivalInterval: 3,
    maxQueue: 12,
  },
  approaches: [
    {
      id: "north",
      name: "North",
      signal: "green",
      queueLength: 2,
      vehicles: [
        { id: 10, type: "Car", lane: "N1" },
        { id: 11, type: "Bus", lane: "N1" },
      ],
    },
    { id: "east", name: "East", signal: "red", queueLength: 0, vehicles: [] },
    {
      id: "south",
      name: "South",
      signal: "yellow",
      queueLength: 1,
      vehicles: [{ id: 12, type: "Motorcycle", lane: "S1" }],
    },
    { id: "west", name: "West", signal: "red", queueLength: 0, vehicles: [] },
  ],
  statistics: {
    tickCount: 8,
    vehiclesGenerated: 3,
    vehiclesProcessed: 0,
    vehiclesWaiting: 3,
    totalWaitingTicks: 0,
    averageWaitingTicks: 0,
    processedPerLogicalTick: 0,
    approaches: [
      { id: "north", vehiclesGenerated: 2, vehiclesProcessed: 0, vehiclesWaiting: 2 },
      { id: "east", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
      { id: "south", vehiclesGenerated: 1, vehiclesProcessed: 0, vehiclesWaiting: 1 },
      { id: "west", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
    ],
  },
};

const monitorProps = {
  connected: true,
  pendingOperation: null,
  actionError: null,
  onAction: () => undefined,
  onDismissActionError: () => undefined,
};

afterEach(() => cleanup());

describe("SimulationMonitor", () => {
  it("renders a four-way top-down scene with backend signal states", () => {
    render(<SimulationMonitor snapshot={sample} {...monitorProps} />);

    expect(screen.getByRole("img", { name: "Live four-way intersection and vehicle queues" })).toBeTruthy();
    expect(screen.getByRole("heading", { name: "Simulation Monitor" })).toBeTruthy();
    expect(screen.getByRole("heading", { name: "North" })).toBeTruthy();
    expect(screen.getByRole("heading", { name: "South" })).toBeTruthy();
    expect(screen.getAllByText("Vehicle #10").length).toBeGreaterThan(0);
    expect(screen.getAllByText("Vehicle #11").length).toBeGreaterThan(0);
    expect(screen.getByText("Red", { selector: ".scene-legend span" })).toBeTruthy();
  });

  it("preserves exact FIFO order and shows the front vehicle and capacity", () => {
    render(<SimulationMonitor snapshot={sample} {...monitorProps} />);

    const queue = screen.getByRole("list", { name: "North FIFO vehicle queue" });
    const rows = within(queue).getAllByRole("listitem");
    expect(rows[0]?.textContent).toContain("Vehicle #10");
    expect(rows[1]?.textContent).toContain("Vehicle #11");
    expect(screen.getByText("Front: #10")).toBeTruthy();
    const northCard = screen.getByRole("heading", { name: "North" }).closest("article");
    expect(within(northCard as HTMLElement).getByText("/ 12", { exact: false })).toBeTruthy();
  });

  it("starts with no invented history and detects changes between snapshots", () => {
    const { rerender } = render(<SimulationMonitor snapshot={sample} {...monitorProps} />);
    expect(screen.getByText(/No changes detected yet/)).toBeTruthy();

    const next: SimulationSnapshot = {
      ...sample,
      state: "paused",
      tickCount: 9,
      approaches: sample.approaches.map((approach) =>
        approach.id === "north" ? { ...approach, signal: "yellow" as const } : approach,
      ),
      statistics: {
        ...sample.statistics,
        tickCount: 9,
        approaches: sample.statistics.approaches.map((approach) =>
          approach.id === "north"
            ? { ...approach, vehiclesGenerated: 4, vehiclesProcessed: 1 }
            : approach,
        ),
      },
    };

    act(() => rerender(<SimulationMonitor snapshot={next} {...monitorProps} />));
    const log = screen.getByRole("list", { name: "Detected simulation activity" });
    expect(within(log).getByText("Simulation paused")).toBeTruthy();
    expect(within(log).getByText("North signal changed")).toBeTruthy();
    expect(within(log).getByText("2 vehicles arrived")).toBeTruthy();
    expect(within(log).getByText("1 vehicle processed")).toBeTruthy();
    expect(screen.getByText(/not read from an event stream/)).toBeTruthy();
  });

  it("does not create arrival or processing events from reset counters", () => {
    const { rerender } = render(<SimulationMonitor snapshot={sample} {...monitorProps} />);
    const reset: SimulationSnapshot = {
      ...sample,
      state: "stopped",
      tickCount: 0,
      approaches: sample.approaches.map((approach) => ({ ...approach, signal: "red", queueLength: 0, vehicles: [] })),
      statistics: {
        ...sample.statistics,
        tickCount: 0,
        vehiclesGenerated: 0,
        vehiclesProcessed: 0,
        vehiclesWaiting: 0,
        approaches: sample.statistics.approaches.map((approach) => ({ ...approach, vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 })),
      },
    };

    act(() => rerender(<SimulationMonitor snapshot={reset} {...monitorProps} />));
    const log = screen.getByRole("list", { name: "Detected simulation activity" });
    expect(within(log).getByText("Simulation reset")).toBeTruthy();
    expect(within(log).queryByText(/arrived/)).toBeNull();
    expect(within(log).queryByText(/processed/)).toBeNull();
  });

  it("states that the scene is a faithful queue view without countdown or fake motion", () => {
    render(<SimulationMonitor snapshot={sample} {...monitorProps} />);
    expect(screen.getByText(/map shows the six nearest per approach/)).toBeTruthy();
    expect(screen.getByText(/No countdown is displayed/)).toBeTruthy();
  });
});
