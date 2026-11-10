import type { BackendStatus } from "../hooks/useBackendStatus";
import { StatusBadge } from "./StatusBadge";

interface BackendConnectionCardProps {
  status: BackendStatus;
  onRetry: () => void;
  isBusy: boolean;
}

export function BackendConnectionCard({
  status,
  onRetry,
  isBusy,
}: BackendConnectionCardProps) {
  const connected = status.kind === "connected";
  const loading = status.kind === "loading";
  const lastKnownSnapshot = status.kind === "error" ? status.snapshot : undefined;
  const displayedState = connected
    ? status.snapshot.state
    : lastKnownSnapshot?.state;

  return (
    <section
      aria-labelledby="connection-title"
      className="surface-card relative overflow-hidden p-6 sm:p-8"
    >
      <div aria-hidden="true" className="connection-glow" />
      <div className="relative z-10 flex flex-col gap-6">
        <div className="flex flex-wrap items-start justify-between gap-4">
          <div>
            <p className="eyebrow">Backend connection</p>
            <h2
              id="connection-title"
              className="mt-2 text-xl font-semibold tracking-tight text-white sm:text-2xl"
            >
              {connected
                ? "The service is responding"
                : lastKnownSnapshot
                  ? "Backend connection needs attention"
                  : "Simulation service"}
            </h2>
          </div>
          {displayedState ? (
            <StatusBadge state={displayedState} />
          ) : (
            <span
              className={
                "state-badge " +
                (loading ? "state-badge-loading" : "state-badge-error")
              }
              role="status"
            >
              <span aria-hidden="true" className="state-indicator" />
              {loading ? "Connecting" : "Unavailable"}
            </span>
          )}
        </div>

        <div className="connection-divider" />

        {connected ? (
          <div className="flex flex-col gap-4 sm:flex-row sm:items-end sm:justify-between">
            <div>
              <p className="text-sm leading-6 text-slate-300">
                Live status is read from the C++ REST API. This shell does not
                change simulation state.
              </p>
              <p className="mt-2 font-mono text-xs text-slate-500">
                GET /api/simulation/state
              </p>
            </div>
            <div className="rounded-xl border border-white/10 bg-slate-950/50 px-4 py-3 text-sm text-slate-300">
              Tick{" "}
              <span className="ml-1 font-mono font-semibold text-white">
                {status.snapshot.tickCount}
              </span>
            </div>
          </div>
        ) : (
          <div
            aria-live="polite"
            className="flex flex-col gap-4 sm:flex-row sm:items-center sm:justify-between"
          >
            <p
              className={
                loading
                  ? "text-sm leading-6 text-slate-300"
                  : "max-w-2xl text-sm leading-6 text-rose-200"
              }
              role={loading ? "status" : "alert"}
            >
              {loading
                ? "Checking the configured backend through the local API proxy…"
                : status.message}
            </p>
            {!loading && (
              <button
                className="button-secondary shrink-0"
                type="button"
                onClick={onRetry}
                disabled={isBusy}
                aria-label="Retry backend connection"
              >
                {isBusy ? "Refreshing…" : "Retry connection"}
              </button>
            )}
          </div>
        )}
      </div>
    </section>
  );
}
