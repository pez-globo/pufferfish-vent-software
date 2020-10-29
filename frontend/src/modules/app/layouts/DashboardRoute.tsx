import React, { Component, PropsWithChildren, useEffect, useState } from 'react';
import { Route, RouteProps, useLocation } from 'react-router-dom';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Button, Drawer, Grid } from '@material-ui/core';

import { useSelector } from 'react-redux';
import Routes from '../../navigation/Routes';
import Sidebar from '../Sidebar';
import ToolBar from '../ToolBar';
import UserActivity from '../UserActivity';
import { getScreenStatus } from '../../../store/controller/selectors';
import { SCREENSAVER_ROUTE } from '../../navigation/constants';
import SidebarClickable from '../SidebarClickable';

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
  sidebarGrid: {
    gridArea: 'content',
    height: '100vh',
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
  list: {
    width: 250,
  },
  fullList: {
    width: 'auto',
  },
  screensaverButton: {
    minWidth: 0,
    borderRadius: 5,
    lineHeight: 'normal',
    marginRight: '16px',
    padding: '6px 10px',
  },
  screensaverSidebar: {
    display: 'flex',
    alignItems: 'center',
  },
}));

const FullWidthToolBar = (): JSX.Element => {
  const classes = useStyles();
  const [toggle, setToggle] = React.useState<boolean>(false);

  const toggleDrawer = (value: boolean) => (event: React.MouseEvent<unknown>) => {
    setToggle(value);
  };

  return (
    <ToolBar>
      <Grid>
        <React.Fragment key="left">
          <Button
            onClick={toggleDrawer(true)}
            variant="contained"
            color="primary"
            className={classes.screensaverButton}
            disableElevation
          >
            <div className={classes.screensaverSidebar}>
              <SCREENSAVER_ROUTE.icon style={{ fontSize: '1.5rem' }} />
            </div>
          </Button>
          <Drawer anchor="left" open={toggle} onClose={toggleDrawer(false)}>
            <SidebarClickable toggleStatus={(toggle: boolean) => setToggle(toggle)} />
          </Drawer>
        </React.Fragment>
      </Grid>
    </ToolBar>
  );
};

const SidebarLayout = ({ children, ...rest }: PropsWithChildren<unknown>): JSX.Element => {
  const classes = useStyles();
  const screenStatus = useSelector(getScreenStatus);
  const [overlay, setOverlay] = useState(screenStatus || false);

  useEffect(() => {
    setOverlay(screenStatus);
  }, [screenStatus]);

  return (
    <React.Fragment>
      {overlay && <div className={classes.overlay} />}
      <Grid container justify="center" alignItems="stretch" className={classes.root}>
        <Grid container item direction="column" className={classes.main}>
          <Grid container item alignItems="center">
            <FullWidthToolBar />
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

const DashboardRoute = ({ component: Component, ...rest }: RouteProps): JSX.Element | null => {
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

export default DashboardRoute;
