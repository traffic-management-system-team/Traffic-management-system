import type { ApiErrorPayload } from "./types";

export type ApiClientErrorCode =
  | ApiErrorPayload["code"]
  | "NETWORK_ERROR"
  | "INVALID_RESPONSE"
  | "HTTP_ERROR";

export class ApiClientError extends Error {
  readonly code: ApiClientErrorCode;
  readonly status?: number;
  readonly details?: unknown;

  constructor(
    code: ApiClientErrorCode,
    message: string,
    status?: number,
    details?: unknown,
  ) {
    super(message);
    this.name = "ApiClientError";
    this.code = code;
    this.status = status;
    this.details = details;
  }
}

export function isApiClientError(error: unknown): error is ApiClientError {
  return error instanceof ApiClientError;
}

export function getApiErrorMessage(error: unknown): string {
  if (isApiClientError(error)) {
    return error.message;
  }

  return "The backend could not be reached. Check the connection and try again.";
}