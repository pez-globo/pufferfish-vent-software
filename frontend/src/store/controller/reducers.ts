import { combineReducers, Action } from 'redux';
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
import {
  RotaryEncoder,
  FrontendDisplaySetting,
  ThemeVariant,
  Unit,
  SystemSettingRequest,
} from './proto/frontend_pb';
import {
  MessageType,
  ParameterCommitAction,
  PARAMETER_COMMITTED,
  PBMessage,
  PBMessageType,
  StateUpdateAction,
  STATE_UPDATED,
  WaveformHistory,
  WaveformPoint,
  PVHistory,
  PVPoint,
  ALARM_LIMITS,
  FRONTEND_DISPLAY_SETTINGS,
  SYSTEM_SETTINGS,
  commitAction,
} from './types';
import DECIMAL_RADIX from '../../modules/app/AppConstants';

const messageReducer = <T extends PBMessage>(
  messageType: MessageType,
  pbMessageType: PBMessageType,
) => (state: T = pbMessageType.fromJSON({}) as T, action: StateUpdateAction): T => {
  switch (action.type) {
    case STATE_UPDATED:
      if (action.messageType === messageType) {
        return action.state as T;
      }
      return state;
    default:
      return state;
  }
};

const alarmLimitsReducer = (
  state: AlarmLimitsRequest = AlarmLimitsRequest.fromJSON({
    rrMax: 100,
    pipMax: 100,
    peepMax: 100,
    ipAbovePeepMax: 100,
    inspTimeMax: 100,
    fio2Max: 100,
    pawMax: 100,
    mveMax: 100,
    tvMax: 100,
    etco2Max: 100,
    flowMax: 100,
    apneaMax: 100,
    spo2Max: 100,
  }) as AlarmLimitsRequest,
  action: commitAction,
): AlarmLimitsRequest => {
  return withRequestUpdate<AlarmLimitsRequest>(state, action, ALARM_LIMITS);
};

const frontendDisplaySettingReducer = (
  state: FrontendDisplaySetting = FrontendDisplaySetting.fromJSON({
    theme: ThemeVariant.dark,
    unit: Unit.imperial,
  }) as FrontendDisplaySetting,
  action: commitAction,
): FrontendDisplaySetting => {
  return withRequestUpdate<FrontendDisplaySetting>(state, action, FRONTEND_DISPLAY_SETTINGS);
};

const systemSettingRequestReducer = (
  state: SystemSettingRequest = SystemSettingRequest.fromJSON({
    brightness: 100,
    date: parseInt((new Date().getTime() / 1000).toFixed(0), DECIMAL_RADIX),
  }) as SystemSettingRequest,
  action: commitAction,
): SystemSettingRequest => {
  return withRequestUpdate<SystemSettingRequest>(state, action, SYSTEM_SETTINGS);
};

const withRequestUpdate = <T>(state: T, action: commitAction, prefix: string): T => {
  switch (action.type) {
    case STATE_UPDATED: // ignore message from backend
      return state;
    case `@controller/${prefix}_COMMITTED`:
      return { ...state, ...action.update } as T;
    default:
      return state;
  }
};

const parametersRequestReducer = (
  state: ParametersRequest = ParametersRequest.fromJSON({
    mode: VentilationMode.hfnc,
    pip: 30,
    peep: 0,
    rr: 30,
    ie: 1.0,
    fio2: 60.0,
  }) as ParametersRequest,
  action: StateUpdateAction | ParameterCommitAction,
): ParametersRequest => {
  switch (action.type) {
    case STATE_UPDATED: // ignore message from backend
      return state;
    case PARAMETER_COMMITTED:
      return { ...state, ...action.update };
    default:
      return state;
  }
};

const waveformHistoryReducer = <T extends PBMessage>(
  messageType: MessageType,
  getTime: (values: T) => number,
  getValue: (values: T) => number,
  bufferDuration = 60,
  segmentUpdateOffset = 0,
  maxDuration = 10000,
  gapDuration = 500,
  maxSegmentDuration = 2500,
) => (
  state: WaveformHistory = {
    waveformOld: {
      full: [],
    },
    waveformNew: {
      full: [],
      buffer: [],
      segmented: [[]],
    },
    waveformNewStart: 0,
  },
  action: StateUpdateAction,
): WaveformHistory => {
  switch (action.type) {
    case STATE_UPDATED:
      if (action.messageType === messageType) {
        const sampleTime = getTime(action.state as T);
        const lastTime =
          state.waveformNew.full.length === 0
            ? state.waveformNewStart
            : state.waveformNew.full[state.waveformNew.full.length - 1].date;
        if (
          sampleTime > state.waveformNewStart + maxDuration ||
          sampleTime < lastTime ||
          new Date(sampleTime - gapDuration - state.waveformNewStart) > lastTime
        ) {
          // make waveformNew start over
          const newPoint = {
            date: new Date(0),
            value: getValue(action.state as T),
          };
          return {
            waveformOld: {
              full: state.waveformNew.full,
            },
            waveformNew: {
              full: [],
              buffer: [newPoint],
              segmented: [[]],
            },
            waveformNewStart: sampleTime,
          };
        }

        // update buffer
        let buffered = [...state.waveformNew.buffer];
        const newPointTime = sampleTime - state.waveformNewStart;
        const newPoint = {
          date: new Date(newPointTime),
          value: getValue(action.state as T),
        };
        buffered = buffered.concat([newPoint]);

        // apply segment update offset
        let segments = [...state.waveformNew.segmented];
        const bufferedStart = buffered[0].date.getTime();
        const bufferedEnd = buffered[buffered.length - 1].date.getTime();
        if (segments.length === 1 && bufferedEnd - bufferedStart < segmentUpdateOffset) {
          return {
            ...state,
            waveformNew: {
              ...state.waveformNew,
              buffer: buffered,
            },
          };
        }

        // update segmented
        const lastSegment = segments[segments.length - 1];
        if (lastSegment.length === 0) {
          segments[segments.length - 1] = buffered;
          buffered = [];
        } else {
          const lastSegmentStart = lastSegment[0].date.getTime();
          const lastSegmentEnd = lastSegment[lastSegment.length - 1].date.getTime();
          if (newPointTime - lastSegmentEnd >= bufferDuration) {
            const lastSegmentDuration = newPointTime - lastSegmentStart;
            if (lastSegmentDuration >= maxSegmentDuration) {
              // start a new segment, but add an overlap of points
              segments[segments.length - 1] = lastSegment.concat([buffered[0]]);
              segments = segments.concat([buffered]);
            } else {
              segments[segments.length - 1] = lastSegment.concat(buffered);
            }
            buffered = [];
          }
        }

        return {
          ...state,
          waveformNew: {
            full: state.waveformNew.full.concat([newPoint]),
            buffer: buffered,
            segmented: segments,
          },
        };
      }
      return state;
    default:
      return state;
  }
};

const pvHistoryReducer = (
  state: PVHistory = {
    loop: [],
    loopOrigin: {
      pressure: 0,
      volume: 0,
    },
    cycle: 0,
  },
  action: StateUpdateAction,
): PVHistory => {
  switch (action.type) {
    case STATE_UPDATED:
      if (action.messageType === MessageType.SensorMeasurements) {
        const newState = action.state as SensorMeasurements;
        const { cycle, paw, volume } = newState;
        if (cycle !== state.cycle) {
          // make loop start over
          return {
            loop: [{ pressure: 0, volume: 0 }],
            loopOrigin: { pressure: paw, volume },
            cycle,
          };
        }

        // add point to loop
        const newPoint = {
          pressure: paw - state.loopOrigin.pressure,
          volume: volume - state.loopOrigin.volume,
        };
        return {
          ...state,
          loop: state.loop.concat([newPoint]),
        };
      }
      return state;
    default:
      return state;
  }
};

export const controllerReducer = combineReducers({
  // Message states from mcu_pb
  alarms: messageReducer<Alarms>(MessageType.Alarms, Alarms),
  alarmLimitsRequest: alarmLimitsReducer,
  systemSettingRequest: systemSettingRequestReducer,
  frontendDisplaySetting: frontendDisplaySettingReducer,
  sensorMeasurements: messageReducer<SensorMeasurements>(
    MessageType.SensorMeasurements,
    SensorMeasurements,
  ),
  cycleMeasurements: messageReducer<CycleMeasurements>(
    MessageType.CycleMeasurements,
    CycleMeasurements,
  ),
  parameters: messageReducer<Parameters>(MessageType.Parameters, Parameters),
  parametersRequest: parametersRequestReducer,
  ping: messageReducer<Ping>(MessageType.Ping, Ping),
  announcement: messageReducer<Announcement>(MessageType.Announcement, Announcement),

  // Message states from frontend_pb
  rotaryEncoder: messageReducer<RotaryEncoder>(MessageType.RotaryEncoder, RotaryEncoder),

  // Derived states
  waveformHistoryPaw: waveformHistoryReducer<SensorMeasurements>(
    MessageType.SensorMeasurements,
    (sensorMeasurements: SensorMeasurements) => sensorMeasurements.time,
    (sensorMeasurements: SensorMeasurements) => sensorMeasurements.paw,
    60,
    0,
  ),
  waveformHistoryFlow: waveformHistoryReducer<SensorMeasurements>(
    MessageType.SensorMeasurements,
    (sensorMeasurements: SensorMeasurements) => sensorMeasurements.time,
    (sensorMeasurements: SensorMeasurements) => sensorMeasurements.flow,
    60,
    20,
  ),
  waveformHistoryVolume: waveformHistoryReducer<SensorMeasurements>(
    MessageType.SensorMeasurements,
    (sensorMeasurements: SensorMeasurements) => sensorMeasurements.time,
    (sensorMeasurements: SensorMeasurements) => sensorMeasurements.volume,
    60,
    40,
  ),
  pvHistory: pvHistoryReducer,
});

export default controllerReducer;
