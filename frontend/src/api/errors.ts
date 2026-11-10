import type {
  ApiErrorPayload,
  SimulationConfig,
  ValidationIssue,
} from "./types";

export type ConfigurationField = keyof SimulationConfig;

const configurationFields: readonly ConfigurationField[] = [
  "tickMs",
  "greenTicks",
  "yellowTicks",
  "arrivalInterval",
  "maxQueue",
];

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

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === "object" && value !== null;
}

function isValidationIssue(value: unknown): value is ValidationIssue {
  return (
    isRecord(value) &&
    typeof value.field === "string" &&
    configurationFields.includes(value.field as ConfigurationField) &&
    typeof value.code === "string" &&
    typeof value.message === "string"
  );
}

export function getConfigurationFieldErrors(
  error: unknown,
): Partial<Record<ConfigurationField, string>> {
  if (!isApiClientError(error) || !Array.isArray(error.details)) {
    return {};
  }

  return error.details.reduce<
    Partial<Record<ConfigurationField, string>>
  >((fieldErrors, issue) => {
    if (isValidationIssue(issue)) {
      fieldErrors[issue.field as ConfigurationField] = issue.message;
    }
    return fieldErrors;
  }, {});
}
