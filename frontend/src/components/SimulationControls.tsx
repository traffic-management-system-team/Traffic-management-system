import type { SimulationState } from "../api/types";
import type { SimulationOperation } from "../hooks/useBackendStatus";

interface SimulationControlsProps {
  compact?: boolean;
  state: SimulationState | "unknown";
  pendingOperation: SimulationOperation | null;
  connected: boolean;
  error: string | null;
  onAction: (action: "start" | "pause" | "resume" | "reset") => void;
  onDismissError: () => void;
}

const actionLabels = {
  start: "Start",
  pause: "Pause",
  resume: "Resume",
  reset: "Reset",
} as const;

const pendingLabels: Partial<Record<SimulationOperation, string>> = {
  start: "Starting…",
  pause: "Pausing…",
  resume: "Resuming…",
  reset: "Resetting…",
};

export function SimulationControls({
  compact = false,
  state,
  pendingOperation,
  connected,
  error,
  onAction,
  onDismissError,
}: SimulationControlsProps) {
  const busy = pendingOperation !== null;
  const actions = [
    { action: "start" as const, enabled: state === "stopped", style: "button-primary" },
    { action: "pause" as const, enabled: state === "running", style: "button-secondary" },
    { action: "resume" as const, enabled: state === "paused", style: "button-primary" },
    { action: "reset" as const, enabled: true, style: "button-secondary" },
  ];

  return (
    <section
      aria-labelledby="simulation-controls-title"
      aria-busy={busy}
      className={`surface-card ${compact ? "p-4 sm:p-5" : "p-5 sm:p-7"}`}
    >
      <div className="flex flex-wrap items-start justify-between gap-3">
        <div>
          <p className="eyebrow">Simulation</p>
          <h2
            id="simulation-controls-title"
            className="mt-2 text-lg font-semibold tracking-tight text-white"
          >
            Lifecycle controls
          </h2>
        </div>
        <p
          aria-live="polite"
          aria-atomic="true"
          className="text-xs text-slate-300"
        >
          Backend state: <span className="font-semibold text-white">{state}</span>
        </p>
      </div>

      {!compact ? (
        <p id="lifecycle-help" className="mt-3 text-sm leading-6 text-slate-400">
          Reset is available in every state.
        </p>
      ) : null}

      <div className="mt-5 flex flex-wrap gap-3" aria-describedby={compact ? undefined : "lifecycle-help"}>
        {actions.map(({ action, enabled, style }) => {
          const isPending = pendingOperation === action;
          const disabled = !connected || busy || !enabled;
          return (
            <button
              key={action}
              type="button"
              className={style + " px-4 py-2.5 text-sm disabled:cursor-not-allowed disabled:opacity-45"}
              disabled={disabled}
              aria-describedby={compact ? undefined : "lifecycle-help"}
              onClick={() => onAction(action)}
            >
              {isPending ? pendingLabels[action] : actionLabels[action]}
            </button>
          );
        })}
      </div>

      {!connected && (
        <p className="mt-3 text-xs text-amber-200" role="status">
          Controls are disabled until the backend state is available.
        </p>
      )}
      {busy && !(compact && pendingOperation === "configuration") && (
        <p className="mt-3 text-sm text-cyan-100" role="status" aria-live="polite">
          {pendingOperation === "configuration"
            ? "Saving configuration…"
            : pendingLabels[pendingOperation]}
        </p>
      )}
      {error && (
        <div className="mt-4 flex flex-wrap items-start justify-between gap-3 rounded-xl border border-rose-300/20 bg-rose-950/30 p-3 text-sm text-rose-100">
          <p role="alert" aria-live="assertive" className="min-w-0 flex-1">
            {error}
          </p>
          <button
            type="button"
            className="rounded-lg px-2 py-1 text-xs font-semibold text-rose-100 underline underline-offset-4 focus-visible:outline focus-visible:outline-2 focus-visible:outline-cyan-200"
            onClick={onDismissError}
            aria-label="Dismiss simulation action error"
          >
            Dismiss
          </button>
        </div>
      )}
    </section>
  );
}
