import React, { useCallback, useEffect } from 'react';
import { useLocation, Link } from 'react-router-dom';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Button, Grid, AppBar } from '@material-ui/core';
import { useDispatch, useSelector } from 'react-redux';
import {
  LOGS_ROUTE,
  DASHBOARD_ROUTE,
  QUICKSTART_ROUTE,
  SCREENSAVER_ROUTE,
} from '../navigation/constants';
import ModesDropdown from '../modes/ModesDropdown';
import ViewDropdown from '../dashboard/views/ViewDropdown';
import { BackIcon } from '../icons';
import ClockIcon from '../icons/ClockIcon';
import PowerFullIcon from '../icons/PowerFullIcon';
import { PERCENT } from '../info/units';
import { getClockTime } from '../../store/app/selectors';
import EventAlerts from './EventAlerts';
import {
  getBatteryPower,
  getParametersRequest,
  getParametersRequestMode,
  getParametersRequestStandby,
} from '../../store/controller/selectors';
import { VentilationMode } from '../../store/controller/proto/mcu_pb';
import { updateCommittedParameter } from '../../store/controller/actions';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    // border: '1px solid red',
  },
  marginRight: {
    marginRight: theme.spacing(2),
  },
  paddingRight: {
    paddingRight: theme.spacing(1),
  },
  toolContainer: {
    height: '100%',
    // border: '1px solid red'
  },
}));

/**
 * ToolBar
 *
 * A container for displaying buttons that handle system changes based on
 * various states and conditions such as ventilator state and current page route.
 */
export const ToolBar = ({ children }: { children?: React.ReactNode }): JSX.Element => {
  const classes = useStyles();
  // Store the route location so we can change button/breadcrumb displays
  // depending on the current route.
  const location = useLocation();
  const dispatch = useDispatch();
  const currentMode = useSelector(getParametersRequestMode);
  const parameterRequestStandby = useSelector(getParametersRequestStandby);
  const parameterRequest = useSelector(getParametersRequest);
  const batteryPower = useSelector(getBatteryPower);
  const [isVentilatorOn, setIsVentilatorOn] = React.useState(parameterRequest.ventilating);
  const label = isVentilatorOn ? 'Pause Ventilation' : 'Start Ventilation';
  const toPath = isVentilatorOn ? QUICKSTART_ROUTE.path : DASHBOARD_ROUTE.path;
  const isDisabled = !isVentilatorOn && location.pathname !== QUICKSTART_ROUTE.path;
  const updateVentilationStatus = () => {
    dispatch(updateCommittedParameter({ ventilating: !isVentilatorOn }));
    setIsVentilatorOn(!isVentilatorOn);
  };

  const initParameterUpdate = useCallback(() => {
    if (isVentilatorOn) {
      switch (currentMode) {
        case VentilationMode.pc_ac:
        case VentilationMode.pc_simv:
        case VentilationMode.vc_ac:
        case VentilationMode.vc_simv:
        case VentilationMode.niv:
          dispatch(
            updateCommittedParameter({
              peep: parameterRequestStandby.peep,
              vt: parameterRequestStandby.vt,
              rr: parameterRequestStandby.rr,
              fio2: parameterRequestStandby.fio2,
            }),
          );
          break;
        case VentilationMode.hfnc:
        default:
          dispatch(
            updateCommittedParameter({
              fio2: parameterRequestStandby.fio2,
              flow: parameterRequestStandby.flow,
            }),
          );
          break;
      }
    }
  }, [isVentilatorOn, parameterRequestStandby, currentMode, dispatch]);

  useEffect(() => {
    initParameterUpdate();
  }, [initParameterUpdate]);

  const StartPauseButton = (
    <Button
      component={Link}
      to={toPath}
      onClick={updateVentilationStatus}
      variant="contained"
      color="secondary"
      disabled={isDisabled}
    >
      {label}
    </Button>
  );

  const tools = [<ModesDropdown />];
  if (location.pathname === DASHBOARD_ROUTE.path) {
    tools.push(<ViewDropdown />);
    tools.push(<EventAlerts path={LOGS_ROUTE.path} label={LOGS_ROUTE.label} />);
  } else if (location.pathname === QUICKSTART_ROUTE.path) {
    tools.push(
      <Button variant="contained" color="primary" disabled>
        Last Patient Settings
      </Button>,
    );
  } else if (isVentilatorOn && location.pathname !== SCREENSAVER_ROUTE.path) {
    tools.push(
      <Button component={Link} to={DASHBOARD_ROUTE.path} variant="contained" color="primary">
        <BackIcon style={{ paddingRight: 8 }} />
        {DASHBOARD_ROUTE.label}
      </Button>,
    );
  }
  const clockTime = useSelector(getClockTime);

  return (
    <AppBar color="transparent" elevation={0} position="static">
      <Grid
        container
        direction="row"
        justify="space-between"
        alignItems="center"
        className={classes.root}
        wrap="nowrap"
      >
        {children}
        <Grid
          container
          item
          xs={8}
          direction="row"
          justify="flex-start"
          alignItems="center"
          wrap="nowrap"
          zeroMinWidth
          className={classes.toolContainer}
        >
          {tools.map((tool) => (
            <Grid item className={classes.marginRight}>
              {tool}
            </Grid>
          ))}
        </Grid>
        <Grid
          container
          item
          xs
          direction="row"
          justify="flex-end"
          alignItems="center"
          zeroMinWidth
          className={classes.toolContainer}
        >
          <Grid container item xs justify="flex-end" alignItems="center">
            <span className={classes.paddingRight}>{`${
              batteryPower !== undefined ? batteryPower.toFixed(0) : '--'
            }${PERCENT}`}</span>
            <PowerFullIcon style={{ fontSize: '2.5rem' }} />
            <span className={classes.paddingRight}>{clockTime}</span>
            <ClockIcon style={{ fontSize: '2.5rem' }} />
          </Grid>
          <Grid item>{StartPauseButton}</Grid>
        </Grid>
      </Grid>
    </AppBar>
  );
};

export default ToolBar;
