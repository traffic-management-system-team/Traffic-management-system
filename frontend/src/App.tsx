
import { BackendConnectionCard } from "./components/BackendConnectionCard";
import { ConfigurationForm } from "./components/ConfigurationForm";
import { BrandMark } from "./components/BrandMark";
import { SectionNavigation } from "./components/SectionNavigation";
import { SimulationControls } from "./components/SimulationControls";
import { IntersectionDashboard } from "./components/IntersectionDashboard";
import { useBackendStatus } from "./hooks/useBackendStatus";

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
            <SectionNavigation />
          </div>

          <span className="header-tag">
            <span aria-hidden="true" className="header-tag-dot" />
            Live simulation dashboard
          </span>
        </header>

        <div className="md:hidden">
          <SectionNavigation />
        </div>

        <main
          id="overview"
          className="mx-auto w-full max-w-6xl px-4 pb-16 pt-12 sm:px-6 sm:pt-16 lg:px-8"
        >
          <section className="mb-10 max-w-3xl">
            <p className="eyebrow">Traffic operations / 01</p>

            <h1 className="mt-4 text-4xl font-semibold tracking-tight text-white sm:text-5xl">
              Intersection overview
            </h1>

            <p className="mt-4 max-w-2xl text-base leading-7 text-slate-400">
              Live signals, FIFO vehicle queues and statistics are read from the
              authoritative C++ simulation snapshot.
            </p>
          </section>

          <BackendConnectionCard
            status={status}
            onRetry={() => void refresh()}
            isBusy={isRefreshing || pendingOperation !== null}
          />

          {snapshot && <IntersectionDashboard snapshot={snapshot} />}

          {snapshot && (
            <div className="mt-6 grid gap-5 lg:grid-cols-[minmax(0,0.9fr)_minmax(0,1.1fr)]">
              <SimulationControls
                state={snapshot.state}
                pendingOperation={pendingOperation}
                connected={connected}
                error={actionError}
                onAction={(action) => void runAction(action)}
                onDismissError={clearActionError}
              />

              <ConfigurationForm
                acceptedConfig={snapshot.config}
                locked={configurationLocked}
                connected={connected}
                busy={pendingOperation !== null || isRefreshing}
                feedback={configurationError}
                onSubmit={updateConfiguration}
                onClearFeedback={clearConfigurationError}
              />
            </div>
          )}

          <footer className="mt-14 flex flex-col gap-2 border-t border-white/10 pt-5 text-xs text-slate-500 sm:flex-row sm:items-center sm:justify-between">
            <p>Simulation rules and state remain owned by the C++ backend.</p>
            <p className="font-mono">REST base path: /api</p>
          </footer>
        </main>
      </div>
    </div>
  );
}
