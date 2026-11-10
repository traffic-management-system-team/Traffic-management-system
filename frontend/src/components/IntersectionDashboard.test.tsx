
// @vitest-environment jsdom
import { act, cleanup, render, screen, within } from "@testing-library/react";
import { afterEach, describe, expect, it } from "vitest";
import type { SimulationSnapshot } from "../api/types";
import { IntersectionDashboard } from "./IntersectionDashboard";

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

function withNorthCounters(
  snapshot: SimulationSnapshot,
  vehiclesGenerated: number,
  vehiclesProcessed: number,
): SimulationSnapshot {
  return {
    ...snapshot,
    statistics: {
      ...snapshot.statistics,
      approaches: snapshot.statistics.approaches.map((approach) =>
        approach.id === "north"
          ? { ...approach, vehiclesGenerated, vehiclesProcessed }
          : approach,
      ),
    },
  };
}

afterEach(() => cleanup());

describe("IntersectionDashboard", () => {
  it("shows each backend approach and readable signal states", () => {
    render(<IntersectionDashboard snapshot={sample} />);

    for (const direction of ["North", "East", "South", "West"]) {
      expect(screen.getByRole("heading", { name: direction })).toBeTruthy();
    }

    expect(screen.getByText("GREEN")).toBeTruthy();
    expect(screen.getAllByText("RED")).toHaveLength(2);
    expect(screen.getByText("YELLOW")).toBeTruthy();
  });

  it("renders authoritative aggregate statistics from each refreshed snapshot", () => {
    const { rerender } = render(
      <IntersectionDashboard snapshot={sample} />,
    );

    const refreshed: SimulationSnapshot = {
      ...sample,
      tickCount: 25,
      statistics: {
        ...sample.statistics,
        tickCount: 25,
        vehiclesGenerated: 16,
        vehiclesProcessed: 11,
        vehiclesWaiting: 5,
        totalWaitingTicks: 28,
        averageWaitingTicks: 2.5,
        processedPerLogicalTick: 0.44,
      },
    };

    act(() => {
      rerender(<IntersectionDashboard snapshot={refreshed} />);
    });

    const summary = screen.getByLabelText("Simulation statistics");
    expect(within(summary).getByText("16")).toBeTruthy();
    expect(within(summary).getByText("11")).toBeTruthy();
    expect(within(summary).getByText("5")).toBeTruthy();
    expect(within(summary).getByText("2.5")).toBeTruthy();
    expect(screen.getByText("Logical tick:").parentElement?.textContent).toContain("25");
  });

  it("uses responsive grids for summary metrics and approach cards", () => {
    render(<IntersectionDashboard snapshot={sample} />);

    const summary = screen.getByLabelText("Simulation statistics");
    expect(summary.className).toContain("grid-cols-2");
    expect(summary.className).toContain("sm:grid-cols-4");

    const northCard = screen
      .getByRole("heading", { name: "North" })
      .closest("article");
    expect(northCard).toBeTruthy();

    const approachGrid = northCard?.parentElement;
    expect(approachGrid?.className).toContain("sm:grid-cols-2");
    expect(approachGrid?.className).toContain("xl:grid-cols-4");
  });

  it("preserves backend FIFO order and displays queue capacity", () => {
    render(<IntersectionDashboard snapshot={sample} />);

    const northHeading = screen.getByRole("heading", { name: "North" });
    const northCard = northHeading.closest("article");

    expect(northCard).toBeTruthy();

    const card = within(northCard as HTMLElement);
    const queue = card.getByRole("list", {
      name: "North FIFO vehicle queue",
    });
    const rows = within(queue).getAllByRole("listitem");

    expect(rows[0]?.textContent).toContain("Vehicle #10");
    expect(rows[1]?.textContent).toContain("Vehicle #11");
    expect(card.getByText(/Queued\s*\/\s*12 capacity/)).toBeTruthy();
    expect(screen.getByText(/Logical tick:/)).toBeTruthy();
  });

  it("shows arrivals when backend vehiclesGenerated increases", () => {
    const { rerender } = render(
      <IntersectionDashboard snapshot={sample} />,
    );

    const nextSnapshot = withNorthCounters(sample, 5, 0);

    act(() => {
      rerender(<IntersectionDashboard snapshot={nextSnapshot} />);
    });

    const northCard = screen
      .getByRole("heading", { name: "North" })
      .closest("article") as HTMLElement;

    expect(within(northCard).getByText("+3 generated")).toBeTruthy();
  });

  it("shows processed vehicles when backend vehiclesProcessed increases", () => {
    const { rerender } = render(
      <IntersectionDashboard snapshot={sample} />,
    );

    const nextSnapshot = withNorthCounters(sample, 2, 3);

    act(() => {
      rerender(<IntersectionDashboard snapshot={nextSnapshot} />);
    });

    const northCard = screen
      .getByRole("heading", { name: "North" })
      .closest("article") as HTMLElement;

    expect(within(northCard).getByText("3 processed")).toBeTruthy();
  });

  it("does not show negative activity when backend counters reset", () => {
    const beforeReset = withNorthCounters(sample, 5, 4);
    const { rerender } = render(
      <IntersectionDashboard snapshot={beforeReset} />,
    );

    const resetSnapshot = withNorthCounters(beforeReset, 0, 0);

    act(() => {
      rerender(<IntersectionDashboard snapshot={resetSnapshot} />);
    });

    const northCard = screen
      .getByRole("heading", { name: "North" })
      .closest("article") as HTMLElement;

    expect(within(northCard).getByText("+0 generated")).toBeTruthy();
    expect(within(northCard).getByText("0 processed")).toBeTruthy();
    expect(northCard.textContent).not.toMatch(/-\d+ (generated|processed)/);
  });

  it("exposes descriptive accessible labels for every signal indicator", () => {
    render(<IntersectionDashboard snapshot={sample} />);

    expect(
      screen.getByRole("img", { name: "North approach signal: GREEN" }),
    ).toBeTruthy();
    expect(
      screen.getByRole("img", { name: "East approach signal: RED" }),
    ).toBeTruthy();
    expect(
      screen.getByRole("img", { name: "South approach signal: YELLOW" }),
    ).toBeTruthy();
    expect(
      screen.getByRole("img", { name: "West approach signal: RED" }),
    ).toBeTruthy();
  });
});
