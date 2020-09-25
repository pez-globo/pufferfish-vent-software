import {
  Alarms,
  SensorMeasurements,
  CycleMeasurements,
  Parameters,
  ParametersRequest,
  Ping,
  Announcement,
  AlarmLimitsRequest,
} from './proto/mcu_pb';
import { RotaryEncoder, SystemSettingRequest, FrontendDisplaySetting } from './proto/frontend_pb';

// Action Types

export const STATE_UPDATED = '@controller/STATE_UPDATED';
export const PARAMETER_COMMITTED = '@controller/PARAMETER_COMMITTED';
export const ALARM_LIMITS = 'ALARM_LIMITS';
export const FRONTEND_DISPLAY_SETTINGS = 'FRONTEND_DISPLAY_SETTINGS';
export const SYSTEM_SETTINGS = 'SYSTEM_SETTINGS';

// Protocol Buffers

export type PBMessage =
  | // mcu_pb
  AlarmLimitsRequest
  | SystemSettingRequest
  | FrontendDisplaySetting
  | Alarms
  | SensorMeasurements
  | CycleMeasurements
  | Parameters
  | ParametersRequest
  | Ping
  | Announcement
  // frontend_pb
  | RotaryEncoder;

export type PBMessageType =
  | // mcu_pb
  typeof AlarmLimitsRequest
  | typeof SystemSettingRequest
  | typeof FrontendDisplaySetting
  | typeof Alarms
  | typeof SensorMeasurements
  | typeof CycleMeasurements
  | typeof Parameters
  | typeof ParametersRequest
  | typeof Ping
  | typeof Announcement
  // frontend_pb
  | typeof RotaryEncoder;

export enum MessageType {
  Alarms = 1,
  SensorMeasurements = 2,
  CycleMeasurements = 3,
  Parameters = 4,
  ParametersRequest = 5,
  Ping = 6,
  Announcement = 7,
  AlarmLimitsRequest = 8,
  SystemSettingRequest = 9,
  FrontendDisplaySetting = 10,
  RotaryEncoder = 128,
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
  systemSettingRequest: SystemSettingRequest;
  frontendDisplaySetting: FrontendDisplaySetting;
  sensorMeasurements: SensorMeasurements;
  cycleMeasurements: CycleMeasurements;
  parameters: Parameters;
  parametersRequest: ParametersRequest;
  ping: Ping;
  announcement: Announcement;

  // Message states from frontend_pb
  rotaryEncoder: RotaryEncoder;

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
  [MessageType.Ping, Ping],
  [MessageType.Announcement, Announcement],
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
  [Ping, MessageType.Ping],
  [Announcement, MessageType.Announcement],
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
