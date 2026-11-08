# Simulation Control API Contract — Issue #28

**Status:** Agreed endpoint contract for Issue #28, as of 2026-11-08  
**Scope:** Control and configuration endpoint handlers. The HTTP host, server lifecycle, and route registration are owned by the Issue #9 integration work.  
**Service boundary:** Handlers use only the public `SimulationService` interface. They must not access `SimModel` directly or create/manage a scheduler.

## 1. Common conventions

- Base path: `/api/simulation` (no `/api/v1` prefix).
- JSON content type: `application/json` for requests with bodies and JSON responses.
- State values are lowercase strings: `stopped`, `running`, `paused`.
- State values map to the corresponding `SimModel::State` values. The HTTP layer serializes the state; it does not manipulate the model.
- All successful operations use HTTP `200` and the success envelope below.
- All errors use the error envelope below.

### Success envelope

```json
{
  "success": true,
  "data": {},
  "error": null
}
```

### Error envelope

```json
{
  "success": false,
  "data": null,
  "error": {
    "code": "INVALID_STATE",
    "message": "The requested operation is not valid in the current simulation state."
  }
}
```

For configuration validation failures, `error.details` may contain field-specific validation issues. Each issue should preserve the `field`, `code`, and `message` reported by `ValidationIssue` in `include/input_validator.h`:

```json
{
  "success": false,
  "data": null,
  "error": {
    "code": "VALIDATION_FAILED",
    "message": "One or more configuration fields are invalid.",
    "details": [
      {
        "field": "tickMs",
        "code": "OUT_OF_RANGE",
        "message": "Field-specific message from InputValidator."
      }
    ]
  }
}
```

The example issue code/message above is illustrative; handlers must forward the actual validator issue values rather than inventing or duplicating validation rules.

## 2. Endpoints

### 2.1 Start simulation

`POST /api/simulation/start`

- Request body: none.
- Allowed transition: `stopped` → `running` only.
- Success (`200 OK`):

```json
{
  "success": true,
  "data": { "state": "running" },
  "error": null
}
```

- If the service rejects the transition because the simulation is not stopped, return `409 Conflict` with error code `INVALID_STATE`.

### 2.2 Pause simulation

`POST /api/simulation/pause`

- Request body: none.
- Allowed transition: `running` → `paused` only.
- Success (`200 OK`):

```json
{
  "success": true,
  "data": { "state": "paused" },
  "error": null
}
```

- The successful response acknowledges the service's synchronized pause operation: any tick already executing has completed, and no subsequent scheduler tick occurs until resume.
- If the service rejects the transition because the simulation is not running, return `409 Conflict` with error code `INVALID_STATE`.

### 2.3 Resume simulation

`POST /api/simulation/resume`

- Request body: none.
- Allowed transition: `paused` → `running` only.
- Success (`200 OK`):

```json
{
  "success": true,
  "data": { "state": "running" },
  "error": null
}
```

- If the service rejects the transition because the simulation is not paused, return `409 Conflict` with error code `INVALID_STATE`.

### 2.4 Reset simulation

`POST /api/simulation/reset`

- Request body: none.
- Allowed from any state.
- Success (`200 OK`):

```json
{
  "success": true,
  "data": {
    "state": "stopped",
    "config": {
      "tickMs": 1000,
      "greenTicks": 10,
      "yellowTicks": 2,
      "arrivalInterval": 3,
      "maxQueue": 12
    }
  },
  "error": null
}
```

- Reset is idempotent: repeating reset while already stopped still returns success.
- Reset restores the model's reset state and retains the last successfully accepted configuration. The example configuration above is illustrative; return the actual retained configuration.

### 2.5 Update simulation configuration

`PUT /api/simulation/config`

- Request body: all five fields are required and must be JSON integers (not strings, booleans, or floating-point values).
- No partial configuration updates are supported.

```json
{
  "tickMs": 1000,
  "greenTicks": 10,
  "yellowTicks": 2,
  "arrivalInterval": 3,
  "maxQueue": 12
}
```

| Field | JSON type | Inclusive accepted range | Meaning |
|---|---|---:|---|
| `tickMs` | integer | 100–5000 | Scheduler interval in milliseconds |
| `greenTicks` | integer | 1–60 | Green duration in logical ticks |
| `yellowTicks` | integer | 1–10 | Yellow duration in logical ticks |
| `arrivalInterval` | integer | 1–20 | Vehicle arrival interval in logical ticks |
| `maxQueue` | integer | 1–50 | Maximum queued vehicles per road |

Success (`200 OK`):

```json
{
  "success": true,
  "data": {
    "config": {
      "tickMs": 1000,
      "greenTicks": 10,
      "yellowTicks": 2,
      "arrivalInterval": 3,
      "maxQueue": 12
    }
  },
  "error": null
}
```

- The handler converts the integer JSON fields to the existing `RawConfig` representation as required by `SimulationService::setConfig()` and delegates validation/application to the service's existing `InputValidator`/`ConfigController` path.
- Do not duplicate parsing bounds or validation rules in route handlers. The HTTP boundary must still reject malformed JSON, missing fields, wrong JSON types, and values that cannot be safely represented by the existing configuration input type.
- If validation/application fails, return `400 Bad Request`, include the validator's field-specific issues in `error.details` when available, and leave the previously accepted configuration unchanged.
- On success, return the actual accepted configuration. The service is responsible for applying it consistently, including the scheduler interval.

## 3. HTTP status-code mapping

| HTTP status | Use |
|---|---|
| `200 OK` | Operation succeeded |
| `400 Bad Request` | Malformed JSON, missing required field, wrong field type, or configuration validation failure |
| `409 Conflict` | Start/pause/resume transition is invalid for the current simulation state |
| `500 Internal Server Error` | Unexpected server-side failure; do not expose internal exception details |

Unexpected errors should use the common error envelope with a stable public code such as `INTERNAL_ERROR` and a generic message. Do not return exception text, stack traces, filesystem paths, or other internal diagnostics.

## 4. Behaviour and implementation requirements

1. Start succeeds only from `stopped`.
2. Pause succeeds only from `running` and acknowledges the synchronized pause boundary; no later scheduler tick may occur before resume.
3. Resume succeeds only from `paused`.
4. Reset succeeds from every state, is idempotent, and retains the last accepted configuration.
5. Configuration updates are complete replacements: all five fields are required.
6. A rejected configuration must not partially modify or replace the accepted configuration.
7. The scheduler interval and model configuration change only after the service successfully applies the candidate configuration.
8. The service-owned scheduler remains the only caller of `model.tick()`.
9. Handlers use the public `SimulationService` API (`start`, `pause`, `resume`, `reset`, `setConfig`, and snapshot/state accessors as needed). No direct `SimModel` access and no second scheduler are permitted.
10. State-changing handlers, snapshots, and scheduler ticks must continue to share the service's synchronization boundary.

## 5. Integration boundary and open coordination point

Issue #9 owns the Crow HTTP host, server startup/shutdown, and route registration. Issue #28 owns the five control/configuration handler behaviours and this contract. The handler module should be independently usable by the host integration without taking ownership of server lifecycle.

The agreed decisions above do not specify whether configuration changes are allowed in every simulation state. The handler must follow the finalized `SimulationService` behaviour and obtain explicit team agreement before imposing an additional state restriction at the HTTP layer.

## 6. Source-of-truth interfaces

- `include/simulation_service.h` — public service operations and synchronized snapshot access.
- `include/input_validator.h` — `RawConfig`, `ValidationIssue`, and `InputValidator` diagnostics.
- `include/config_controller.h` — accepted/rejected configuration application path.
- `include/sim_config.h` — configuration fields and canonical inclusive bounds.
- `include/simulation_dto.h` — snapshot data structures.

## 7. Issue #28 handler module interface

The framework-neutral handler implementation is provided by:

- `include/simulation_control_api.h`
- `src/simulation_control_api.cpp`

`SimulationControlApi::handle(const ApiRequest&)` returns an `ApiResponse` containing the HTTP status code, `application/json` content type, and serialized response envelope. The handler module is compiled into `tms_core` and has no Crow or Qt dependency.

Issue #9's HTTP adapter is responsible for:

1. Registering the five method/path pairs above.
2. Parsing the incoming JSON body and mapping each JSON field to `ApiJsonField` while preserving its JSON type (`Integer`, `Number`, `String`, `Boolean`, etc.). Do not coerce floats, strings, or booleans to integers.
3. Setting `ApiRequest::jsonValid` to `false` when parsing fails.
4. Passing method, path, parse status, and object fields to `SimulationControlApi::handle`.
5. Returning `ApiResponse::statusCode`, `contentType`, and `body` unchanged to the HTTP client.

The handler returns `400 MALFORMED_JSON` when `jsonValid` is false. The API module deliberately does not parse raw JSON itself; parsing belongs to the HTTP integration boundary and remains replaceable with the framework selected by Issue #9.
