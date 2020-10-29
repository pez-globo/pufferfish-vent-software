import { Route, Switch } from 'react-router-dom';
import React from 'react';
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
import DashboardRoute from '../app/layouts/DashboardRoute';

const Routes = (): JSX.Element => (
  <Switch>
    <DashboardRoute exact path={DASHBOARD_ROUTE.path} component={DashboardPage} />
    <SidebarRoute path={QUICKSTART_ROUTE.path} component={QuickStartPage} />
    <SidebarRoute path={MODES_ROUTE.path} component={ModesPage} />
    <SidebarRoute path={ALARMS_ROUTE.path} component={AlarmsPage} />
    <SidebarRoute path={VALUES_ROUTE.path} component={ValuesPage} />
    <SidebarRoute path={SETTINGS_ROUTE.path} component={SettingsPage} />
    <SidebarRoute path={LOGS_ROUTE.path} component={LogsPage} />
    <SidebarRoute path={SCREENSAVER_ROUTE.path} component={ScreensaverPage} />
    <NoLayoutRoute component={LandingPage} />
  </Switch>
);

export default Routes;
