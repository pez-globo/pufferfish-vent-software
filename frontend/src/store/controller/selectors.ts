import { createSelector, OutputSelector } from 'reselect';
import { StoreState } from '../types';
import {
  Alarms,
  SensorMeasurements,
  CycleMeasurements,
  Parameters,
  ParametersRequest,
  Ping,
  Announcement,
  AlarmLimitsRequest,
  VentilationMode,
} from './proto/mcu_pb';
import { RotaryEncoder, FrontendDisplaySetting, SystemSettingRequest } from './proto/frontend_pb';
import { ControllerStates, WaveformPoint, WaveformHistory } from './types';

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

export const getParametersRequestMode = createSelector(
  getParametersRequest,
  (parametersRequest: ParametersRequest): VentilationMode => parametersRequest.mode,
);

// Ping
export const getPing = createSelector(
  getController,
  (states: ControllerStates): Ping => states.ping,
);

// Announcement
export const getAnnouncement = createSelector(
  getController,
  (states: ControllerStates): Announcement => states.announcement,
);

// RotaryEncoder
export const getRotaryEncoder = createSelector(
  getController,
  (states: ControllerStates): RotaryEncoder => states.rotaryEncoder,
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

// Alarm Limits
export const getAlarmLimitsRequest = createSelector(
  getController,
  (states: ControllerStates): AlarmLimitsRequest | Record<string, number> =>
    states.alarmLimitsRequest,
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
