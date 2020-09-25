import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { ValueModal, Knob } from '../controllers';
import { StoreState } from '../../store/types';
import { getCycleMeasurementsVT } from '../../store/controller/selectors';
import { CMH20 } from './units';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getCycleMeasurementsVT,
});
const PplatDisplay = connect(displaySelector)(ValueDisplay);

const label = 'Pplat';
const units = CMH20;

/**
 * Pplat
 *
 * A `Knob`-based component for handling IP Above PEEP information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const PplatInfo = (): JSX.Element => (
  <Knob
    valueDisplay={<PplatDisplay label={label} units={units} isLive={true} />}
    valueModal={
      <ValueModal
        label={label}
        units={units}
        committedSetting={12}
        requestCommitSetting={() => null}
      />
    }
    alarmModal={null}
  />
);

export default PplatInfo;
