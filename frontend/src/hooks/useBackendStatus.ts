import { useCallback, useEffect, useState } from "react";
import { apiClient } from "../api/client";
import { getApiErrorMessage } from "../api/errors";
import type { SimulationSnapshot } from "../api/types";

export type BackendStatus =
  | { kind: "loading" }
  | { kind: "connected"; snapshot: SimulationSnapshot }
  | { kind: "error"; message: string };

export function useBackendStatus() {
  const [status, setStatus] = useState<BackendStatus>({ kind: "loading" });

  const refresh = useCallback(async () => {
    setStatus({ kind: "loading" });
    try {
      const snapshot = await apiClient.getState();
      setStatus({ kind: "connected", snapshot });
    } catch (error) {
      setStatus({ kind: "error", message: getApiErrorMessage(error) });
    }
  }, []);

  useEffect(() => {
    void refresh();
  }, [refresh]);

  return { status, refresh };
}