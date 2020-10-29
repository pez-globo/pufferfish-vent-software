import React, { PropsWithChildren, useEffect } from 'react';
import { Link, useLocation } from 'react-router-dom';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Tabs, Tab, Typography } from '@material-ui/core';
import {
  QUICKSTART_ROUTE,
  VALUES_ROUTE,
  MODES_ROUTE,
  ALARMS_ROUTE,
  SETTINGS_ROUTE,
} from './constants';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    width: '100%',
    height: '100%',
  },
  tabs: {
    alignText: 'center',
    zIndex: 1,
    height: '100%',
    '&;selected': {
      backgroundColor: 'white',
    },
    '& .MuiTabs-fixed': {
      height: '100%',
      '& .MuiTabs-centered': {
        height: '100%',
      },
    },
  },
  tab: {
    minWidth: 0,
    // minHeight: 80,
    color: theme.palette.text.primary,
    fontSize: theme.typography.subtitle2.fontSize,
    zIndex: 1,
  },
  indicator: {
    color: 'theme.palette.text.primary',
    backgroundColor: 'transparent',
    borderLeft: 'transparent',
    opacity: '1',
    width: '100%',
    zIndex: 0,
  },

  sidebarIcon: {
    border: '2px solid #0056b3b0',
    // opacity: '.5',
    padding: '5px',
    width: '40px',
    height: '40px',
    borderRadius: '8px',

    '&:hover': {
      border: '2px solid #0056B3',
      opacity: '1',
    },
    '&;selected': {
      border: '2px solid #0056B3',
      opacity: '1',
    },
  },
}));
/**
 * Navigation
 *
 * The main interface for router/page-based navigation.
 */
export const Navigation = ({ fullPage }: { fullPage?: boolean }): JSX.Element => {
  const classes = useStyles();
  const location = useLocation();

  const routes = [
    // QUICKSTART_ROUTE, // TODO: Hide QuickStart tab when ventilator is on. Need to tap into redux store.
    MODES_ROUTE,
    ALARMS_ROUTE,
    VALUES_ROUTE,
    SETTINGS_ROUTE,
  ];

  const routePath = routes.find((route) => location.pathname.startsWith(route.path));
  const [route, setRoute] = React.useState(routePath ? routePath.key : 0);

  useEffect(() => {
    const routePath = routes.find((route) => location.pathname.startsWith(route.path));
    setRoute(routePath ? routePath.key : 0);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [location]);

  const handleRouteChange = (
    event: React.ChangeEvent<Record<string, unknown>>,
    newRoute: number,
  ) => {
    setRoute(newRoute);
  };

  const RouteLabel = (
    props: PropsWithChildren<{ fullPage?: boolean; label: string }>,
  ): JSX.Element => {
    const { fullPage, label, children } = props;
    if (fullPage) {
      return (
        <React.Fragment>
          {children}
          <br />
          <Typography variant="subtitle2" align="center">
            {label}
          </Typography>
        </React.Fragment>
      );
    }
    return (
      <React.Fragment>
        <div className={classes.sidebarIcon}>{children}</div>
        <div>
          <Typography variant="subtitle2" align="center">
            {label}
          </Typography>
        </div>
      </React.Fragment>
    );
  };

  return (
    <div className={classes.root}>
      <Tabs
        orientation="vertical"
        variant="fullWidth"
        value={route}
        onChange={handleRouteChange}
        centered
        className={classes.tabs}
        classes={{ indicator: classes.indicator }}
      >
        {routes.map((route) => {
          return (
            <Tab
              value={route.key}
              label={
                <RouteLabel fullPage={fullPage} label={route.label}>
                  <route.icon style={{ width: '100%' }} />
                </RouteLabel>
              }
              component={Link}
              to={route.path}
              className={classes.tab}
            />
          );
        })}
      </Tabs>
    </div>
  );
};

export default Navigation;
