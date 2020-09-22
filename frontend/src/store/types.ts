import { AppState } from './app/types';
import { ControllerStates } from './controller/types';

export interface StoreState {
  app: AppState;
  controller: ControllerStates;
}
