
import { useEffect, useState } from "react";
import { BackendConnectionCard } from "./components/BackendConnectionCard";
import { BrandMark } from "./components/BrandMark";
import { ControlCentre } from "./components/ControlCentre";
import { SectionNavigation } from "./components/SectionNavigation";
import { SimulationMonitor } from "./components/SimulationMonitor";
import { StatusBadge } from "./components/StatusBadge";
import { useBackendStatus } from "./hooks/useBackendStatus";

type WorkspacePage = "overview" | "intersection";

function getWorkspacePage(): WorkspacePage {
  return window.location.hash === "#intersection" ? "intersection" : "overview";
}

function HeaderTelemetry({ status }: { status: ReturnType<typeof useBackendStatus>["status"] }) {
  const snapshot = status.kind === "connected"
    ? status.snapshot
    : status.kind === "error"
      ? status.snapshot
      : undefined;
  const connectionLabel = status.kind === "connected"
    ? "API connected"
    : status.kind === "loading"
      ? "Connecting"
      : "API unavailable";

  return (
    <div className="header-telemetry" aria-label="Backend and simulation status">
      <span className={`backend-indicator backend-indicator-${status.kind}`} role="status">
        <i aria-hidden="true" />
        {connectionLabel}
      </span>
      {snapshot ? <StatusBadge state={snapshot.state} /> : <span className="header-state-pending">State pending</span>}
      <span className="header-tick" aria-label={`Current logical tick ${snapshot?.tickCount ?? "unavailable"}`}>
        <span>Tick</span>
        <strong>{snapshot ? snapshot.tickCount.toLocaleString() : "—"}</strong>
      </span>
    </div>
  );
}

export default function App() {
  const {
    status,
    refresh,
    isRefreshing,
    pendingOperation,
    actionError,
    configurationError,
    runAction,
    updateConfiguration,
    clearActionError,
    clearConfigurationError,
  } = useBackendStatus();

  const [currentPage, setCurrentPage] = useState<WorkspacePage>(getWorkspacePage);

  useEffect(() => {
    const syncPage = () => setCurrentPage(getWorkspacePage());
    window.addEventListener("hashchange", syncPage);
    return () => window.removeEventListener("hashchange", syncPage);
  }, []);

  const snapshot =
    status.kind === "connected"
      ? status.snapshot
      : status.kind === "error"
        ? status.snapshot
        : undefined;

  const connected = status.kind === "connected";

  const configurationLocked =
    !snapshot ||
    snapshot.state !== "stopped" ||
    snapshot.tickCount !== 0 ||
    snapshot.statistics.vehiclesGenerated !== 0;

  return (
    <div className="min-h-screen">
      <a
        href="#overview"
        className="sr-only focus:not-sr-only focus:fixed focus:left-4 focus:top-4 focus:z-50 focus:rounded-lg focus:bg-slate-900 focus:px-4 focus:py-3 focus:text-white"
      >
        Skip to main content
      </a>

      <div className="page-shell">
        <header className="app-header">
          <a
            className="flex min-w-0 items-center gap-3"
            href="#overview"
            aria-label="Traffic Management System home"
          >
            <BrandMark />
            <span className="min-w-0">
              <span className="block truncate text-sm font-semibold tracking-wide text-white">
                Traffic Management System
              </span>
              <span className="mt-0.5 block text-[10px] font-medium uppercase tracking-[0.2em] text-slate-500">
                Operations workspace
              </span>
            </span>
          </a>

          <div className="hidden md:block">
            <SectionNavigation currentPage={currentPage} />
          </div>

          <HeaderTelemetry status={status} />
        </header>

        <div className="mobile-navigation md:hidden">
          <SectionNavigation currentPage={currentPage} />
        </div>

        <main id="overview" className="mx-auto w-full max-w-7xl px-4 pb-16 pt-8 sm:px-6 sm:pt-10 lg:px-8">
          <section
            id="control-centre"
            aria-label="Control Centre page"
            hidden={currentPage !== "overview"}
          >
            <ControlCentre
              status={status}
              snapshot={snapshot}
              pendingOperation={pendingOperation}
              isRefreshing={isRefreshing}
              actionError={actionError}
              configurationError={configurationError}
              configurationLocked={configurationLocked}
              connected={connected}
              onRetry={() => void refresh()}
              onAction={(action) => void runAction(action)}
              onDismissActionError={clearActionError}
              onUpdateConfiguration={updateConfiguration}
              onClearConfigurationError={clearConfigurationError}
            />
          </section>

          <section
            id="intersection"
            aria-label="Simulation Monitor page"
            hidden={currentPage !== "intersection"}
          >
            {snapshot ? (
              <SimulationMonitor
                snapshot={snapshot}
                connected={connected}
                pendingOperation={pendingOperation}
                actionError={actionError}
                onAction={(action) => void runAction(action)}
                onDismissActionError={clearActionError}
              />
            ) : (
              <section className="page-intro">
                <p className="eyebrow">Traffic operations / 02</p>
                <h1 className="page-title">Simulation Monitor</h1>
                <BackendConnectionCard
                  status={status}
                  onRetry={() => void refresh()}
                  isBusy={isRefreshing || pendingOperation !== null}
                />
              </section>
            )}
          </section>

          <footer className="mt-14 flex flex-col gap-2 border-t border-white/10 pt-5 text-xs text-slate-500 sm:flex-row sm:items-center sm:justify-between">
            <p>Simulation rules and state remain owned by the C++ backend.</p>
            <p className="font-mono">REST base path: /api</p>
          </footer>
        </main>
      </div>
    </div>
  );
}
