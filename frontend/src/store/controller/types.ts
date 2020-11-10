import {
  Alarms,
  SensorMeasurements,
  CycleMeasurements,
  Parameters,
  ParametersRequest,
  AlarmLimits,
  AlarmLimitsRequest,
  ActiveLogEvents,
  ExpectedLogEvent,
  LogEvent,
  NextLogEvents,
  BatteryPower,
  ScreenStatus,
} from './proto/mcu_pb';
import { SystemSettingRequest, FrontendDisplaySetting, RotaryEncoder } from './proto/frontend_pb';

// Action Types

export const STATE_UPDATED = '@controller/STATE_UPDATED';
export const PARAMETER_COMMITTED = '@controller/PARAMETER_COMMITTED';
export const ALARM_LIMITS = 'ALARM_LIMITS';
export const EXPECTED_LOG_EVENT_ID = 'EXPECTED_LOG_EVENT_ID';
export const ALARM_LIMITS_STANDBY = 'ALARM_LIMITS_STANDBY';
export const PARAMETER_STANDBY = 'PARAMETERS_STANDBY';
export const FRONTEND_DISPLAY_SETTINGS = 'FRONTEND_DISPLAY_SETTINGS';
export const SYSTEM_SETTINGS = 'SYSTEM_SETTINGS';

// Rotary encoder overriden params

export interface RotaryEncoderParameter {
  step: number;
  lastStepChange: number;
  buttonPressed: boolean;
  lastButtonDown: number;
  lastButtonUp: number;
  stepDiff: number;
}

// Protocol Buffers

export type PBMessage =
  // mcu_pb
  | AlarmLimits
  | AlarmLimitsRequest
  | Alarms
  | SensorMeasurements
  | CycleMeasurements
  | Parameters
  | ParametersRequest
  | LogEvent
  | ExpectedLogEvent
  | NextLogEvents
  | ActiveLogEvents
  | BatteryPower
  | ScreenStatus
  // frontend_pb
  | SystemSettingRequest
  | FrontendDisplaySetting
  | RotaryEncoderParameter
  | RotaryEncoder;

export type PBMessageType =
  // mcu_pb
  | typeof AlarmLimits
  | typeof AlarmLimitsRequest
  | typeof Alarms
  | typeof SensorMeasurements
  | typeof CycleMeasurements
  | typeof Parameters
  | typeof ParametersRequest
  | typeof NextLogEvents
  | typeof ActiveLogEvents
  | typeof BatteryPower
  | typeof ScreenStatus
  // frontend_pb
  | typeof SystemSettingRequest
  | typeof FrontendDisplaySetting
  | typeof RotaryEncoder;

export enum MessageType {
  Alarms = 1,
  SensorMeasurements = 2,
  CycleMeasurements = 3,
  Parameters = 4,
  ParametersRequest = 5,
  AlarmLimits = 6,
  AlarmLimitsRequest = 7,
  ExpectedLogEvent = 8,
  NextLogEvents = 9,
  ActiveLogEvents = 10,
  BatteryPower = 64,
  ScreenStatus = 65,
  RotaryEncoder = 128,
  SystemSettingRequest = 129,
  FrontendDisplaySetting = 130,
}

// States

export interface WaveformPoint {
  date: Date;
  value: number;
}

export interface WaveformHistory {
  waveformOld: {
    full: WaveformPoint[];
  };
  waveformNew: {
    full: WaveformPoint[];
    buffer: WaveformPoint[];
    segmented: WaveformPoint[][];
  };
  waveformNewStart: number;
}

export interface PVPoint {
  pressure: number;
  volume: number;
}

export interface PVHistory {
  loop: PVPoint[];
  loopOrigin: PVPoint;
  cycle: number;
}

export interface ControllerStates {
  // Message states from mcu_pb
  alarms: Alarms;
  alarmLimitsRequest: AlarmLimitsRequest;
  alarmLimitsRequestStandby: { alarmLimits: AlarmLimitsRequest };
  parametersRequestStandby: { parameters: ParametersRequest };
  systemSettingRequest: SystemSettingRequest;
  frontendDisplaySetting: FrontendDisplaySetting;
  sensorMeasurements: SensorMeasurements;
  cycleMeasurements: CycleMeasurements;
  parameters: Parameters;
  parametersRequest: ParametersRequest;
  logEvent: LogEvent;
  expectedLogEvent: ExpectedLogEvent;
  nextLogEvents: NextLogEvents;
  activeLogEvents: ActiveLogEvents;
  batteryPower: BatteryPower;
  screenStatus: ScreenStatus;

  // Message states from frontend_pb
  rotaryEncoder: RotaryEncoderParameter;

  // Derived states
  waveformHistoryPaw: WaveformHistory;
  waveformHistoryFlow: WaveformHistory;
  waveformHistoryVolume: WaveformHistory;
  pvHistory: PVHistory;
}

export const MessageClass = new Map<MessageType, PBMessageType>([
  [MessageType.Alarms, Alarms],
  [MessageType.AlarmLimitsRequest, AlarmLimitsRequest],
  [MessageType.SystemSettingRequest, SystemSettingRequest],
  [MessageType.FrontendDisplaySetting, FrontendDisplaySetting],
  [MessageType.SensorMeasurements, SensorMeasurements],
  [MessageType.CycleMeasurements, CycleMeasurements],
  [MessageType.Parameters, Parameters],
  [MessageType.ParametersRequest, ParametersRequest],
  [MessageType.NextLogEvents, NextLogEvents],
  [MessageType.ActiveLogEvents, ActiveLogEvents],
  [MessageType.RotaryEncoder, RotaryEncoder],
]);

export const MessageTypes = new Map<PBMessageType, MessageType>([
  [Alarms, MessageType.Alarms],
  [AlarmLimitsRequest, MessageType.AlarmLimitsRequest],
  [SystemSettingRequest, MessageType.SystemSettingRequest],
  [FrontendDisplaySetting, MessageType.FrontendDisplaySetting],
  [SensorMeasurements, MessageType.SensorMeasurements],
  [CycleMeasurements, MessageType.CycleMeasurements],
  [Parameters, MessageType.Parameters],
  [ParametersRequest, MessageType.ParametersRequest],
  [NextLogEvents, MessageType.NextLogEvents],
  [ActiveLogEvents, MessageType.ActiveLogEvents],
  [RotaryEncoder, MessageType.RotaryEncoder],
]);

// State Update Actions

interface StateUpdatedAction {
  type: typeof STATE_UPDATED;
  messageType: MessageType;
  state: PBMessage;
}

export type StateUpdateAction = StateUpdatedAction;

// State Update Actions

export interface commitAction {
  type: string;
  update: Record<string, unknown>;
}

// Parameter Commit Actions

interface ParameterCommittedAction {
  type: typeof PARAMETER_COMMITTED;
  update: Record<string, unknown>;
}

export type ParameterCommitAction = ParameterCommittedAction;
