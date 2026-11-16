import { useEffect, useRef, useState } from "react";
import type {
  ApproachId,
  ApproachSnapshot,
  SignalState,
  SimulationSnapshot,
} from "../api/types";
import type { SimulationOperation } from "../hooks/useBackendStatus";
import { SimulationControls } from "./SimulationControls";

interface SimulationMonitorProps {
  snapshot: SimulationSnapshot;
  connected: boolean;
  pendingOperation: SimulationOperation | null;
  actionError: string | null;
  onAction: (action: "start" | "pause" | "resume" | "reset") => void;
  onDismissActionError: () => void;
}

interface MonitorEvent {
  id: number;
  tick: number;
  category: "lifecycle" | "signal" | "arrival" | "processing";
  title: string;
  detail: string;
}

const signalColors: Record<SignalState, string> = {
  red: "#fb7185",
  yellow: "#fbbf24",
  green: "#4ade80",
  unknown: "#475569",
};

const directionAngle: Record<ApproachId, number> = {
  north: 90,
  east: 180,
  south: -90,
  west: 0,
};

function vehiclePosition(id: ApproachId, index: number) {
  const spacing = 34;
  switch (id) {
    case "north":
      return { x: 420, y: 222 - index * spacing };
    case "east":
      return { x: 618 + index * spacing, y: 420 };
    case "south":
      return { x: 540, y: 498 + index * spacing };
    case "west":
      return { x: 342 - index * spacing, y: 300 };
  }
}

function RoadVehicle({
  approach,
  index,
  front,
}: {
  approach: ApproachSnapshot;
  index: number;
  front: boolean;
}) {
  const position = vehiclePosition(approach.id, index);
  return (
    <g
      transform={`translate(${position.x} ${position.y}) rotate(${directionAngle[approach.id]})`}
      aria-hidden="true"
    >
      <rect x="-16" y="-10" width="32" height="20" rx="6" fill={front ? "#67e8f9" : "#cbd5e1"} stroke={front ? "#ecfeff" : "#f8fafc"} strokeWidth="1.5" />
      <rect x="6" y="-7" width="7" height="14" rx="3" fill="#334155" />
      <path d="M-10 -7v14" stroke="#64748b" strokeWidth="2" />
      <circle cx="13" cy="-7" r="2" fill="#fef3c7" />
      <circle cx="13" cy="7" r="2" fill="#fef3c7" />
    </g>
  );
}

function TrafficSignal({
  x,
  y,
  state,
  active,
  label,
}: {
  x: number;
  y: number;
  state: SignalState;
  active: boolean;
  label: string;
}) {
  const lenses: SignalState[] = ["red", "yellow", "green"];
  return (
    <g transform={`translate(${x} ${y})`} aria-label={`${label} signal ${state}`}>
      {active ? <circle r="31" fill="none" stroke="#22d3ee" strokeWidth="2" opacity="0.85" /> : null}
      <rect x="-13" y="-27" width="26" height="54" rx="10" fill="#08111e" stroke="#64748b" strokeWidth="1.5" />
      {lenses.map((lens, index) => (
        <circle
          key={lens}
          cx="0"
          cy={-16 + index * 16}
          r="5.5"
          fill={lens === state ? signalColors[lens] : "#263446"}
          stroke={lens === state ? "#f8fafc" : "#475569"}
          strokeWidth={lens === state ? 1 : 0.5}
        />
      ))}
    </g>
  );
}

function IntersectionScene({ snapshot }: { snapshot: SimulationSnapshot }) {
  const approachMap = Object.fromEntries(
    snapshot.approaches.map((approach) => [approach.id, approach]),
  ) as Record<ApproachId, ApproachSnapshot>;

  return (
    <figure className="scene-frame" aria-labelledby="scene-heading">
      <figcaption className="scene-caption">
        <div>
          <p className="eyebrow">Live intersection plan</p>
          <h2 id="scene-heading" className="section-title">Four-way signalized junction</h2>
        </div>
        <span className="subtle-chip">Top-down · inbound queues</span>
      </figcaption>
      <div className="scene-canvas">
        <svg
          className="intersection-map"
          viewBox="0 0 960 720"
          role="img"
          aria-labelledby="intersection-map-title"
          aria-describedby="intersection-map-description"
        >
          <title id="intersection-map-title">Live four-way intersection and vehicle queues</title>
          <desc id="intersection-map-description">
            Actual backend vehicles are shown front-first on their approach road, facing the stop line. {snapshot.approaches.map((approach) => `${approach.name}: ${approach.signal}, ${approach.queueLength} queued`).join(". ")}.
          </desc>
          <rect width="960" height="720" fill="#0b1522" />
          <rect x="350" y="0" width="260" height="720" fill="#1a2939" />
          <rect x="0" y="230" width="960" height="260" fill="#1a2939" />

          <path d="M350 0v720 M610 0v720 M0 230h960 M0 490h960" fill="none" stroke="#334155" strokeWidth="3" />
          <path d="M480 0v230 M480 490v230 M0 360h350 M610 360h350" fill="none" stroke="#8290a1" strokeWidth="2" strokeDasharray="20 16" opacity="0.7" />
          <path d="M350 230h130 M480 490h130 M350 230v130 M610 360v130" fill="none" stroke="#f8fafc" strokeWidth="6" opacity="0.9" />

          <text x="390" y="36" fill="#94a3b8" fontSize="15" fontWeight="700" letterSpacing="2">NORTH · INBOUND</text>
          <text x="768" y="337" fill="#94a3b8" fontSize="15" fontWeight="700" letterSpacing="2">EAST · INBOUND</text>
          <text x="490" y="690" fill="#94a3b8" fontSize="15" fontWeight="700" letterSpacing="2">SOUTH · INBOUND</text>
          <text x="28" y="280" fill="#94a3b8" fontSize="15" fontWeight="700" letterSpacing="2">WEST · INBOUND</text>

          <path d="M420 334v48m0 0-8-9m8 9 8-9 M486 420h-48m0 0 9-8m-9 8 9 8 M540 386v-48m0 0-8 9m8-9 8 9 M474 300h48m0 0-9-8m9 8-9 8" fill="none" stroke="#67e8f9" strokeWidth="3" strokeLinecap="round" strokeLinejoin="round" opacity="0.7" />

          {snapshot.approaches.map((approach) => {
            const visibleVehicles = approach.vehicles.slice(0, 6);
            return visibleVehicles.map((_, index) => (
              <RoadVehicle
                key={`${approach.id}-${approach.vehicles[index]?.id}-${index}`}
                approach={approach}
                index={index}
                front={index === 0}
              />
            ));
          })}

          <TrafficSignal
            x={330}
            y={194}
            label="North"
            state={approachMap.north.signal}
            active={snapshot.activeApproach === "north"}
          />
          <TrafficSignal
            x={626}
            y={330}
            label="East"
            state={approachMap.east.signal}
            active={snapshot.activeApproach === "east"}
          />
          <TrafficSignal
            x={630}
            y={526}
            label="South"
            state={approachMap.south.signal}
            active={snapshot.activeApproach === "south"}
          />
          <TrafficSignal
            x={330}
            y={390}
            label="West"
            state={approachMap.west.signal}
            active={snapshot.activeApproach === "west"}
          />
        </svg>
      </div>
      <div className="scene-legend">
        <span><i className="legend-swatch legend-red" />Red</span>
        <span><i className="legend-swatch legend-yellow" />Yellow</span>
        <span><i className="legend-swatch legend-green" />Green</span>
        <span className="scene-note">Vehicle markers are actual queued vehicles; the map shows the six nearest per approach.</span>
      </div>
    </figure>
  );
}

function QueueCard({
  approach,
  capacity,
  active,
}: {
  approach: ApproachSnapshot;
  capacity: number;
  active: boolean;
}) {
  const isFull = approach.queueLength >= capacity;
  const status = isFull ? "At capacity" : approach.queueLength === 0 ? "Clear" : "Queue building";
  return (
    <article className={`monitor-queue-card${active ? " monitor-queue-card-active" : ""}`} aria-labelledby={`monitor-${approach.id}`}>
      <header className="flex flex-wrap items-start justify-between gap-2">
        <div>
          <p className="eyebrow">{active ? "Active approach" : "Approach"}</p>
          <h3 id={`monitor-${approach.id}`} className="mt-1 text-lg font-semibold text-white">{approach.name}</h3>
        </div>
        <span className={`queue-state ${isFull ? "queue-state-full" : approach.queueLength === 0 ? "queue-state-clear" : "queue-state-active"}`}>{status}</span>
      </header>
      <div className="mt-3 flex items-end justify-between border-b border-white/10 pb-3">
        <p className="text-sm text-slate-400">Queue count</p>
        <p className="font-mono text-2xl font-semibold tabular-nums text-white">
          {approach.queueLength}<span className="ml-1 text-sm text-slate-500">/ {capacity}</span>
        </p>
      </div>
      <p className="mt-3 text-xs text-slate-400">
        FIFO · front first {approach.vehicles[0] ? <span className="ml-1 text-cyan-200">Front: #{approach.vehicles[0].id}</span> : null}
      </p>
      {approach.vehicles.length > 0 ? (
        <ol className="monitor-fifo-list" aria-label={`${approach.name} FIFO vehicle queue`}>
          {approach.vehicles.map((vehicle, index) => (
            <li key={`${vehicle.id}-${index}`} className={index === 0 ? "monitor-fifo-front" : ""}>
              <span className="fifo-position">{index + 1}</span>
              <span className="min-w-0 flex-1">
                <strong>Vehicle #{vehicle.id}</strong>
                <small>{vehicle.type} · {vehicle.lane}</small>
              </span>
              {index === 0 ? <span className="fifo-front-label">FRONT</span> : null}
            </li>
          ))}
        </ol>
      ) : (
        <p className="queue-empty">No vehicles waiting</p>
      )}
      {approach.queueLength !== approach.vehicles.length ? (
        <p className="mt-3 text-xs leading-5 text-amber-200" role="status">
          Backend count is {approach.queueLength}; detailed vehicle data is available for {approach.vehicles.length}.
        </p>
      ) : null}
    </article>
  );
}

function useDetectedEvents(snapshot: SimulationSnapshot) {
  const previousSnapshot = useRef<SimulationSnapshot | null>(null);
  const nextId = useRef(1);
  const [events, setEvents] = useState<MonitorEvent[]>([]);

  useEffect(() => {
    const previous = previousSnapshot.current;
    if (!previous) {
      previousSnapshot.current = snapshot;
      return;
    }

    const detected: Omit<MonitorEvent, "id">[] = [];
    const add = (event: Omit<MonitorEvent, "id">) => detected.push(event);
    const resetObserved = snapshot.tickCount < previous.tickCount ||
      (snapshot.state === "stopped" && previous.state !== "stopped");

    if (snapshot.state !== previous.state) {
      if (snapshot.state === "running" && previous.state === "stopped") {
        add({ tick: snapshot.tickCount, category: "lifecycle", title: "Simulation started", detail: "State change observed in backend snapshots." });
      } else if (snapshot.state === "paused" && previous.state === "running") {
        add({ tick: snapshot.tickCount, category: "lifecycle", title: "Simulation paused", detail: "State change observed in backend snapshots." });
      } else if (snapshot.state === "running" && previous.state === "paused") {
        add({ tick: snapshot.tickCount, category: "lifecycle", title: "Simulation resumed", detail: "State change observed in backend snapshots." });
      } else if (snapshot.state === "stopped" && previous.state !== "stopped") {
        add({ tick: snapshot.tickCount, category: "lifecycle", title: "Simulation reset", detail: "Stopped state observed after simulation activity." });
      }
    } else if (resetObserved && previous.tickCount > 0) {
      add({ tick: snapshot.tickCount, category: "lifecycle", title: "Simulation reset", detail: "Tick counter reset observed in backend snapshots." });
    }

    if (!resetObserved) {
      for (const current of snapshot.approaches) {
        const prior = previous.approaches.find((item) => item.id === current.id);
        if (prior && current.signal !== prior.signal && current.signal !== "unknown" && prior.signal !== "unknown") {
          add({ tick: snapshot.tickCount, category: "signal", title: `${current.name} signal changed`, detail: `${prior.signal.toUpperCase()} → ${current.signal.toUpperCase()} observed in backend snapshots.` });
        }
      }

      for (const current of snapshot.statistics.approaches) {
        const prior = previous.statistics.approaches.find((item) => item.id === current.id);
        if (!prior) continue;
        const arrivals = current.vehiclesGenerated - prior.vehiclesGenerated;
        const processed = current.vehiclesProcessed - prior.vehiclesProcessed;
        const approachName = snapshot.approaches.find((item) => item.id === current.id)?.name ?? current.id;
        if (arrivals > 0) {
          add({ tick: snapshot.tickCount, category: "arrival", title: `${arrivals} vehicle${arrivals === 1 ? "" : "s"} arrived`, detail: `${approachName} approach · cumulative count increased.` });
        }
        if (processed > 0) {
          add({ tick: snapshot.tickCount, category: "processing", title: `${processed} vehicle${processed === 1 ? "" : "s"} processed`, detail: `${approachName} approach · cumulative count increased.` });
        }
      }
    }

    previousSnapshot.current = snapshot;
    if (detected.length > 0) {
      const withIds = detected.map((event) => ({ ...event, id: nextId.current++ }));
      setEvents((current) => [...withIds.reverse(), ...current].slice(0, 24));
    }
  }, [snapshot]);

  return events;
}

function ActivityLog({ snapshot }: { snapshot: SimulationSnapshot }) {
  const events = useDetectedEvents(snapshot);
  return (
    <section className="monitor-panel" aria-labelledby="activity-title" aria-live="polite">
      <div className="section-heading-row">
        <div>
          <p className="eyebrow">Observed changes</p>
          <h2 id="activity-title" className="section-title">Activity log</h2>
        </div>
        <span className="subtle-chip">Snapshot detected</span>
      </div>
      <p className="activity-disclaimer">
        Events are inferred from consecutive backend snapshots, not read from an event stream. Brief changes between polls may not appear.
      </p>
      {events.length > 0 ? (
        <ol className="activity-list" aria-label="Detected simulation activity">
          {events.map((event) => (
            <li key={event.id} className="activity-item">
              <span className={`activity-mark activity-mark-${event.category}`} aria-hidden="true" />
              <div className="min-w-0 flex-1">
                <p className="text-sm font-semibold text-slate-100">{event.title}</p>
                <p className="mt-0.5 text-xs leading-5 text-slate-400">{event.detail}</p>
              </div>
              <span className="activity-tick">Tick {event.tick}</span>
            </li>
          ))}
        </ol>
      ) : (
        <p className="activity-empty">
          No changes detected yet. This page starts from the current snapshot and only records changes observed after that point.
        </p>
      )}
    </section>
  );
}

export function SimulationMonitor({
  snapshot,
  connected,
  pendingOperation,
  actionError,
  onAction,
  onDismissActionError,
}: SimulationMonitorProps) {
  return (
    <section id="page-intersection" aria-labelledby="intersection-page-title">
      <header className="page-intro monitor-page-intro">
        <p className="eyebrow">Traffic operations / 02</p>
        <h1 id="intersection-page-title" className="page-title">Simulation Monitor</h1>
        <p className="page-description">
          Live backend queues and signal phases arranged around a top-down view of the junction.
        </p>
      </header>

      <IntersectionScene snapshot={snapshot} />

      <section className="mt-5" aria-labelledby="queues-title">
        <div className="section-heading-row mb-3">
          <div>
            <p className="eyebrow">Authoritative queue snapshots</p>
            <h2 id="queues-title" className="section-title">Approach queues</h2>
          </div>
          <span className="subtle-chip">FIFO · front vehicle first</span>
        </div>
        <div className="monitor-queue-grid">
          {snapshot.approaches.map((approach) => (
            <QueueCard
              key={approach.id}
              approach={approach}
              capacity={snapshot.config.maxQueue}
              active={snapshot.activeApproach === approach.id}
            />
          ))}
        </div>
      </section>

      <div className="mt-5 grid items-start gap-5 xl:grid-cols-[minmax(18rem,0.8fr)_minmax(0,1.2fr)]">
        <SimulationControls
          compact
          state={snapshot.state}
          pendingOperation={pendingOperation}
          connected={connected}
          error={actionError}
          onAction={onAction}
          onDismissError={onDismissActionError}
        />
        <ActivityLog snapshot={snapshot} />
      </div>

      <p className="mt-4 text-xs leading-5 text-slate-500">
        No countdown is displayed because the API snapshot does not provide a phase timer. Vehicles are positioned as queues, not animated, because the backend does not expose movement coordinates.
      </p>
    </section>
  );
}

// Kept as a named alias for callers that used the earlier dashboard component.
export { SimulationMonitor as IntersectionDashboard };
