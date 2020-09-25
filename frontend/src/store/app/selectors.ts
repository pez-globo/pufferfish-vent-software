import { createSelector } from 'reselect';
import enUS from '../../translations/en-us.json';
import { StoreState } from '../types';
import { LOCALE_DEFAULT, LOCALE_EN_US, AppState } from './types';

const MESSAGE_CATALOG: Record<string, Record<string, string>> = {
  [LOCALE_EN_US]: enUS,
};

export const getLocale = ({ app }: StoreState): string => app.locale;
export const getLocales = (): Array<string> => [LOCALE_EN_US];
export const getMessages = createSelector(
  getLocale,
  (locale: string): Record<string, string> => {
    return {
      ...MESSAGE_CATALOG[LOCALE_DEFAULT],
      ...MESSAGE_CATALOG[locale],
    };
  },
);

export const getApp = (store: StoreState): AppState => store.app;

// Clock
export const getClock = createSelector(getApp, (app: AppState): Date => app.clock);

export const getClockTime = createSelector(getApp, (states: AppState): string =>
  states.clock.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', hour12: true }),
);
