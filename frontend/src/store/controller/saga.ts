import { EventChannel } from 'redux-saga';
import {
  put,
  take,
  takeEvery,
  fork,
  delay,
  select,
  takeLatest,
  all,
  ChannelTakeEffect,
} from 'redux-saga/effects';
import { INITIALIZED } from '../app/types';
import { PBMessageType } from './types';
import { updateState } from './actions';
import { deserializeMessage } from './protocols/messages';
import { advanceSchedule } from './protocols/states';
import { getStateProcessor, initialSendSchedule } from './protocols/backend';
import { createReceiveChannel, receiveBuffer, sendBuffer, setupConnection } from './io/websocket';
import updateClock from './io/clock';

function* deserializeResponse(response: Response) {
  const buffer = yield receiveBuffer(response);
  return deserializeMessage(buffer);
}

function* receive(response: ChannelTakeEffect<Response>) {
  try {
    const results = yield deserializeResponse(yield response);
    yield put(updateState(results.messageType, results.pbMessage));
  } catch (err) {
    console.error(err);
  }
}

function* receiveAll(channel: EventChannel<Response>) {
  while (true) {
    const response = yield take(channel);
    yield receive(response);
  }
}

function* sendState(sock: WebSocket, pbMessageType: PBMessageType) {
  const processor = getStateProcessor(pbMessageType);
  const pbMessage = yield select(processor.selector);
  const body = processor.serializer(pbMessage);
  yield sendBuffer(sock, body);
}

function* sendAll(sock: WebSocket) {
  const schedule = Array.from(initialSendSchedule);
  while (sock.readyState === WebSocket.OPEN) {
    const { time, pbMessageType } = advanceSchedule(schedule);
    yield sendState(sock, pbMessageType);
    yield delay(time);
  }
}

function* serviceConnection() {
  const { sock, connectionChannel } = yield setupConnection();
  const receiveChannel = createReceiveChannel(sock);
  yield fork(receiveAll, receiveChannel);
  yield fork(sendAll, sock);
  const connection = yield take(connectionChannel);
  receiveChannel.close();
}

export function* serviceConnectionPersistently(): IterableIterator<unknown> {
  while (true) {
    yield serviceConnection();
    console.warn('Reestablishing WebSocket connection...');
  }
}

export function* controllerSaga(): IterableIterator<unknown> {
  yield all([
    yield takeEvery(INITIALIZED, serviceConnectionPersistently),
    yield takeLatest(INITIALIZED, updateClock),
  ]);
}

export default controllerSaga;
