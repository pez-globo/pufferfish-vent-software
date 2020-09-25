import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { ValueDisplay, ValueProps } from '../displays/ValueDisplay';
import { ValueModal, AlarmModal, Knob } from '../controllers';
import { StoreState } from '../../store/types';
import { getCycleMeasurementsVT } from '../../store/controller/selectors';
import { CMH20 } from './units';

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getCycleMeasurementsVT,
});
const IPAbovePEEPDisplay = connect(displaySelector)(ValueDisplay);

const label = 'IP > PEEP';
const stateKey = 'ipAbovePeep';
const units = CMH20;

/**
 * IPAbovePEEP
 *
 * A `Knob`-based component for handling IP Above PEEP information.
 *
 * TODO: Hook this component into the redux store with correct selectors.
 */
const IPAbovePEEPInfo = (): JSX.Element => (
  <Knob
    valueDisplay={<IPAbovePEEPDisplay label={label} units={units} isLive={true} />}
    valueModal={
      <ValueModal
        label={label}
        units={units}
        committedSetting={12}
        requestCommitSetting={() => null}
      />
    }
    alarmModal={
      <AlarmModal label={label} units={units} stateKey={stateKey} requestCommitRange={() => null} />
    }
  />
);

export default IPAbovePEEPInfo;
