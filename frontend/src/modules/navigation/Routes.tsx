import { Route, Switch } from 'react-router-dom';
import React from 'react';
import { useSelector } from 'react-redux';
import QuickStartPage from '../quickstart/QuickStartPage';
import AlarmsPage from '../alarms/AlarmsPage';
import ValuesPage from '../values/ValuesPage';
import ModesPage from '../modes/ModesPage';
import DashboardPage from '../dashboard/DashboardPage';
import SettingsPage from '../settings/SettingsPage';
import LogsPage from '../logs/LogsPage';
import ScreensaverPage from '../screensaver/ScreensaverPage';
import {
  DASHBOARD_ROUTE,
  QUICKSTART_ROUTE,
  DASHBOARD2_ROUTE,
  MODES_ROUTE,
  ALARMS_ROUTE,
  SETTINGS_ROUTE,
  VALUES_ROUTE,
  LOGS_ROUTE,
  SCREENSAVER_ROUTE,
} from './constants';
import LandingPage from '../landing-page/LandingPage';
import NoLayoutRoute from '../app/layouts/NoLayoutRoute';
import SidebarRoute from '../app/layouts/SidebarRoute';
import { getParameters, getParametersRequest } from '../../store/controller/selectors';
import SidebarSlideRoute from '../app/layouts/SidebarSlideRoute';
import ScreensaverRoute from '../app/layouts/ScreensaverRoute';

const Routes = (): JSX.Element => {
  const paramters = useSelector(getParametersRequest);
  const RouteLayout = paramters.ventilating ? SidebarSlideRoute : SidebarRoute;
  return (
    <Switch>
      <RouteLayout exact path={DASHBOARD_ROUTE.path} component={DashboardPage} />
      <RouteLayout path={QUICKSTART_ROUTE.path} component={QuickStartPage} />
      <RouteLayout path={MODES_ROUTE.path} component={ModesPage} />
      <RouteLayout path={ALARMS_ROUTE.path} component={AlarmsPage} />
      <RouteLayout path={VALUES_ROUTE.path} component={ValuesPage} />
      <RouteLayout path={SETTINGS_ROUTE.path} component={SettingsPage} />
      <RouteLayout path={LOGS_ROUTE.path} component={LogsPage} />
      <ScreensaverRoute path={SCREENSAVER_ROUTE.path} component={ScreensaverPage} />
      <NoLayoutRoute component={LandingPage} />
    </Switch>
  );
};

export default Routes;
