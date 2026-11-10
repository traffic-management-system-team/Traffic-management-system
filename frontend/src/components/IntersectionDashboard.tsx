
import { useEffect, useRef, useState } from "react";
import type {
  ApproachId,
  ApproachSnapshot,
  SignalState,
  SimulationSnapshot,
} from "../api/types";

interface Props {
  snapshot: SimulationSnapshot;
}

interface ApproachActivity {
  arrivals: number;
  processed: number;
}

const signalLabels: Record<SignalState, string> = {
  red: "RED",
  yellow: "YELLOW",
  green: "GREEN",
  unknown: "UNKNOWN",
};

function SignalIndicator({
  signal,
  approachName,
}: {
  signal: SignalState;
  approachName: string;
}) {
  return (
    <div
      className={`signal-indicator signal-${signal}`}
      role="img"
      aria-label={`${approachName} approach signal: ${signalLabels[signal]}`}
    >
      <span className="signal-light signal-light-red" aria-hidden="true" />
      <span className="signal-light signal-light-yellow" aria-hidden="true" />
      <span className="signal-light signal-light-green" aria-hidden="true" />
      <span className="signal-text" aria-hidden="true">
        {signalLabels[signal]}
      </span>
    </div>
  );
}

function ApproachCard({
  approach,
  active,
  capacity,
  activity,
}: {
  approach: ApproachSnapshot;
  active: boolean;
  capacity: number;
  activity: ApproachActivity;
}) {
  return (
    <article
      className={`approach-card${active ? " approach-card-active" : ""}`}
      aria-labelledby={`approach-${approach.id}`}
    >
      <div className="flex flex-wrap items-start justify-between gap-3">
        <div>
          <p className="eyebrow">Approach</p>
          <h3
            id={`approach-${approach.id}`}
            className="mt-1 text-xl font-semibold text-white"
          >
            {approach.name}
          </h3>
        </div>
        {active && <span className="active-tag">Active approach</span>}
      </div>

      <div className="mt-4 flex items-center justify-between gap-3">
        <SignalIndicator
          signal={approach.signal}
          approachName={approach.name}
        />
        <div className="text-right">
          <p className="text-2xl font-semibold tabular-nums text-white">
            {approach.queueLength}
          </p>
          <p className="text-xs text-slate-400">
            Queued / {capacity} capacity
          </p>
        </div>
      </div>

      <div className="mt-3 rounded-lg bg-slate-950/40 px-3 py-2">
        <p className="text-xs font-semibold text-slate-300">
          Since previous update
        </p>
        <p className="mt-1 text-sm text-slate-400">
          <span className="text-emerald-300">
            +{activity.arrivals} generated
          </span>
          {" · "}
          <span className="text-cyan-300">
            {activity.processed} processed
          </span>
        </p>
      </div>

      <div className="mt-4 border-t border-white/10 pt-3">
        <div className="mb-2 flex items-center justify-between">
          <h4 className="text-sm font-semibold text-slate-200">
            Vehicle queue
          </h4>
          <span className="text-xs text-slate-500">FIFO · front first</span>
        </div>

        {approach.vehicles.length === 0 ? (
          <p className="rounded-lg bg-slate-950/40 px-3 py-4 text-center text-sm text-slate-500">
            No vehicles waiting
          </p>
        ) : (
          <ol
            className="space-y-2"
            aria-label={`${approach.name} FIFO vehicle queue`}
          >
            {approach.vehicles.map((vehicle, index) => (
              <li key={`${vehicle.id}-${index}`} className="vehicle-row">
                <span className="vehicle-position">{index + 1}</span>
                <span className="min-w-0 flex-1">
                  <span className="block text-sm font-semibold text-slate-100">
                    Vehicle #{vehicle.id}
                  </span>
                  <span className="block text-xs text-slate-400">
                    {vehicle.type} · {vehicle.lane}
                  </span>
                </span>
                {index === 0 && (
                  <span className="text-[10px] font-semibold uppercase tracking-wider text-cyan-300">
                    Front
                  </span>
                )}
              </li>
            ))}
          </ol>
        )}

        {approach.queueLength !== approach.vehicles.length && (
          <p role="status" className="mt-2 text-xs text-amber-300">
            Backend reports {approach.queueLength} queued;{" "}
            {approach.vehicles.length} vehicle snapshots received.
          </p>
        )}
      </div>
    </article>
  );
}

const emptyActivity: ApproachActivity = {
  arrivals: 0,
  processed: 0,
};

export function IntersectionDashboard({ snapshot }: Props) {
  const previousSnapshotRef = useRef<SimulationSnapshot | null>(null);

  const [activityByApproach, setActivityByApproach] = useState<
    Partial<Record<ApproachId, ApproachActivity>>
  >({});

  useEffect(() => {
    const previous = previousSnapshotRef.current;

    const nextActivity: Partial<
      Record<ApproachId, ApproachActivity>
    > = {};

    for (const current of snapshot.statistics.approaches) {
      const previousApproach = previous?.statistics.approaches.find(
        (item) => item.id === current.id,
      );

      nextActivity[current.id] = {
        arrivals: previousApproach
          ? Math.max(
              0,
              current.vehiclesGenerated -
                previousApproach.vehiclesGenerated,
            )
          : 0,
        processed: previousApproach
          ? Math.max(
              0,
              current.vehiclesProcessed -
                previousApproach.vehiclesProcessed,
            )
          : 0,
      };
    }

    setActivityByApproach(nextActivity);
    previousSnapshotRef.current = snapshot;
  }, [snapshot]);

  return (
    <section
      className="mt-6 space-y-5"
      aria-labelledby="intersection-title"
    >
      <div className="surface-card p-5 sm:p-6">
        <div className="flex flex-wrap items-end justify-between gap-3">
          <div>
            <p className="eyebrow">Live backend snapshot</p>
            <h2
              id="intersection-title"
              className="mt-2 text-2xl font-semibold text-white"
            >
              Intersection & approach queues
            </h2>
          </div>

          <div className="text-right text-sm text-slate-400">
            <p>
              Simulation:{" "}
              <strong className="capitalize text-slate-100">
                {snapshot.state}
              </strong>
            </p>
            <p>
              Logical tick:{" "}
              <strong className="font-mono text-white">
                {snapshot.tickCount}
              </strong>
            </p>
          </div>
        </div>

        <dl
          className="mt-5 grid grid-cols-2 gap-3 sm:grid-cols-4"
          aria-label="Simulation statistics"
        >
          <div className="rounded-xl border border-white/10 bg-slate-950/40 p-3">
            <dt className="text-xs font-medium text-slate-400">Vehicles generated</dt>
            <dd className="mt-2 text-2xl font-semibold tabular-nums text-white">
              {snapshot.statistics.vehiclesGenerated}
            </dd>
          </div>
          <div className="rounded-xl border border-white/10 bg-slate-950/40 p-3">
            <dt className="text-xs font-medium text-slate-400">Vehicles processed</dt>
            <dd className="mt-2 text-2xl font-semibold tabular-nums text-white">
              {snapshot.statistics.vehiclesProcessed}
            </dd>
          </div>
          <div className="rounded-xl border border-white/10 bg-slate-950/40 p-3">
            <dt className="text-xs font-medium text-slate-400">Vehicles waiting</dt>
            <dd className="mt-2 text-2xl font-semibold tabular-nums text-white">
              {snapshot.statistics.vehiclesWaiting}
            </dd>
          </div>
          <div className="rounded-xl border border-white/10 bg-slate-950/40 p-3">
            <dt className="text-xs font-medium text-slate-400">Average wait · ticks</dt>
            <dd className="mt-2 text-2xl font-semibold tabular-nums text-white">
              {snapshot.statistics.averageWaitingTicks.toFixed(1)}
            </dd>
          </div>
        </dl>

        <div className="mt-5 grid gap-3 sm:grid-cols-2 xl:grid-cols-4">
          {snapshot.approaches.map((approach) => (
            <ApproachCard
              key={approach.id}
              approach={approach}
              active={snapshot.activeApproach === approach.id}
              capacity={snapshot.config.maxQueue}
              activity={activityByApproach[approach.id] ?? emptyActivity}
            />
          ))}
        </div>
      </div>
    </section>
  );
}
