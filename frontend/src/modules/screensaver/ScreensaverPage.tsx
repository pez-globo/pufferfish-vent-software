import React, { useEffect } from 'react';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Grid } from '@material-ui/core';
import { useSelector } from 'react-redux';
import { OutputSelector } from 'reselect';
import ModeBanner from '../displays/ModeBanner';
import ValueJumbotron from './ValueJumbotron';
import ControlJumbotron from './ControlJumbotron';
import { CMH20, PERCENT, BMIN, LMIN } from '../info/units';
import {
  getSensorMeasurementsSpO2,
  getCycleMeasurementsRR,
  getROXIndex,
  getSensorMeasurementsFiO2,
  getSensorMeasurementsFlow,
  getParametersRequestMode,
} from '../../store/controller/selectors';
import { VentilationMode } from '../../store/controller/proto/mcu_pb';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    border: '1px solid white',
  },
  gridItems: {
    paddingLeft: theme.spacing(2),
    paddingRight: theme.spacing(2),
    paddingTop: theme.spacing(1),
    paddingBottom: theme.spacing(1),
  },
  screenSavergrid: {
    display: 'grid',
    gridGap: '15px',
    gridTemplateRows: '1fr 140px 100px',
    gridTemplateColumns: '1fr',
  },
  screenSaverItem: {
    textAlign: 'center',
    background: '#fff',
    color: '#000',
    fontSize: '17px',
    padding: '10px',
    fontWeight: 'bolder',
    transform: 'rotate(-180deg)',
    writingMode: 'vertical-rl',
  },
}));

const HFNCValueGrid = (): JSX.Element => {
  const classes = useStyles();

  return (
    <React.Fragment>
      <Grid item xs={3} className={classes.gridItems}>
        <ValueJumbotron
          value={useSelector(getSensorMeasurementsSpO2)}
          label="SpO2"
          units={PERCENT}
        />
      </Grid>
      <Grid item xs={3} className={classes.gridItems}>
        <ValueJumbotron value={useSelector(getCycleMeasurementsRR)} label="RR" units={BMIN} />
      </Grid>
      <Grid item xs={6} className={classes.gridItems}>
        <ValueJumbotron value={useSelector(getROXIndex)} label="ROX Index" units="" />
      </Grid>
    </React.Fragment>
  );
};

const HFNCControlGrid = (): JSX.Element => {
  const classes = useStyles();

  return (
    <React.Fragment>
      <Grid item xs={3} className={classes.gridItems}>
        <ControlJumbotron
          value={useSelector(getSensorMeasurementsFiO2)}
          label="FiO2"
          units={PERCENT}
        />
      </Grid>
      <Grid item xs={3} className={classes.gridItems}>
        <ControlJumbotron
          value={useSelector(getSensorMeasurementsFlow)}
          label="Flow Rate"
          units={LMIN}
        />
      </Grid>
      <Grid item xs={3} className={classes.gridItems} />
      <Grid item xs={3} className={classes.gridItems} />
    </React.Fragment>
  );
};

/**
 * ScreensaverPage
 *
 * A farview screensaver page.
 */
export const ScreensaverPage = (): JSX.Element => {
  const classes = useStyles();
  const currentMode = useSelector(getParametersRequestMode);

  const ConfigureControlMode = ({ mode }: { mode: VentilationMode }): JSX.Element => {
    switch (mode) {
      case VentilationMode.pc_ac:
      case VentilationMode.pc_simv:
      case VentilationMode.vc_ac:
      case VentilationMode.vc_simv:
      case VentilationMode.niv:
      case VentilationMode.hfnc:
      default:
        return <HFNCControlGrid />;
    }
  };

  const ConfigureValueMode = ({ mode }: { mode: VentilationMode }): JSX.Element => {
    switch (mode) {
      case VentilationMode.pc_ac:
      case VentilationMode.pc_simv:
      case VentilationMode.vc_ac:
      case VentilationMode.vc_simv:
      case VentilationMode.niv:
      case VentilationMode.hfnc:
      default:
        return <HFNCValueGrid />;
    }
  };

  return (
    <Grid container direction="column" justify="space-between" className={classes.screenSavergrid}>
      <Grid
        container
        xs
        justify="space-evenly"
        alignItems="stretch"
        className={classes.root}
        wrap="nowrap"
      >
        <Grid item xs className={classes.screenSaverItem}>
          Values
        </Grid>
        <ConfigureValueMode mode={currentMode} />
      </Grid>
      <Grid
        container
        xs
        justify="space-evenly"
        alignItems="stretch"
        className={classes.root}
        wrap="nowrap"
      >
        <Grid item xs className={classes.screenSaverItem}>
          Control
        </Grid>
        <ConfigureControlMode mode={currentMode} />
      </Grid>
      <Grid item>
        <ModeBanner bannerType="screenSaver" />
      </Grid>
    </Grid>
  );
};

export default ScreensaverPage;
