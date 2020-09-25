import { INITIALIZED, SET_LOCALE, AppAction } from './types';

export function initialize(): AppAction {
  return { type: INITIALIZED };
}

export function setLocale(locale: string): AppAction {
  return { type: SET_LOCALE, locale };
}
