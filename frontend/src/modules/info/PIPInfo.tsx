import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { ValueModal, SettingAdjustProps } from '../controllers/ValueModal';
import { AlarmModal, Knob } from '../controllers';
import store from '../../store';
import { StoreState } from '../../store/types';
import { getCycleMeasurementsPIP, getParametersPIP } from '../../store/controller/selectors';
import { CMH20 } from './units';
import { updateCommittedParameter } from '../../store/controller/actions';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getCycleMeasurementsPIP,
});
const PIPDisplay = connect(displaySelector)(ValueDisplay);

const doSetPIP = (setting: number) => updateCommittedParameter({ pip: setting });
const boundDoSetPIP = bindActionCreators(doSetPIP, store.dispatch);

const settingSelector = createStructuredSelector<StoreState, SettingAdjustProps>({
  committedSetting: getParametersPIP,
});
const PIPValueModal = connect(settingSelector)(ValueModal);

const label = 'PIP';
const stateKey = 'pip';
const units = CMH20;

/**
 * PIPInfo
 *
 * A `Knob`-based component for handling PIP information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const PIPInfo = (): JSX.Element => (
  <Knob
    valueDisplay={<PIPDisplay label={label} units={units} isLive={true} />}
    valueModal={<PIPValueModal label={label} units={units} requestCommitSetting={boundDoSetPIP} />}
    alarmModal={
      <AlarmModal label={label} units={units} stateKey={stateKey} requestCommitRange={() => null} />
    }
  />
);

export default PIPInfo;
