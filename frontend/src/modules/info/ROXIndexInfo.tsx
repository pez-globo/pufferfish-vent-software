import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { AlarmModal, Knob } from '../controllers';

import { StoreState } from '../../store/types';
import { getROXIndex } from '../../store/controller/selectors';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getROXIndex,
});
const ROXIndexDisplay = connect(displaySelector)(ValueDisplay);

const label = 'ROX Index';

/**
 * ROXIndexInfo
 *
 * A `Knob`-based component for handling SpO2 information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const ROXIndexInfo = (): JSX.Element => (
  <Knob
    valueDisplay={<ROXIndexDisplay label={label} isLive={true} />}
    valueModal={null}
    alarmModal={null}
  />
);

export default ROXIndexInfo;
