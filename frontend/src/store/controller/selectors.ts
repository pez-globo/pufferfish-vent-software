import { createSelector, OutputSelector } from 'reselect';
import { StoreState } from '../types';
import {
  Alarms,
  SensorMeasurements,
  CycleMeasurements,
  Parameters,
  ParametersRequest,
  AlarmLimitsRequest,
  VentilationMode,
  ExpectedLogEvent,
  NextLogEvents,
  LogEvent,
} from './proto/mcu_pb';
import { RotaryEncoder, FrontendDisplaySetting, SystemSettingRequest } from './proto/frontend_pb';
import {
  PBMessageType,
  ControllerStates,
  WaveformPoint,
  WaveformHistory,
  PVPoint,
  PVHistory,
  RotaryEncoderParameter,
} from './types';

export const getController = ({ controller }: StoreState): ControllerStates => controller;

// Alarms
export const getAlarms = createSelector(
  getController,
  (states: ControllerStates): Alarms => states.alarms,
);

// SensorMeasurements
export const getSensorMeasurements = createSelector(
  getController,
  (states: ControllerStates): SensorMeasurements => states.sensorMeasurements,
);
export const getSensorMeasurementsTime = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.time,
);
export const getSensorMeasurementsPaw = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.paw,
);
export const getSensorMeasurementsFlow = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.flow,
);
export const getSensorMeasurementsVolume = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.volume,
);
export const getSensorMeasurementsFiO2 = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.fio2,
);
export const getSensorMeasurementsSpO2 = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.spo2,
);

// CycleMeasurements
export const getCycleMeasurements = createSelector(
  getController,
  (states: ControllerStates): CycleMeasurements => states.cycleMeasurements,
);
export const getCycleMeasurementsPIP = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.pip,
);
export const getCycleMeasurementsPEEP = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.peep,
);
export const getCycleMeasurementsRR = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.rr,
);
export const getCycleMeasurementsVT = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.vt,
);

// ROX Index
export const getROXIndex = createSelector(
  getSensorMeasurements,
  getCycleMeasurements,
  (sensorMeasurements: SensorMeasurements, cycleMeasurements: CycleMeasurements): number => {
    if (sensorMeasurements.spo2 && sensorMeasurements.fio2 && cycleMeasurements.rr) {
      return sensorMeasurements.spo2 / sensorMeasurements.fio2 / cycleMeasurements.rr;
    }
    return 0;
  },
);

// Parameters
export const getParameters = createSelector(
  getController,
  (states: ControllerStates): Parameters => states.parameters,
);
export const getParametersPIP = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.pip,
);
export const getParametersPEEP = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.peep,
);
export const getParametersRR = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.rr,
);
export const getParametersFiO2 = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.fio2,
);
export const getParametersFlow = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.flow,
);

// ParametersRequest
export const getParametersRequest = createSelector(
  getController,
  (states: ControllerStates): ParametersRequest => states.parametersRequest,
);

export const getParametersRequestStandby = createSelector(
  getController,
  (states: ControllerStates): ParametersRequest =>
    states.parametersRequestStandby.parameters as ParametersRequest,
);

export const getParametersRequestMode = createSelector(
  getParametersRequest,
  (parametersRequest: ParametersRequest): VentilationMode => parametersRequest.mode,
);

// RotaryEncoder
export const getRotaryEncoder = createSelector(
  getController,
  (states: ControllerStates): RotaryEncoderParameter => states.rotaryEncoder,
);

// Waveforms
export const getWaveformHistoryPaw = createSelector(
  getController,
  (states: ControllerStates): WaveformHistory => states.waveformHistoryPaw,
);
export const getWaveformPawOld = createSelector(
  getWaveformHistoryPaw,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformOld.full,
);
// Paw Waveforms
export const getWaveformPawNewSegments = createSelector(
  getWaveformHistoryPaw,
  (waveformHistory: WaveformHistory): WaveformPoint[][] => waveformHistory.waveformNew.segmented,
);
export const getWaveformPawNewSegment = (
  segmentIndex: number,
): OutputSelector<StoreState, WaveformPoint[], unknown> =>
  createSelector(
    getWaveformPawNewSegments,
    (waveformSegments: WaveformPoint[][]): WaveformPoint[] => waveformSegments[segmentIndex],
  );
export const getWaveformHistoryFlow = createSelector(
  getController,
  (states: ControllerStates): WaveformHistory => states.waveformHistoryFlow,
);
export const getWaveformHistoryVolume = createSelector(
  getController,
  (states: ControllerStates): WaveformHistory => states.waveformHistoryVolume,
);
export const getWaveformFlowOld = createSelector(
  getWaveformHistoryFlow,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformOld.full,
);
export const getWaveformFlowNewSegments = createSelector(
  getWaveformHistoryFlow,
  (waveformHistory: WaveformHistory): WaveformPoint[][] => waveformHistory.waveformNew.segmented,
);
export const getWaveformFlowNewSegment = (
  segmentIndex: number,
): OutputSelector<StoreState, WaveformPoint[], unknown> =>
  createSelector(
    getWaveformFlowNewSegments,
    (waveformSegments: WaveformPoint[][]): WaveformPoint[] => waveformSegments[segmentIndex],
  );
// Volume Waveforms
export const getWaveformVolumeOld = createSelector(
  getWaveformHistoryVolume,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformOld.full,
);
export const getWaveformVolumeNewSegments = createSelector(
  getWaveformHistoryVolume,
  (waveformHistory: WaveformHistory): WaveformPoint[][] => waveformHistory.waveformNew.segmented,
);
export const getWaveformVolumeNewSegment = (
  segmentIndex: number,
): OutputSelector<StoreState, WaveformPoint[], unknown> =>
  createSelector(
    getWaveformVolumeNewSegments,
    (waveformSegments: WaveformPoint[][]): WaveformPoint[] => waveformSegments[segmentIndex],
  );

// P-V Loops
export const getPVHistory = createSelector(
  getController,
  (states: ControllerStates): PVHistory => states.pvHistory,
);
export const getPVLoop = createSelector(
  getPVHistory,
  (pvHistory: PVHistory): PVPoint[] => pvHistory.loop,
);

// Alarm Limits
export const getAlarmLimitsRequest = createSelector(
  getController,
  (states: ControllerStates): AlarmLimitsRequest | Record<string, number> =>
    states.alarmLimitsRequest,
);

export const getAlarmLimitsRequestStandby = createSelector(
  getController,
  (states: ControllerStates): AlarmLimitsRequest | Record<string, number> =>
    states.alarmLimitsRequestStandby.alarmLimits as AlarmLimitsRequest,
);

// Display Settings
export const getFrontendDisplaySetting = createSelector(
  getController,
  (states: ControllerStates): FrontendDisplaySetting => states.frontendDisplaySetting,
);

// Display Settings
export const getSystemSettingRequest = createSelector(
  getController,
  (states: ControllerStates): SystemSettingRequest => states.systemSettingRequest,
);

// New Log event
export const getLogEvent = createSelector(
  getController,
  (states: ControllerStates): LogEvent => states.logEvent,
);

// Next Log Events
export const getNextLogEvents = createSelector(
  getController,
  (states: ControllerStates): LogEvent[] => states.nextLogEvents.elements,
);

// Patient Alarm Event
export const getExpectedLogEvent = createSelector(
  getController,
  (states: ControllerStates): number => states.expectedLogEvent.id,
);

// Active log event Ids
export const getActiveLogEventIds = createSelector(
  getController,
  (states: ControllerStates): number[] => states.activeLogEvents.id,
);

// Active popup event log
export const getPopupEventLog = createSelector(getController, (states: ControllerStates):
  | LogEvent
  | undefined => {
  const maxId = Math.max(...states.activeLogEvents.id);
  return states.nextLogEvents.elements.find((el: LogEvent) => el.id === maxId);
});

// Battery power
export const getBatteryPower = createSelector(
  getController,
  (states: ControllerStates): number => states.batteryPower.powerLeft,
);

// Srceen Status
export const getScreenStatus = createSelector(
  getController,
  (states: ControllerStates): boolean => states.screenStatus.lock,
);
