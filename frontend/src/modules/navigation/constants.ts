import { GearIcon, LungsIcon, BellIcon, ChartIcon, PufferfishIcon, HomeIcon } from '../icons';
import BellNoneIcon from '../icons/BellNoneIcon';

// Constants for storing objects of route paths.
export const DASHBOARD_ROUTE = { key: 1, label: 'Dashboard', path: '/dashboard', icon: null };
export const QUICKSTART_ROUTE = {
  key: 2,
  label: 'Quick Start',
  path: '/quickstart',
  icon: HomeIcon,
};
export const MODES_ROUTE = { key: 3, label: 'Modes', path: '/modes', icon: LungsIcon };
export const ALARMS_ROUTE = { key: 4, label: 'Set Alarms', path: '/alarms', icon: BellNoneIcon };
export const LOGS_ROUTE = { key: 5, label: 'Events Log', path: '/logs', icon: BellIcon };
export const VALUES_ROUTE = { key: 6, label: 'All Values', path: '/values', icon: ChartIcon };
export const SCREENSAVER_ROUTE = {
  key: 7,
  label: 'Pufferfish',
  path: '/screensaver',
  icon: PufferfishIcon,
};
export const SETTINGS_ROUTE = { key: 8, label: 'Settings', path: '/settings', icon: GearIcon };
export const DASHBOARD2_ROUTE = { key: 9, label: 'New Dashboard', path: '/dashboard2', icon: null };
