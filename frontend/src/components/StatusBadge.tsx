import type { SimulationState } from "../api/types";

interface StatusBadgeProps {
  state: SimulationState | "unknown";
}

const labels: Record<StatusBadgeProps["state"], string> = {
  stopped: "Stopped",
  running: "Running",
  paused: "Paused",
  unknown: "Unknown",
};

export function StatusBadge({ state }: StatusBadgeProps) {
  return (
    <span className={"state-badge state-badge-" + state}>
      <span aria-hidden="true" className="state-indicator" />
      {labels[state]}
    </span>
  );
}