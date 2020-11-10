import React from 'react';
import { bindActionCreators } from 'redux';
import { connect, useDispatch } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { ValueModal, SettingAdjustProps } from '../controllers/ValueModal';
import { AlarmModal, Knob } from '../controllers';

import store from '../../store';
import { StoreState } from '../../store/types';
import { getParametersFlow, getSensorMeasurementsFlow } from '../../store/controller/selectors';
import { LMIN } from './units';
import { updateCommittedParameter, updateCommittedState } from '../../store/controller/actions';
import { PARAMETER_STANDBY } from '../../store/controller/types';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getSensorMeasurementsFlow,
});
const FlowDisplay = connect(displaySelector)(ValueDisplay);

const settingSelector = createStructuredSelector<StoreState, SettingAdjustProps>({
  committedSetting: getParametersFlow,
});
const FlowValueModal = connect(settingSelector)(ValueModal);

const label = 'Flow';
const stateKey = 'flow';
const units = LMIN;

/**
 * FlowInfo
 *
 * A `Knob`-based component for handling Flow information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const FlowInfo = (): JSX.Element => {
  const dispatch = useDispatch();
  const doSetFlow = (setting: number) => {
    dispatch(updateCommittedParameter({ flow: setting }));
    dispatch(updateCommittedState(PARAMETER_STANDBY, { flow: setting }));
  };
  return (
    <Knob
      valueDisplay={<FlowDisplay label={label} units={units} isLive={true} />}
      valueModal={
        <FlowValueModal label={label} units={units} requestCommitSetting={doSetFlow} max={80} />
      }
      alarmModal={
        <AlarmModal
          label={label}
          units={units}
          stateKey={stateKey}
          requestCommitRange={() => null}
        />
      }
    />
  );
};

export default FlowInfo;
