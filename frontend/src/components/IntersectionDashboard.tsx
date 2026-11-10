import type { ApproachSnapshot, SignalState, SimulationSnapshot } from "../api/types";

interface Props { snapshot: SimulationSnapshot; }

const signalLabels: Record<SignalState, string> = {
  red: "RED", yellow: "YELLOW", green: "GREEN", unknown: "UNKNOWN",
};

function SignalIndicator({ signal }: { signal: SignalState }) {
  return (
    <div className={`signal-indicator signal-${signal}`} aria-label={`Signal ${signalLabels[signal]}`}>
      <span className="signal-light signal-light-red" aria-hidden="true" />
      <span className="signal-light signal-light-yellow" aria-hidden="true" />
      <span className="signal-light signal-light-green" aria-hidden="true" />
      <span className="signal-text">{signalLabels[signal]}</span>
    </div>
  );
}

function ApproachCard({ approach, active, capacity }: {
  approach: ApproachSnapshot; active: boolean; capacity: number;
}) {
  return (
    <article className={`approach-card${active ? " approach-card-active" : ""}`} aria-labelledby={`approach-${approach.id}`}>
      <div className="flex flex-wrap items-start justify-between gap-3">
        <div><p className="eyebrow">Approach</p><h3 id={`approach-${approach.id}`} className="mt-1 text-xl font-semibold text-white">{approach.name}</h3></div>
        {active && <span className="active-tag">Active approach</span>}
      </div>
      <div className="mt-4 flex items-center justify-between gap-3">
        <SignalIndicator signal={approach.signal} />
        <div className="text-right"><p className="text-2xl font-semibold tabular-nums text-white">{approach.queueLength}</p><p className="text-xs text-slate-400">Queued / {capacity} capacity</p></div>
      </div>
      <div className="mt-4 border-t border-white/10 pt-3">
        <div className="mb-2 flex items-center justify-between"><h4 className="text-sm font-semibold text-slate-200">Vehicle queue</h4><span className="text-xs text-slate-500">FIFO · front first</span></div>
        {approach.vehicles.length === 0 ? <p className="rounded-lg bg-slate-950/40 px-3 py-4 text-center text-sm text-slate-500">No vehicles waiting</p> : (
          <ol className="space-y-2" aria-label={`${approach.name} FIFO vehicle queue`}>
            {approach.vehicles.map((vehicle, index) => (
              <li key={`${vehicle.id}-${index}`} className="vehicle-row">
                <span className="vehicle-position">{index + 1}</span>
                <span className="min-w-0 flex-1"><span className="block text-sm font-semibold text-slate-100">Vehicle #{vehicle.id}</span><span className="block text-xs text-slate-400">{vehicle.type} · {vehicle.lane}</span></span>
                {index === 0 && <span className="text-[10px] font-semibold uppercase tracking-wider text-cyan-300">Front</span>}
              </li>
            ))}
          </ol>
        )}
        {approach.queueLength !== approach.vehicles.length && <p role="status" className="mt-2 text-xs text-amber-300">Backend reports {approach.queueLength} queued; {approach.vehicles.length} vehicle snapshots received.</p>}
      </div>
    </article>
  );
}

export function IntersectionDashboard({ snapshot }: Props) {
  return (
    <section className="mt-6 space-y-5" aria-labelledby="intersection-title">
      <div className="surface-card p-5 sm:p-6">
        <div className="flex flex-wrap items-end justify-between gap-3">
          <div><p className="eyebrow">Live backend snapshot</p><h2 id="intersection-title" className="mt-2 text-2xl font-semibold text-white">Intersection & approach queues</h2></div>
          <div className="text-right text-sm text-slate-400"><p>Simulation: <strong className="capitalize text-slate-100">{snapshot.state}</strong></p><p>Logical tick: <strong className="font-mono text-white">{snapshot.tickCount}</strong></p></div>
        </div>
        <div className="mt-5 grid gap-3 sm:grid-cols-2 xl:grid-cols-4">
          {snapshot.approaches.map((approach) => <ApproachCard key={approach.id} approach={approach} active={snapshot.activeApproach === approach.id} capacity={snapshot.config.maxQueue} />)}
        </div>
      </div>
    </section>
  );
}
