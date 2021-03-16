import { Grid } from '@material-ui/core';
import { makeStyles, Theme } from '@material-ui/core/styles';
import React, { PropsWithChildren, useEffect } from 'react';
import { Route, RouteProps } from 'react-router-dom';
import { useSelector } from 'react-redux';
import Sidebar from '../Sidebar';
import ToolBar from '../ToolBar';
import UserActivity from '../UserActivity';
import OverlayScreen from '../OverlayScreen';
import { getAlarmNotifyStatus } from '../../../store/app/selectors';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100vh',
    backgroundColor: theme.palette.background.default,
    flexWrap: 'nowrap',
    display: 'grid',
    gridTemplateAreas: `
                    'content vent'`,
    gridTemplateColumns: '90px 1fr',
  },
  sidebarGrid: {
    gridArea: 'content',
    height: '100vh',
  },

  main: {
    gridGap: '10px',
    display: 'grid',
    padding: '15px',
    height: '100vh',
    width: '100%',
    flexWrap: 'nowrap',
    gridArea: 'vent',
    gridTemplateRows: '40px 1fr',
    overflow: 'hidden',
    zIndex: 9,
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
  SidebarbordersOverlay: {
    width: '100%',
    height: '100%',
    position: 'absolute',
    border: '4px solid red',
  },
}));

const SidebarLayout = ({ children }: PropsWithChildren<unknown>): JSX.Element => {
  const classes = useStyles();

  return (
    <React.Fragment>
      <OverlayScreen />
      <Grid container justify="center" alignItems="stretch" className={classes.root}>
        <ContentComponent>{children}</ContentComponent>
      </Grid>
      <UserActivity />
    </React.Fragment>
  );
};

const ContentComponent = React.memo(({ children }: PropsWithChildren<unknown>) => {
  const classes = useStyles();
  const notifyAlarm = useSelector(getAlarmNotifyStatus);
  const [showBorder, setShowBorder] = React.useState(false);

  useEffect(() => {
    setShowBorder(notifyAlarm);
  }, [notifyAlarm]);

  return (
    <React.Fragment>
      <Grid item className={`${showBorder && classes.SidebarbordersOverlay} ${classes.root}`}>
        <Sidebar />
      </Grid>
      <Grid container item direction="column" className={classes.main}>
        <Grid container item alignItems="center">
          <ToolBar />
        </Grid>
        <Grid container item className={classes.mainContainer}>
          {children}
        </Grid>
      </Grid>
    </React.Fragment>
  );
});

const SidebarRoute = ({ component: Component, ...rest }: RouteProps): JSX.Element | null => {
  if (!Component) return null;
  return (
    <Route
      {...rest}
      render={(matchProps) => (
        <SidebarLayout>
          <Component {...matchProps} />
        </SidebarLayout>
      )}
    />
  );
};

export default SidebarRoute;
