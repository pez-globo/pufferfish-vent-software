import { PBMessageType } from '../types';
import { getParametersRequest, getAlarmLimitsRequest, getExpectedLogEvent } from '../selectors';
import { MessageSerializer, serializeMessage } from './messages';
import { Schedule } from './states';
import { ParametersRequest, AlarmLimitsRequest, ExpectedLogEvent } from '../proto/mcu_pb';

export const initialSendSchedule: Schedule = [
  { time: 50, pbMessageType: ParametersRequest },
  { time: 50, pbMessageType: AlarmLimitsRequest },
  { time: 50, pbMessageType: ExpectedLogEvent },
];

export const MessageSerializers = new Map<PBMessageType, MessageSerializer>([
  [AlarmLimitsRequest, serializeMessage<AlarmLimitsRequest>(AlarmLimitsRequest)],
  [ParametersRequest, serializeMessage<ParametersRequest>(ParametersRequest)],
  [ExpectedLogEvent, serializeMessage<ExpectedLogEvent>(ExpectedLogEvent)],
]);

// The OutputSelector type is templated and so complicated that it's not clear
// whether we can specify its type in a Map, but for now we'll just delegate the
// responsibility of using types correctly to the calling code.
// eslint-disable @typescript-eslint/no-explicit-any
export const MessageSelectors = new Map<PBMessageType, any>([
  [AlarmLimitsRequest, getAlarmLimitsRequest],
  [ParametersRequest, getParametersRequest],
  [ExpectedLogEvent, getExpectedLogEvent],
]);

export interface StateProcessor {
  // This "any" is needed because MessageSelectors has a value type of "any".
  // eslint-disable @typescript-eslint/no-explicit-any
  selector: any;
  serializer: MessageSerializer;
}

export const getStateProcessor = (pbMessageType: PBMessageType): StateProcessor => {
  const selector = MessageSelectors.get(pbMessageType);
  if (selector === undefined) {
    throw new Error(`Backend: missing selector for ${pbMessageType}`);
  }

  const serializer = MessageSerializers.get(pbMessageType);
  if (serializer === undefined) {
    throw new Error(`Backend: missing message serializer for ${pbMessageType}`);
  }

  return { selector, serializer };
};
