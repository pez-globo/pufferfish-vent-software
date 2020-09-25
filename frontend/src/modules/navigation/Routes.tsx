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
  MODES_ROUTE,
  ALARMS_ROUTE,
  SETTINGS_ROUTE,
  VALUES_ROUTE,
  LOGS_ROUTE,
  SCREENSAVER_ROUTE,
} from './constants';

const Routes = (): JSX.Element => (
  <Switch>
    <Route exact path={DASHBOARD_ROUTE.path} component={DashboardPage} />
    <Route path={QUICKSTART_ROUTE.path} component={QuickStartPage} />
    <Route path={MODES_ROUTE.path} component={ModesPage} />
    <Route path={ALARMS_ROUTE.path} component={AlarmsPage} />
    <Route path={VALUES_ROUTE.path} component={ValuesPage} />
    <Route path={SETTINGS_ROUTE.path} component={SettingsPage} />
    <Route path={LOGS_ROUTE.path} component={LogsPage} />
    <Route path={SCREENSAVER_ROUTE.path} component={ScreensaverPage} />
  </Switch>
);

export default Routes;
