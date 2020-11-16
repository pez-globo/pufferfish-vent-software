import { AppBar, Button, Grid, makeStyles, Theme } from '@material-ui/core';
import React, { PropsWithChildren, useEffect, useState } from 'react';
import { useSelector } from 'react-redux';
import { Link, Route, RouteProps } from 'react-router-dom';
import { getClockTime } from '../../../store/app/selectors';
import {
  getBatteryPower,
  getParametersRequest,
  getScreenStatus,
} from '../../../store/controller/selectors';
import ClockIcon from '../../icons/ClockIcon';
import PowerFullIcon from '../../icons/PowerFullIcon';
import { PERCENT } from '../../info/units';
import {
  DASHBOARD_ROUTE,
  LOGS_ROUTE,
  QUICKSTART_ROUTE,
  SCREENSAVER_ROUTE,
} from '../../navigation/constants';
import EventAlerts from '../EventAlerts';
import Sidebar from '../Sidebar';
import UserActivity from '../UserActivity';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100vh',
    backgroundColor: theme.palette.background.default,
    flexWrap: 'nowrap',
    display: 'grid',
    gridTemplateAreas: `
                    'vent vent'`,
    gridTemplateColumns: '90px 1fr',
  },
  main: {
    gridGap: '15px',
    display: 'grid',
    padding: '15px',
    height: '100vh',
    width: '100%',
    flexWrap: 'nowrap',
    gridArea: 'vent',
    gridTemplateRows: '40px 1fr',
    overflow: 'hidden',
  },
  mainContainer: {
    height: '100%',
  },
  overlay: {
    width: '100%',
    height: '100%',
    background: 'rgb(0 0 0 / 39%)',
    position: 'absolute',
    zIndex: 9999,
  },
  marginRight: {
    marginRight: theme.spacing(2),
  },
  paddingRight: {
    paddingRight: theme.spacing(1),
  },
  screensaverButton: {
    minWidth: 0,
    borderRadius: 8,
    marginRight: '0px',
    padding: '6px 10px',
    lineHeight: 'normal',
  },
}));

const ScreensaverLayout = ({ children, ...rest }: PropsWithChildren<unknown>): JSX.Element => {
  const classes = useStyles();
  const screenStatus = useSelector(getScreenStatus);
  const [overlay, setOverlay] = useState(screenStatus || false);
  const batteryPower = useSelector(getBatteryPower);

  useEffect(() => {
    setOverlay(screenStatus);
  }, [screenStatus]);

  const clockTime = useSelector(getClockTime);
  const parameterRequest = useSelector(getParametersRequest);

  return (
    <React.Fragment>
      {overlay && <div className={classes.overlay} />}
      <Grid container justify="center" alignItems="stretch" className={classes.root}>
        <Grid container item className={classes.main}>
          <Grid container item alignItems="center">
            <AppBar
              color="transparent"
              elevation={0}
              position="static"
              style={{ display: 'contents' }}
            >
              <Grid item style={{ margin: '0 auto' }}>
                <Button
                  component={Link}
                  to={parameterRequest.ventilating ? DASHBOARD_ROUTE.path : QUICKSTART_ROUTE.path}
                  variant="contained"
                  color="primary"
                  className={classes.screensaverButton}
                  disableElevation
                >
                  <div>
                    <SCREENSAVER_ROUTE.icon style={{ fontSize: '1.5rem' }} />
                  </div>
                </Button>
              </Grid>
              <Grid item xs className={classes.marginRight} style={{ margin: '0 auto' }}>
                <EventAlerts path={LOGS_ROUTE.path} label={LOGS_ROUTE.label} />
              </Grid>
              <Grid container item xs justify="flex-end" alignItems="center">
                <span className={classes.paddingRight}>{`${
                  batteryPower !== undefined ? batteryPower.toFixed(0) : '--'
                }${PERCENT}`}</span>
                <PowerFullIcon style={{ fontSize: '2.5rem' }} />
                <span className={classes.paddingRight}>{clockTime}</span>
                <ClockIcon style={{ fontSize: '2.5rem' }} />
              </Grid>
            </AppBar>
          </Grid>
          <Grid container item className={classes.mainContainer}>
            {children}
          </Grid>
        </Grid>
      </Grid>
      <UserActivity />
    </React.Fragment>
  );
};

const ScreensaverRoute = ({ component: Component, ...rest }: RouteProps): JSX.Element | null => {
  if (!Component) return null;
  return (
    <Route
      {...rest}
      render={(matchProps) => (
        <ScreensaverLayout>
          <Component {...matchProps} />
        </ScreensaverLayout>
      )}
    />
  );
};

export default ScreensaverRoute;
