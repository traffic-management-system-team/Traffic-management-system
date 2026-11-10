import { useCallback, useEffect, useRef, useState } from "react";
import { apiClient } from "../api/client";
import {
  getApiErrorMessage,
  getConfigurationFieldErrors,
  isApiClientError,
  type ConfigurationField,
} from "../api/errors";
import type { SimulationConfig, SimulationSnapshot } from "../api/types";

export type BackendStatus =
  | { kind: "loading" }
  | { kind: "connected"; snapshot: SimulationSnapshot }
  | { kind: "error"; message: string; snapshot?: SimulationSnapshot };

export type SimulationOperation =
  | "start"
  | "pause"
  | "resume"
  | "reset"
  | "configuration";

export interface ConfigurationFeedback {
  message: string;
  fieldErrors: Partial<Record<ConfigurationField, string>>;
}

const lifecycleActions = {
  start: () => apiClient.start(),
  pause: () => apiClient.pause(),
  resume: () => apiClient.resume(),
  reset: () => apiClient.reset(),
} as const;

export function useBackendStatus() {
  const [status, setStatus] = useState<BackendStatus>({ kind: "loading" });
  const [pendingOperation, setPendingOperation] =
    useState<SimulationOperation | null>(null);
  const [isRefreshing, setIsRefreshing] = useState(false);
  const [actionError, setActionError] = useState<string | null>(null);
  const [configurationError, setConfigurationError] =
    useState<ConfigurationFeedback | null>(null);
  const snapshotRef = useRef<SimulationSnapshot | null>(null);
  const pendingRef = useRef(false);

  const refresh = useCallback(async () => {
    if (!snapshotRef.current) {
      setStatus({ kind: "loading" });
    }
    setIsRefreshing(true);
    try {
      const snapshot = await apiClient.getState();
      snapshotRef.current = snapshot;
      setStatus({ kind: "connected", snapshot });
      return snapshot;
    } catch (error) {
      setStatus({
        kind: "error",
        message: getApiErrorMessage(error),
        snapshot: snapshotRef.current ?? undefined,
      });
      return null;
    } finally {
      setIsRefreshing(false);
    }
  }, []);

  const runAction = useCallback(
    async (action: keyof typeof lifecycleActions) => {
      if (pendingRef.current) return;
      pendingRef.current = true;
      setPendingOperation(action);
      setActionError(null);
      setConfigurationError(null);

      try {
        await lifecycleActions[action]();
        const refreshed = await refresh();
        if (!refreshed) {
          setActionError(
            "The request succeeded, but the latest simulation state could not be loaded. Retry the state request before continuing.",
          );
        }
      } catch (error) {
        setActionError(getApiErrorMessage(error));
        if (isApiClientError(error) && error.status === 409) {
          await refresh();
        }
      } finally {
        pendingRef.current = false;
        setPendingOperation(null);
      }
    },
    [refresh],
  );

  const updateConfiguration = useCallback(
    async (config: SimulationConfig): Promise<SimulationConfig | null> => {
      if (pendingRef.current) return null;
      pendingRef.current = true;
      setPendingOperation("configuration");
      setConfigurationError(null);
      setActionError(null);

      try {
        const result = await apiClient.updateConfig(config);
        const acceptedSnapshot = snapshotRef.current
          ? { ...snapshotRef.current, config: result.config }
          : null;
        if (acceptedSnapshot) {
          snapshotRef.current = acceptedSnapshot;
          setStatus({ kind: "connected", snapshot: acceptedSnapshot });
        }

        const refreshed = await refresh();
        if (!refreshed) {
          setConfigurationError({
            message:
              "The backend accepted these settings, but the latest simulation state could not be loaded. The accepted settings are retained below.",
            fieldErrors: {},
          });
        }
        return result.config;
      } catch (error) {
        setConfigurationError({
          message: getApiErrorMessage(error),
          fieldErrors: getConfigurationFieldErrors(error),
        });
        if (
          isApiClientError(error) &&
          (error.status === 409 || error.code === "NETWORK_ERROR")
        ) {
          await refresh();
        }
        return null;
      } finally {
        pendingRef.current = false;
        setPendingOperation(null);
      }
    },
    [refresh],
  );

  const clearActionError = useCallback(() => setActionError(null), []);
  const clearConfigurationError = useCallback(
    () => setConfigurationError(null),
    [],
  );

  useEffect(() => {
    void refresh();
  }, [refresh]);

  return {
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
  };
}
