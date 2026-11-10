import { afterEach, describe, expect, it, vi } from "vitest";
import { createApiClient } from "./client";
import { ApiClientError } from "./errors";
import type { SimulationSnapshot } from "./types";

const snapshot: SimulationSnapshot = {
  state: "stopped",
  tickCount: 0,
  activeApproach: "north",
  activeSignal: "green",
  approaches: [
    { id: "north", name: "North", signal: "green", queueLength: 0, vehicles: [] },
    { id: "east", name: "East", signal: "red", queueLength: 0, vehicles: [] },
    { id: "south", name: "South", signal: "red", queueLength: 0, vehicles: [] },
    { id: "west", name: "West", signal: "red", queueLength: 0, vehicles: [] },
  ],
  config: {
    tickMs: 1000,
    greenTicks: 10,
    yellowTicks: 2,
    arrivalInterval: 3,
    maxQueue: 12,
  },
  statistics: {
    tickCount: 0,
    vehiclesGenerated: 0,
    vehiclesProcessed: 0,
    vehiclesWaiting: 0,
    totalWaitingTicks: 0,
    averageWaitingTicks: 0,
    processedPerLogicalTick: 0,
    approaches: [
      { id: "north", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
      { id: "south", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
      { id: "east", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
      { id: "west", vehiclesGenerated: 0, vehiclesProcessed: 0, vehiclesWaiting: 0 },
    ],
  },
};

function jsonResponse(body: unknown, status = 200): Response {
  return new Response(JSON.stringify(body), {
    status,
    headers: { "Content-Type": "application/json" },
  });
}

function mockFetch(
  implementation: (
    input: RequestInfo | URL,
    init?: RequestInit,
  ) => Promise<Response>,
) {
  return vi.fn(implementation);
}

afterEach(() => {
  vi.unstubAllGlobals();
});

describe("simulation API client", () => {
  it("parses a successful state envelope and uses the same-origin API path", async () => {
    const fetchMock = mockFetch(async () =>
      jsonResponse({ success: true, data: snapshot, error: null }),
    );
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.getState()).resolves.toEqual(snapshot);
    expect(fetchMock).toHaveBeenCalledTimes(1);
    expect(fetchMock.mock.calls[0]?.[0]).toBe("/api/simulation/state");

    const init = fetchMock.mock.calls[0]?.[1];
    const headers = new Headers(init?.headers);
    expect(init?.method).toBe("GET");
    expect(headers.get("Accept")).toBe("application/json");
    expect(headers.has("Content-Type")).toBe(false);
  });

  it("uses the documented lifecycle endpoints with POST and no request body", async () => {
    const fetchMock = mockFetch(async (input) => {
      const path = String(input);
      const data = path.endsWith("/reset")
        ? { state: "stopped", config: snapshot.config }
        : { state: path.endsWith("/pause") ? "paused" : "running" };
      return jsonResponse({ success: true, data, error: null });
    });
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await api.start();
    await api.pause();
    await api.resume();
    await api.reset();

    expect(fetchMock.mock.calls.map(([path]) => String(path))).toEqual([
      "/api/simulation/start",
      "/api/simulation/pause",
      "/api/simulation/resume",
      "/api/simulation/reset",
    ]);
    for (const [, init] of fetchMock.mock.calls) {
      expect(init?.method).toBe("POST");
      expect(init?.body).toBeUndefined();
    }
  });

  it("preserves backend validation messages and details", async () => {
    const fetchMock = mockFetch(async () =>
      jsonResponse(
        {
          success: false,
          data: null,
          error: {
            code: "VALIDATION_FAILED",
            message: "Field-specific validation failed.",
            details: [
              {
                field: "tickMs",
                code: "OUT_OF_RANGE",
                message: "tickMs must be between 100 and 5000.",
              },
            ],
          },
        },
        400,
      ),
    );
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.updateConfig(snapshot.config)).rejects.toMatchObject({
      name: "ApiClientError",
      code: "VALIDATION_FAILED",
      message: "Field-specific validation failed.",
      status: 400,
      details: [
        {
          field: "tickMs",
          code: "OUT_OF_RANGE",
          message: "tickMs must be between 100 and 5000.",
        },
      ],
    });
  });

  it("handles HTTP failures and preserves a backend error envelope", async () => {
    const fetchMock = mockFetch(async () =>
      jsonResponse(
        {
          success: false,
          data: null,
          error: {
            code: "INVALID_STATE",
            message: "Simulation can only pause when running.",
          },
        },
        409,
      ),
    );
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.pause()).rejects.toMatchObject({
      code: "INVALID_STATE",
      message: "Simulation can only pause when running.",
      status: 409,
    });
  });

  it("reports an HTTP failure without a backend error envelope", async () => {
    const fetchMock = mockFetch(async () =>
      jsonResponse({ message: "upstream unavailable" }, 502),
    );
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.getState()).rejects.toMatchObject({
      code: "HTTP_ERROR",
      message: "The backend request failed with HTTP 502.",
      status: 502,
    });
  });

  it("normalizes network failures", async () => {
    const fetchMock = mockFetch(async () => {
      throw new TypeError("socket unavailable");
    });
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.getState()).rejects.toMatchObject({
      code: "NETWORK_ERROR",
      message: "Unable to connect to the backend. socket unavailable",
    });
  });

  it("reports malformed JSON as an invalid backend response", async () => {
    const fetchMock = mockFetch(async () =>
      new Response("{not-json", {
        status: 200,
        headers: { "Content-Type": "application/json" },
      }),
    );
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.getState()).rejects.toBeInstanceOf(ApiClientError);
    await expect(api.getState()).rejects.toMatchObject({
      code: "INVALID_RESPONSE",
      status: 200,
      message: "The backend returned a response that was not valid JSON.",
    });
  });

  it("sends configuration using the exact backend field names and JSON header", async () => {
    const config = {
      tickMs: 250,
      greenTicks: 12,
      yellowTicks: 3,
      arrivalInterval: 5,
      maxQueue: 20,
    };
    const fetchMock = mockFetch(async () =>
      jsonResponse({ success: true, data: { config }, error: null }),
    );
    const api = createApiClient(fetchMock as unknown as typeof fetch);

    await expect(api.updateConfig(config)).resolves.toEqual({ config });

    const [path, init] = fetchMock.mock.calls[0] ?? [];
    const headers = new Headers(init?.headers);
    expect(path).toBe("/api/simulation/config");
    expect(init?.method).toBe("PUT");
    expect(headers.get("Accept")).toBe("application/json");
    expect(headers.get("Content-Type")).toBe("application/json");
    expect(JSON.parse(String(init?.body))).toEqual(config);
  });
});
