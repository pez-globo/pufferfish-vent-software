import { GearIcon, LungsIcon, BellIcon, ChartIcon, PufferfishIcon, HomeIcon } from '../icons';
import BellNoneIcon from '../icons/BellNoneIcon';

// Constants for storing objects of route paths.
export const DASHBOARD_ROUTE = { label: 'Dashboard', path: '/dashboard', icon: null };
export const QUICKSTART_ROUTE = { label: 'Quick Start', path: '/quickstart', icon: HomeIcon };
export const MODES_ROUTE = { label: 'Modes', path: '/modes', icon: LungsIcon };
export const ALARMS_ROUTE = { label: 'Set Alarms', path: '/alarms', icon: BellNoneIcon };
export const LOGS_ROUTE = { label: 'Events Log', path: '/logs', icon: BellIcon };
export const VALUES_ROUTE = { label: 'All Values', path: '/values', icon: ChartIcon };
export const SCREENSAVER_ROUTE = {
  label: 'Pufferfish',
  path: '/screensaver',
  icon: PufferfishIcon,
};
export const SETTINGS_ROUTE = { label: 'Settings', path: '/settings', icon: GearIcon };
