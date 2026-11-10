export const APPROACH_IDS = ["north", "east", "south", "west"] as const;

export type ApproachId = (typeof APPROACH_IDS)[number];
export type SerializedApproachId = ApproachId | "unknown";
export type SimulationState = "stopped" | "running" | "paused";
export type SignalState = "red" | "yellow" | "green" | "unknown";
export type VehicleType =
  | "Car"
  | "Motorcycle"
  | "Bus"
  | "Truck"
  | "Emergency"
  | "Unknown";

export interface SimulationConfig {
  tickMs: number;
  greenTicks: number;
  yellowTicks: number;
  arrivalInterval: number;
  maxQueue: number;
}

export interface VehicleSnapshot {
  id: number;
  type: VehicleType;
  lane: string;
}

export interface ApproachSnapshot {
  id: ApproachId;
  name: "North" | "East" | "South" | "West";
  signal: SignalState;
  queueLength: number;
  vehicles: VehicleSnapshot[];
}

export interface ApproachStatistics {
  id: ApproachId;
  vehiclesGenerated: number;
  vehiclesProcessed: number;
  vehiclesWaiting: number;
}

export interface SimulationStatistics {
  tickCount: number;
  vehiclesGenerated: number;
  vehiclesProcessed: number;
  vehiclesWaiting: number;
  totalWaitingTicks: number;
  averageWaitingTicks: number;
  processedPerLogicalTick: number;
  approaches: ApproachStatistics[];
}

export interface SimulationSnapshot {
  state: SimulationState | "unknown";
  tickCount: number;
  activeApproach: SerializedApproachId;
  activeSignal: SignalState;
  approaches: ApproachSnapshot[];
  config: SimulationConfig;
  statistics: SimulationStatistics;
}

export interface ValidationIssue {
  field: string;
  code: string;
  message: string;
}

export interface ApiErrorPayload {
  code: string;
  message: string;
  details?: unknown;
}

export type ApiEnvelope<TData> =
  | { success: true; data: TData; error: null }
  | { success: false; data: null; error: ApiErrorPayload };

export interface StateResult {
  state: SimulationState;
}

export interface ResetResult extends StateResult {
  state: "stopped";
  config: SimulationConfig;
}

export interface ConfigurationResult {
  config: SimulationConfig;
}