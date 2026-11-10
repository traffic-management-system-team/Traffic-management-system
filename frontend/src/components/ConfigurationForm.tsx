import { useState } from "react";
import type { SimulationConfig } from "../api/types";
import type { ConfigurationFeedback } from "../hooks/useBackendStatus";

interface ConfigurationFormProps {
  acceptedConfig: SimulationConfig;
  locked: boolean;
  connected: boolean;
  busy: boolean;
  feedback: ConfigurationFeedback | null;
  onSubmit: (config: SimulationConfig) => Promise<SimulationConfig | null>;
  onClearFeedback: () => void;
}

type DraftConfig = Record<keyof SimulationConfig, string>;

const configFields = [
  {
    key: "tickMs",
    label: "Tick interval",
    unit: "milliseconds",
    range: "100–5000 ms",
  },
  {
    key: "greenTicks",
    label: "GREEN duration",
    unit: "ticks",
    range: "1–60 ticks",
  },
  {
    key: "yellowTicks",
    label: "YELLOW duration",
    unit: "ticks",
    range: "1–10 ticks",
  },
  {
    key: "arrivalInterval",
    label: "Vehicle arrival interval",
    unit: "ticks",
    range: "1–20 ticks",
  },
  {
    key: "maxQueue",
    label: "Maximum queue capacity",
    unit: "vehicles per approach",
    range: "1–50 vehicles per approach",
  },
] as const;

function toDraft(config: SimulationConfig): DraftConfig {
  return {
    tickMs: String(config.tickMs),
    greenTicks: String(config.greenTicks),
    yellowTicks: String(config.yellowTicks),
    arrivalInterval: String(config.arrivalInterval),
    maxQueue: String(config.maxQueue),
  };
}

export function ConfigurationForm({
  acceptedConfig,
  locked,
  connected,
  busy,
  feedback,
  onSubmit,
  onClearFeedback,
}: ConfigurationFormProps) {
  const [draft, setDraft] = useState<DraftConfig>(() => toDraft(acceptedConfig));
  const [inputErrors, setInputErrors] = useState<Partial<DraftConfig>>({});
  const disabled = !connected || locked || busy;

  async function handleSubmit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const config = {} as SimulationConfig;
    const errors: Partial<DraftConfig> = {};

    for (const { key, label } of configFields) {
      const text = draft[key].trim();
      const value = Number(text);
      if (!text || !Number.isFinite(value)) {
        errors[key] = `${label} must be a number.`;
      } else {
        config[key] = value;
      }
    }

    setInputErrors(errors);
    if (Object.keys(errors).length > 0) return;

    onClearFeedback();
    const accepted = await onSubmit(config);
    if (accepted) {
      setDraft(toDraft(accepted));
      setInputErrors({});
    }
  }

  function updateDraft(field: keyof SimulationConfig, value: string) {
    setDraft((current) => ({ ...current, [field]: value }));
    setInputErrors((current) => ({ ...current, [field]: undefined }));
    onClearFeedback();
  }

  return (
    <section
      aria-labelledby="configuration-title"
      aria-busy={busy}
      className="surface-card p-5 sm:p-7"
    >
      <div>
        <p className="eyebrow">Settings</p>
        <h2
          id="configuration-title"
          className="mt-2 text-lg font-semibold tracking-tight text-white"
        >
          Simulation configuration
        </h2>
        <p className="mt-2 text-sm leading-6 text-slate-400">
          Edit a draft, then submit all five settings together. The accepted
          values below change only after the backend confirms them.
        </p>
      </div>

      <div className="mt-5 rounded-xl border border-white/10 bg-slate-950/40 p-4">
        <h3 className="text-xs font-semibold uppercase tracking-[0.14em] text-slate-400">
          Last accepted by server
        </h3>
        <dl className="mt-3 grid grid-cols-2 gap-x-4 gap-y-2 text-sm sm:grid-cols-3">
          {configFields.map(({ key, label }) => (
            <div key={key}>
              <dt className="text-xs text-slate-500">{label}</dt>
              <dd data-testid={`accepted-${key}`} className="mt-0.5 font-mono text-slate-100">
                {acceptedConfig[key]}
              </dd>
            </div>
          ))}
        </dl>
      </div>

      <form className="mt-5" onSubmit={(event) => void handleSubmit(event)}>
        <fieldset disabled={disabled} className="grid gap-4 sm:grid-cols-2">
          <legend className="sr-only">Draft simulation settings</legend>
          {configFields.map(({ key, label, unit, range }) => {
            const error = inputErrors[key] ?? feedback?.fieldErrors[key];
            const helpId = `${key}-help`;
            const errorId = `${key}-error`;
            return (
              <div key={key} className="min-w-0">
                <label
                  htmlFor={key}
                  className="block text-sm font-medium text-slate-200"
                >
                  {label} <span className="text-xs font-normal text-slate-500">({unit})</span>
                </label>
                <input
                  id={key}
                  name={key}
                  type="number"
                  step="any"
                  inputMode="decimal"
                  required
                  value={draft[key]}
                  aria-invalid={error ? "true" : "false"}
                  aria-describedby={error ? `${helpId} ${errorId}` : helpId}
                  className="mt-2 w-full rounded-xl border border-white/15 bg-slate-950/55 px-3 py-2.5 text-sm text-white outline-none transition placeholder:text-slate-600 focus-visible:border-cyan-300/70 focus-visible:ring-2 focus-visible:ring-cyan-300/25 disabled:cursor-not-allowed disabled:opacity-50"
                  onChange={(event) => updateDraft(key, event.currentTarget.value)}
                />
                <p id={helpId} className="mt-1.5 text-xs text-slate-500">
                  Backend accepted range: {range}.
                </p>
                {error && (
                  <p id={errorId} className="mt-1.5 text-xs text-rose-200" aria-live="polite">
                    {error}
                  </p>
                )}
              </div>
            );
          })}
        </fieldset>

        {feedback && (
          <p
            className="mt-4 rounded-xl border border-rose-300/20 bg-rose-950/30 p-3 text-sm text-rose-100"
            role="alert"
            aria-live="assertive"
          >
            {feedback.message}
          </p>
        )}

        {locked && (
          <p className="mt-4 rounded-xl border border-amber-200/15 bg-amber-950/20 p-3 text-sm leading-6 text-amber-100" role="status">
            Configuration is locked unless the backend reports a stopped
            simulation with no ticks or generated vehicles. Reset the simulation
            before applying new settings after activity.
          </p>
        )}
        {!connected && !locked && (
          <p className="mt-4 text-sm text-amber-100" role="status">
            Configuration is disabled until an authoritative backend snapshot is available.
          </p>
        )}
        {busy && (
          <p className="mt-4 text-sm text-cyan-100" role="status" aria-live="polite">
            Saving configuration…
          </p>
        )}

        <div className="mt-5 flex flex-wrap items-center gap-3">
          <button
            type="submit"
            disabled={disabled}
            className="button-primary px-4 py-2.5 text-sm disabled:cursor-not-allowed disabled:opacity-45"
          >
            {busy ? "Please wait…" : "Apply configuration"}
          </button>
          <p className="text-xs text-slate-500">
            Range guidance is shown here; the backend validates every submitted value.
          </p>
        </div>
      </form>
    </section>
  );
}
