import type {
  ApproachSnapshot,
  SignalState,
  SimulationSnapshot,
} from "../api/types";
import type { BackendStatus, SimulationOperation } from "../hooks/useBackendStatus";
import { BackendConnectionCard } from "./BackendConnectionCard";
import { ConfigurationForm } from "./ConfigurationForm";
import { SimulationControls } from "./SimulationControls";

interface ControlCentreProps {
  status: BackendStatus;
  snapshot?: SimulationSnapshot;
  pendingOperation: SimulationOperation | null;
  isRefreshing: boolean;
  actionError: string | null;
  configurationError: Parameters<typeof ConfigurationForm>[0]["feedback"];
  configurationLocked: boolean;
  connected: boolean;
  onRetry: () => void;
  onAction: (action: "start" | "pause" | "resume" | "reset") => void;
  onDismissActionError: () => void;
  onUpdateConfiguration: Parameters<typeof ConfigurationForm>[0]["onSubmit"];
  onClearConfigurationError: () => void;
}

const signalTone: Record<SignalState, string> = {
  red: "bg-rose-400 shadow-[0_0_14px_rgba(251,113,133,0.55)]",
  yellow: "bg-amber-300 shadow-[0_0_14px_rgba(252,211,77,0.55)]",
  green: "bg-emerald-400 shadow-[0_0_14px_rgba(52,211,153,0.55)]",
  unknown: "bg-slate-500",
};

function ApproachSummaryCard({
  approach,
  active,
  capacity,
}: {
  approach: ApproachSnapshot;
  active: boolean;
  capacity: number;
}) {
  return (
    <article
      className={`approach-summary-card${active ? " approach-summary-card-active" : ""}`}
      aria-label={`${approach.name} approach: ${approach.signal}, ${approach.queueLength} vehicles queued${active ? ", active approach" : ""}`}
    >
      <div className="flex items-center justify-between gap-3">
        <div className="flex items-center gap-2.5">
          <span className={`signal-dot ${signalTone[approach.signal]}`} aria-hidden="true" />
          <h3 className="text-sm font-semibold text-white">{approach.name}</h3>
        </div>
        {active ? <span className="active-tag">Active</span> : null}
      </div>
      <div className="mt-3 flex items-end justify-between gap-2">
        <p className="text-xs font-semibold uppercase tracking-wider text-slate-400">
          {approach.signal}
        </p>
        <p className="text-right text-sm text-slate-300">
          <strong className="text-xl tabular-nums text-white">{approach.queueLength}</strong>
          <span className="ml-1 text-xs text-slate-500">/ {capacity} queued</span>
        </p>
      </div>
    </article>
  );
}

function PerformanceMetrics({ snapshot }: { snapshot: SimulationSnapshot }) {
  const metrics = [
    { label: "Generated", value: snapshot.statistics.vehiclesGenerated, unit: "vehicles" },
    { label: "Processed", value: snapshot.statistics.vehiclesProcessed, unit: "vehicles" },
    { label: "Waiting", value: snapshot.statistics.vehiclesWaiting, unit: "vehicles" },
    { label: "Average wait", value: snapshot.statistics.averageWaitingTicks.toFixed(1), unit: "ticks" },
    { label: "Total waiting", value: snapshot.statistics.totalWaitingTicks, unit: "ticks" },
    { label: "Processed / logical tick", value: snapshot.statistics.processedPerLogicalTick.toFixed(2), unit: "vehicles/tick" },
  ];

  return (
    <section aria-labelledby="performance-title" className="control-panel">
      <div className="section-heading-row">
        <div>
          <p className="eyebrow">Live performance</p>
          <h2 id="performance-title" className="section-title">Simulation throughput</h2>
        </div>
        <span className="subtle-chip">Tick {snapshot.tickCount.toLocaleString()}</span>
      </div>
      <dl aria-label="Performance overview" className="performance-grid">
        {metrics.map((metric) => (
          <div className="performance-card" key={metric.label}>
            <dt>{metric.label}</dt>
            <dd>{metric.value}</dd>
            <span>{metric.unit}</span>
          </div>
        ))}
      </dl>
      <p className="metric-footnote">
        Average wait counts completed vehicles. Processed per tick is cumulative, not a per-second rate.
      </p>
    </section>
  );
}

export function ControlCentre({
  status,
  snapshot,
  pendingOperation,
  isRefreshing,
  actionError,
  configurationError,
  configurationLocked,
  connected,
  onRetry,
  onAction,
  onDismissActionError,
  onUpdateConfiguration,
  onClearConfigurationError,
}: ControlCentreProps) {
  return (
    <section id="page-overview" aria-labelledby="overview-title">
      <header className="page-intro">
        <p className="eyebrow">Traffic operations / 01</p>
        <h1 id="overview-title" className="page-title">Control Centre</h1>
        <p className="page-description">
          Operate the simulation, tune its parameters and review live performance at a glance.
        </p>
      </header>

      <BackendConnectionCard
        compact
        status={status}
        onRetry={onRetry}
        isBusy={isRefreshing || pendingOperation !== null}
      />

      {snapshot ? (
        <>
          <section aria-labelledby="approach-summary-title" className="control-panel mt-5">
            <div className="section-heading-row">
              <div>
                <p className="eyebrow">Intersection status</p>
                <h2 id="approach-summary-title" className="section-title">Approach summary</h2>
              </div>
              <span className="subtle-chip">
                Active: {snapshot.activeApproach === "unknown"
                  ? "Unavailable"
                  : snapshot.approaches.find((approach) => approach.id === snapshot.activeApproach)?.name ?? "Unavailable"}
              </span>
            </div>
            <div className="approach-summary-grid">
              {snapshot.approaches.map((approach) => (
                <ApproachSummaryCard
                  key={approach.id}
                  approach={approach}
                  active={snapshot.activeApproach === approach.id}
                  capacity={snapshot.config.maxQueue}
                />
              ))}
            </div>
          </section>

          <PerformanceMetrics snapshot={snapshot} />

          <div className="mt-5 grid items-start gap-5 xl:grid-cols-[minmax(17rem,0.8fr)_minmax(0,1.2fr)]">
            <SimulationControls
              state={snapshot.state}
              pendingOperation={pendingOperation}
              connected={connected}
              error={actionError}
              onAction={onAction}
              onDismissError={onDismissActionError}
            />
            <ConfigurationForm
              acceptedConfig={snapshot.config}
              locked={configurationLocked}
              connected={connected}
              busy={pendingOperation !== null || isRefreshing}
              feedback={configurationError}
              onSubmit={onUpdateConfiguration}
              onClearFeedback={onClearConfigurationError}
            />
          </div>
        </>
      ) : (
        <p className="mt-5 rounded-xl border border-white/10 bg-slate-950/40 p-4 text-sm text-slate-400" role="status">
          Performance, approach and configuration details appear when the backend snapshot is available.
        </p>
      )}
    </section>
  );
}
