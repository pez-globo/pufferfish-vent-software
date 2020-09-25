export const INITIALIZED = '@app/INITIALIZED';
export const SET_LOCALE = '@app/SET_LOCALE';

export const LOCALE_EN_US = 'en-us';
export const LOCALE_DEFAULT = LOCALE_EN_US;
export const CLOCK_UPDATED = '@app/CLOCK_UPDATED';

interface InitializedAction {
  type: typeof INITIALIZED;
}

interface SetLocaleAction {
  type: typeof SET_LOCALE;
  locale: string;
}

interface SetClock {
  type: typeof CLOCK_UPDATED;
}

export type AppAction = InitializedAction | SetLocaleAction | SetClock;
export interface AppState {
  locale: string;
  clock: Date;
}
