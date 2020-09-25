import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { AlarmModal, Knob } from '../controllers';

import { StoreState } from '../../store/types';
import { getSensorMeasurementsSpO2, getParametersPEEP } from '../../store/controller/selectors';
import { PERCENT } from './units';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getSensorMeasurementsSpO2,
});
const SPO2Display = connect(displaySelector)(ValueDisplay);

const label = 'SpO2';
const stateKey = 'spo2';
const units = PERCENT;

/**
 * SpO2Info
 *
 * A `Knob`-based component for handling SpO2 information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const SpO2Info = (): JSX.Element => (
  <Knob
    valueDisplay={<SPO2Display label={label} units={units} isLive={true} />}
    valueModal={null}
    alarmModal={
      <AlarmModal label={label} units={units} stateKey={stateKey} requestCommitRange={() => null} />
    }
  />
);

export default SpO2Info;
