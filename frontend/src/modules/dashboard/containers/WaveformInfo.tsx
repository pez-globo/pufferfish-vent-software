import React, { Fragment } from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { Grid, Typography } from '@material-ui/core';
import { StoreState } from '../../../store/types';
import {
  getSensorMeasurementsTime,
  getSensorMeasurementsPaw,
  getSensorMeasurementsFlow,
  getSensorMeasurementsVolume,
} from '../../../store/controller/selectors';

interface Props {
  time: number;
  paw: number;
  flow: number;
  volume: number;
}

export const WaveformInfo = ({ time, paw, flow, volume }: Props): JSX.Element => (
  <Grid container item direction="column" alignItems="center" justify="space-evenly">
    <>
      <Grid item xs={3}>
        <Typography>{time}</Typography>
      </Grid>
      <Grid item xs={3}>
        <Typography>{paw}</Typography>
      </Grid>
      <Grid item xs={3}>
        <Typography>{flow}</Typography>
      </Grid>
      <Grid item xs={3}>
        <Typography>{volume}</Typography>
      </Grid>
    </>
  </Grid>
);

const selector = createStructuredSelector<StoreState, Props>({
  time: getSensorMeasurementsTime,
  paw: getSensorMeasurementsPaw,
  flow: getSensorMeasurementsFlow,
  volume: getSensorMeasurementsVolume,
});

export default connect(selector)(WaveformInfo);
