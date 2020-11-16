import React from 'react';
import { bindActionCreators } from 'redux';
import { connect, useDispatch } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { ValueModal, SettingAdjustProps } from '../controllers/ValueModal';
import { AlarmModal, Knob } from '../controllers';

import store from '../../store';
import { StoreState } from '../../store/types';
import { getSensorMeasurementsFiO2, getParametersFiO2 } from '../../store/controller/selectors';
import { PERCENT } from './units';
import { updateCommittedParameter, updateCommittedState } from '../../store/controller/actions';
import { PARAMETER_STANDBY } from '../../store/controller/types';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getSensorMeasurementsFiO2,
});
const FiO2Display = connect(displaySelector)(ValueDisplay);

const settingSelector = createStructuredSelector<StoreState, SettingAdjustProps>({
  committedSetting: getParametersFiO2,
});
const FiO2ValueModal = connect(settingSelector)(ValueModal);

const label = 'FiO2';
const stateKey = 'fio2';
const units = PERCENT;

/**
 * FiO2Info
 *
 * A `Knob`-based component for handling FiO2 information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const FiO2Info = (): JSX.Element => {
  const dispatch = useDispatch();
  const doSetFiO2 = (setting: number) => {
    dispatch(updateCommittedParameter({ fio2: setting }));
    dispatch(updateCommittedState(PARAMETER_STANDBY, { fio2: setting }));
  };
  return (
    <Knob
      valueDisplay={<FiO2Display label={label} units={units} isLive={true} />}
      valueModal={
        <FiO2ValueModal label={label} units={units} requestCommitSetting={doSetFiO2} min={21} />
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

export default FiO2Info;
