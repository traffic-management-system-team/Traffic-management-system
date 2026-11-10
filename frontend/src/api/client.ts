import { API_ENDPOINTS } from "./endpoints";
import { ApiClientError } from "./errors";
import type {
  ApiEnvelope,
  ApiErrorPayload,
  ConfigurationResult,
  ResetResult,
  SimulationConfig,
  SimulationSnapshot,
  StateResult,
} from "./types";

type FetchImplementation = typeof fetch;

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === "object" && value !== null;
}

function isApiErrorPayload(value: unknown): value is ApiErrorPayload {
  return (
    isRecord(value) &&
    typeof value.code === "string" &&
    typeof value.message === "string"
  );
}

function asApiError(
  payload: unknown,
  status: number,
): ApiClientError | undefined {
  if (
    !isRecord(payload) ||
    payload.success !== false ||
    !isApiErrorPayload(payload.error)
  ) {
    return undefined;
  }

  return new ApiClientError(
    payload.error.code,
    payload.error.message,
    status,
    payload.error.details,
  );
}

async function requestJson<TData>(
  path: string,
  init: RequestInit,
  fetchImpl: FetchImplementation,
): Promise<TData> {
  const headers = new Headers(init.headers);
  headers.set("Accept", "application/json");

  if (init.body !== undefined) {
    headers.set("Content-Type", "application/json");
  }

  let response: Response;
  try {
    response = await fetchImpl(path, { ...init, headers });
  } catch (cause) {
    const reason =
      cause instanceof Error && cause.message
        ? " " + cause.message
        : "";
    throw new ApiClientError(
      "NETWORK_ERROR",
      "Unable to connect to the backend." + reason,
    );
  }

  let payload: unknown;
  try {
    payload = await response.json();
  } catch {
    throw new ApiClientError(
      "INVALID_RESPONSE",
      "The backend returned a response that was not valid JSON.",
      response.status,
    );
  }

  const backendError = asApiError(payload, response.status);
  if (backendError) {
    throw backendError;
  }

  if (!response.ok) {
    throw new ApiClientError(
      "HTTP_ERROR",
      "The backend request failed with HTTP " + response.status + ".",
      response.status,
    );
  }

  if (
    !isRecord(payload) ||
    payload.success !== true ||
    payload.error !== null ||
    !("data" in payload)
  ) {
    throw new ApiClientError(
      "INVALID_RESPONSE",
      "The backend returned an unexpected response envelope.",
      response.status,
    );
  }

  const envelope = payload as ApiEnvelope<TData>;
  return envelope.data as TData;
}

export function createApiClient(
  fetchImpl: FetchImplementation = (...args) => globalThis.fetch(...args),
) {
  return {
    getState: () =>
      requestJson<SimulationSnapshot>(
        API_ENDPOINTS.state,
        { method: "GET" },
        fetchImpl,
      ),

    start: () =>
      requestJson<StateResult>(
        API_ENDPOINTS.start,
        { method: "POST" },
        fetchImpl,
      ),

    pause: () =>
      requestJson<StateResult>(
        API_ENDPOINTS.pause,
        { method: "POST" },
        fetchImpl,
      ),

    resume: () =>
      requestJson<StateResult>(
        API_ENDPOINTS.resume,
        { method: "POST" },
        fetchImpl,
      ),

    reset: () =>
      requestJson<ResetResult>(
        API_ENDPOINTS.reset,
        { method: "POST" },
        fetchImpl,
      ),

    updateConfig: (config: SimulationConfig) =>
      requestJson<ConfigurationResult>(
        API_ENDPOINTS.config,
        {
          method: "PUT",
          body: JSON.stringify(config),
        },
        fetchImpl,
      ),
  };
}

export const apiClient = createApiClient();
