import { loadFeature, defineFeature } from 'jest-cucumber';
import { h } from 'preact';
import * as React from 'react';
import { delay, takeEvery, takeLatest } from 'redux-saga/effects';
import controllerSaga, { serviceConnectionPersistently } from '../../store/controller/saga';
import updateClock from '../../store/controller/io/clock';
import { INITIALIZED } from '../../store/app/types';

const feature = loadFeature('src/spec/features/saga.feature');

defineFeature(feature, (test) => {
  test('To check if redux saga workers is running as expected', ({ given, when, then }) => {
    let genObject: IterableIterator<unknown>;

    given(/^Saga worker is initialized$/, async () => {
      genObject = controllerSaga();
    });

    when(/^Saga workers are defined$/, async () => {
      expect(controllerSaga).toBeDefined();
      expect(serviceConnectionPersistently).toBeDefined();
      expect(updateClock).toBeDefined();
    });

    then(/^Saga should run workers for socket connection & clock$/, () => {
      expect(genObject.next().value).toEqual(takeEvery(INITIALIZED, serviceConnectionPersistently));
      expect(genObject.next().value).toEqual(takeLatest(INITIALIZED, updateClock));
    });
  });

  test('To check if redux saga worker for clock works', ({ given, when, then }) => {
    let genObject: IterableIterator<unknown>;

    given(/^Saga Clock worker is initialized$/, async () => {
      genObject = updateClock();
    });

    when(/^Saga Clock worker is defined$/, async () => {
      expect(updateClock).toBeDefined();
    });

    then(/^Saga worker should dispatch clock action with 1000ms delay$/, () => {
      const firstValue: IteratorResult<unknown> = genObject.next();
      expect(firstValue.value.type).toBe('CALL');
      expect(firstValue.value).toEqual(delay(1000));
      const nextValue: IteratorResult<unknown> = genObject.next();
      expect(nextValue.value.type).toBe('PUT');
      expect(nextValue.value.payload.action).toEqual({ type: '@app/CLOCK_UPDATED' });
    });
  });
});
