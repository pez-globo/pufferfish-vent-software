import { NextLogEvents } from '../proto/mcu_pb';
import { RotaryEncoder } from '../proto/frontend_pb';
import {
  MessageType,
  PBMessage,
  PBMessageType,
  StateUpdateAction,
  STATE_UPDATED,
  RotaryEncoderParameter,
} from '../types';

export const messageReducer = <T extends PBMessage>(
  messageType: MessageType,
  pbMessageType: PBMessageType,
) => (state: T = pbMessageType.fromJSON({}) as T, action: StateUpdateAction): T => {
  switch (action.type) {
    case STATE_UPDATED:
      if (action.messageType === messageType) {
        return action.state as T;
      }
      return state;
    default:
      return state;
  }
};

export const nextLogEventsReducer = (
  state: NextLogEvents = NextLogEvents.fromJSON({}),
  action: StateUpdateAction,
): NextLogEvents => {
  switch (action.type) {
    case STATE_UPDATED:
      if (action.messageType === MessageType.NextLogEvents) {
        const newEventState = action.state as NextLogEvents;
        const oldEventState = state as NextLogEvents;
        if (!oldEventState || !oldEventState.elements.length) {
          return newEventState as NextLogEvents;
        }
        const ids = new Set(oldEventState.elements.map((d) => d.id));
        const events = [
          ...oldEventState.elements,
          ...newEventState.elements.filter((d) => !ids.has(d.id)),
        ];
        return {
          ...newEventState,
          elements: events,
        };
      }
      return state;
    default:
      return state;
  }
};

export const rotaryEncoderReducer = (
  state: RotaryEncoder = RotaryEncoder.fromJSON({}),
  action: StateUpdateAction,
): RotaryEncoderParameter => {
  switch (action.type) {
    case STATE_UPDATED:
      if (action.messageType === MessageType.RotaryEncoder) {
        const newState = action.state as RotaryEncoder;
        const oldState = state as RotaryEncoder;
        const stepDiff = newState.step - oldState.step;
        const stateCopy = { ...newState } as RotaryEncoderParameter;
        stateCopy.stepDiff = stepDiff;
        return stateCopy;
      }
      return state as RotaryEncoderParameter;
    default:
      return state as RotaryEncoderParameter;
  }
};
