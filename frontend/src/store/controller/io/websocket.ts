import { eventChannel, EventChannel } from 'redux-saga';
import { call, take, delay, apply, CallEffect, TakeEffect } from 'redux-saga/effects';

export interface ConnectionEvent {
  err: Event | string | null;
  sock: WebSocket | null;
}

export const createConnectionChannel = (
  host = 'localhost',
  port = 8000,
): EventChannel<ConnectionEvent> => {
  return eventChannel((emit) => {
    const sock = new WebSocket(`ws://${host}:${port}/`);
    sock.onerror = (err) => emit({ err, sock: null });
    sock.onopen = (event) => emit({ err: null, sock });
    sock.onclose = (event) => emit({ err: 'Closing!', sock });
    return () => {
      // console.log('Closing WebSocket...');
      sock.close();
    };
  });
};

export const createReceiveChannel = (sock: WebSocket): EventChannel<Response> => {
  const sockCopy = sock;
  return eventChannel((emit) => {
    sockCopy.onmessage = (message) => emit(new Response(message.data));
    return () => {
      sockCopy.close();
    };
  });
};

export function* receiveBuffer(
  response: Response,
): Generator<Promise<ArrayBuffer>, Uint8Array, Iterable<number>> {
  return new Uint8Array(yield response.arrayBuffer());
}

export function* sendBuffer(
  sock: WebSocket,
  buffer: Uint8Array | undefined,
): Generator<CallEffect<void>, void, unknown> {
  if (buffer === undefined) {
    return;
  }

  yield apply(sock, sock.send, [buffer]);
}

export interface Connection {
  sock: WebSocket;
  connectionChannel: EventChannel<ConnectionEvent>;
}

export function* setupConnection(
  retryInterval = 10,
): Generator<
  CallEffect<true | EventChannel<ConnectionEvent>> | TakeEffect | CallEffect<true>,
  // The Generator type is templated and so complicated that we can't specify its type.
  // eslint-disable @typescript-eslint/no-explicit-any
  any,
  // The Generator type is templated and so complicated that we can't specify its type.
  // eslint-disable @typescript-eslint/no-explicit-any
  any
> {
  while (true) {
    const connectionChannel = yield call(createConnectionChannel);
    const connection = yield take(connectionChannel);
    if (connection.err == null) {
      return { sock: connection.sock, connectionChannel };
    }
    // console.warn('WebSocket connection error', connection.err)
    yield delay(retryInterval);
  }
  // console.log('Connected to WebSocket!');
}
