import {
  ParametersRequest,
  AlarmLimitsRequest,
  VentilationMode,
  ExpectedLogEvent,
} from '../proto/mcu_pb';
import {
  FrontendDisplaySetting,
  ThemeVariant,
  Unit,
  SystemSettingRequest,
} from '../proto/frontend_pb';
import {
  ParameterCommitAction,
  PARAMETER_COMMITTED,
  StateUpdateAction,
  STATE_UPDATED,
  ALARM_LIMITS,
  FRONTEND_DISPLAY_SETTINGS,
  SYSTEM_SETTINGS,
  commitAction,
  EXPECTED_LOG_EVENT_ID,
  ALARM_LIMITS_STANDBY,
  PARAMETER_STANDBY,
} from '../types';
import DECIMAL_RADIX from '../../../modules/app/AppConstants';

export const alarmLimitsReducer = (
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

export const alarmLimitsRequestStandbyReducer = (
  state: { alarmLimits: AlarmLimitsRequest } = {
    alarmLimits: AlarmLimitsRequest.fromJSON({
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
    }),
  } as { alarmLimits: AlarmLimitsRequest },
  action: commitAction,
): { alarmLimits: AlarmLimitsRequest } => {
  switch (action.type) {
    case STATE_UPDATED: // ignore message from backend
      return state;
    case `@controller/${ALARM_LIMITS_STANDBY}_COMMITTED`:
      return {
        alarmLimits: {
          ...state.alarmLimits,
          ...action.update,
        } as AlarmLimitsRequest,
      };
    default:
      return state;
  }
};

export const parametersRequestStanbyReducer = (
  state: { parameters: ParametersRequest } = {
    parameters: ParametersRequest.fromJSON({
      mode: VentilationMode.hfnc,
      pip: 30,
      peep: 0,
      rr: 30,
      ie: 1.0,
      fio2: 60.0,
    }),
  } as { parameters: ParametersRequest },
  action: commitAction,
): { parameters: ParametersRequest } => {
  switch (action.type) {
    case STATE_UPDATED: // ignore message from backend
      return state;
    case `@controller/${PARAMETER_STANDBY}_COMMITTED`:
      return {
        parameters: {
          ...state.parameters,
          ...action.update,
        } as ParametersRequest,
      };
    default:
      return state;
  }
};

export const frontendDisplaySettingReducer = (
  state: FrontendDisplaySetting = FrontendDisplaySetting.fromJSON({
    theme: ThemeVariant.dark,
    unit: Unit.imperial,
  }) as FrontendDisplaySetting,
  action: commitAction,
): FrontendDisplaySetting => {
  return withRequestUpdate<FrontendDisplaySetting>(state, action, FRONTEND_DISPLAY_SETTINGS);
};

export const expectedLogEventReducer = (
  state: ExpectedLogEvent = ExpectedLogEvent.fromJSON({
    id: 0,
  }) as ExpectedLogEvent,
  action: commitAction,
): ExpectedLogEvent => {
  return withRequestUpdate<ExpectedLogEvent>(state, action, EXPECTED_LOG_EVENT_ID);
};

export const systemSettingRequestReducer = (
  state: SystemSettingRequest = SystemSettingRequest.fromJSON({
    brightness: 100,
    date: parseInt((new Date().getTime() / 1000).toFixed(0), DECIMAL_RADIX),
  }) as SystemSettingRequest,
  action: commitAction,
): SystemSettingRequest => {
  return withRequestUpdate<SystemSettingRequest>(state, action, SYSTEM_SETTINGS);
};

export const withRequestUpdate = <T>(state: T, action: commitAction, prefix: string): T => {
  switch (action.type) {
    case STATE_UPDATED: // ignore message from backend
      return state;
    case `@controller/${prefix}_COMMITTED`:
      return { ...state, ...action.update } as T;
    default:
      return state;
  }
};

export const parametersRequestReducer = (
  state: ParametersRequest = ParametersRequest.fromJSON({
    mode: VentilationMode.hfnc,
    pip: 30,
    peep: 0,
    rr: 30,
    ie: 1.0,
    fio2: 60.0,
    ventilating: false,
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
