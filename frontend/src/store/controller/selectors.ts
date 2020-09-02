import {createSelector} from 'reselect'
import {StoreState} from '../types'
import {
  Alarms,
  SensorMeasurements,
  CycleMeasurements,
  Parameters,
  ParametersRequest,
  Ping,
  Announcement,
  AlarmLimitsRequest,
  FrontendDisplaySetting,
  SystemSettingRequest
} from './proto/mcu_pb'
import {
  RotaryEncoder
} from './proto/frontend_pb'
import {
  ControllerStates,
  WaveformPoint,
  WaveformHistory,
} from './types'

export const getController = (
  {controller}: StoreState
): ControllerStates => controller;

// Alarms
export const getAlarms = createSelector(
  getController,
  (states: ControllerStates): Alarms => states.alarms
)

// SensorMeasurements
export const getSensorMeasurements = createSelector(
  getController,
  (states: ControllerStates): SensorMeasurements => states.sensorMeasurements
)
export const getSensorMeasurementsTime = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.time
)
export const getSensorMeasurementsPaw = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.paw
)
export const getSensorMeasurementsFlow = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.flow
)
export const getSensorMeasurementsVolume = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.volume
)
export const getSensorMeasurementsFiO2 = createSelector(
  getSensorMeasurements,
  (sensorMeasurements: SensorMeasurements): number => sensorMeasurements.fio2
)

// CycleMeasurements
export const getCycleMeasurements = createSelector(
  getController,
  (states: ControllerStates): CycleMeasurements => states.cycleMeasurements
)
export const getCycleMeasurementsPIP = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.pip
)
export const getCycleMeasurementsPEEP = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.peep
)
export const getCycleMeasurementsRR = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.rr
)
export const getCycleMeasurementsVT = createSelector(
  getCycleMeasurements,
  (cycleMeasurements: CycleMeasurements): number => cycleMeasurements.vt
)

// Parameters
export const getParameters = createSelector(
  getController,
  (states: ControllerStates): Parameters => states.parameters
)
export const getParametersPIP = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.pip
)
export const getParametersPEEP = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.peep
)
export const getParametersRR = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.rr
)
export const getParametersFiO2 = createSelector(
  getParameters,
  (parameters: Parameters): number => parameters.fio2
)

// ParametersRequest
export const getParametersRequest = createSelector(
  getController,
  (states: ControllerStates): ParametersRequest => states.parametersRequest
)

export const getParametersRequestMode = createSelector(
  getParametersRequest,
  (parametersRequest: ParametersRequest): number | any => parametersRequest.mode
)

// Ping
export const getPing = createSelector(
  getController,
  (states: ControllerStates): Ping => states.ping
)

// Announcement
export const getAnnouncement = createSelector(
  getController,
  (states: ControllerStates): Announcement => states.announcement
)

// RotaryEncoder
export const getRotaryEncoder = createSelector(
  getController,
  (states: ControllerStates): RotaryEncoder => states.rotaryEncoder
)

// Waveforms
export const getWaveformHistoryPaw = createSelector(
  getController,
  (states: ControllerStates): WaveformHistory => states.waveformHistoryPaw
)
export const getWaveformPawOld = createSelector(
  getWaveformHistoryPaw,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformOld
)
export const getWaveformPawNew = createSelector(
  getWaveformHistoryPaw,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformNew
)
export const getWaveformHistoryFlow = createSelector(
  getController,
  (states: ControllerStates): WaveformHistory => states.waveformHistoryFlow
)
export const getWaveformFlowOld = createSelector(
  getWaveformHistoryFlow,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformOld
)
export const getWaveformFlowNew = createSelector(
  getWaveformHistoryFlow,
  (waveformHistory: WaveformHistory): WaveformPoint[] => waveformHistory.waveformNew
)

// Alarm Limits
export const getAlarmLimitsRequest = createSelector(
  getController,
  (states: ControllerStates): AlarmLimitsRequest => states.alarmLimitsRequest
)

// Display Settings
export const getFrontendDisplaySetting = createSelector(
  getController,
  (states: ControllerStates): FrontendDisplaySetting => states.frontendDisplaySetting
)

// Display Settings
export const getSystemSettingRequest = createSelector(
  getController,
  (states: ControllerStates): SystemSettingRequest => states.systemSettingRequest
)