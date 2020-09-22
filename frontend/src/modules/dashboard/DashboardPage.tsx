import React from 'react';
import { Grid, makeStyles, Theme } from '@material-ui/core';
import { useSelector } from 'react-redux';
import PressureControlMainView from './views/PressureControlMainView';
import { getParametersRequestMode } from '../../store/controller/selectors';
import { VentilationMode } from '../../store/controller/proto/mcu_pb';
import HFNCMainView from './views/HFNCMainView';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    flexGrow: 1,
    justifyContent: 'center',
    alignItems: 'stretch',
    marginBottom: theme.spacing(2),
  },
}));

const renderModeLayout = (mode: VentilationMode): JSX.Element => {
  switch (mode) {
    case VentilationMode.pc_ac:
    case VentilationMode.pc_simv:
    case VentilationMode.vc_ac:
    case VentilationMode.vc_simv:
    case VentilationMode.niv:
      return <PressureControlMainView />;
    case VentilationMode.hfnc:
    default:
      return <HFNCMainView />;
  }
};

/**
 * DashboardPage
 *
 * TODO: Hook into the redux store to get the current dashboard `view` from the
 * `ViewDropdown`
 */
export const DashboardPage = (): JSX.Element => {
  const classes = useStyles();
  const currentMode = useSelector(getParametersRequestMode);

  return (
    <Grid container className={classes.root}>
      {/* TODO: Switch views based on `view` state. */}
      {renderModeLayout(currentMode)}
    </Grid>
  );
};

export default DashboardPage;
