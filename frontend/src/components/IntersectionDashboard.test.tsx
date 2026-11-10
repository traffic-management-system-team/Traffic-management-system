
 // @vitest-environment jsdom
import { cleanup, render, screen, within } from "@testing-library/react";
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
});
