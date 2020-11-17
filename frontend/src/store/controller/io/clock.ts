import { put, delay } from 'redux-saga/effects';
import { CLOCK_UPDATED } from '../../app/types';

export default function* updateClock(updateInterval = 1000): IterableIterator<unknown> {
  while (true) {
    yield delay(updateInterval);
    yield put({ type: CLOCK_UPDATED });
  }
}
