import { loadFeature, defineFeature } from 'jest-cucumber';
import { h } from 'preact';
import * as React from 'react';
import { render, fireEvent, RenderResult } from '@testing-library/preact';
import configureStore, { MockStore } from 'redux-mock-store';
import { MuiThemeProvider } from '@material-ui/core/styles';
import { Provider } from 'react-redux';
import { takeEvery, takeLatest } from 'redux-saga/effects';
import { AlarmModal } from '../../modules/controllers';
import { darkTheme } from '../../styles/customTheme';
import { updateCommittedState } from '../../store/controller/actions';
import { ALARM_LIMITS, commitAction } from '../../store/controller/types';
import { INITIALIZED } from '../../store/app/types';

const feature = loadFeature('src/spec/features/alarm-modal.feature');

defineFeature(feature, (test) => {
  const mockStore = configureStore();
  let wrapper: RenderResult;
  let store: MockStore;
  let commitChange: (min: number, max: number) => void;

  beforeEach(() => {
    commitChange = jest.fn();
    store = mockStore({ controller: { alarmLimitsRequest: { rrMin: 0, rrMax: 100 } } });
    wrapper = render(
      <Provider store={store}>
        <MuiThemeProvider theme={darkTheme}>
          <AlarmModal
            label="dummy"
            units=""
            committedMin={0}
            committedMax={100}
            requestCommitRange={commitChange}
            stateKey="rr"
          />
        </MuiThemeProvider>
      </Provider>,
    );
    fireEvent.click(wrapper.getByText(/alarm/i, { exact: false }));
  });

  test('To check if Alarm increment is working', ({ given, when, then }) => {
    let valueClickerMin: HTMLElement;

    given(/^I mount Alarm modal component$/, () => {
      valueClickerMin = wrapper.getAllByRole('heading')[2];
      expect(wrapper).toBeDefined();
      expect(valueClickerMin).toBeDefined();
    });

    when(/^I click on increment button$/, async () => {
      const buttons = wrapper.getAllByRole('button');
      expect(valueClickerMin.textContent).toBe('0');
      await fireEvent.click(buttons[0]);
    });

    then(/^I should get the result as incremented value$/, () => {
      expect(valueClickerMin.textContent).toBe('1');
    });
  });

  test('To check if Alarm decrement is working', ({ given, when, then }) => {
    let valueClickerMax: HTMLElement;

    given(/^I mount Alarm modal component$/, () => {
      valueClickerMax = wrapper.getAllByRole('heading')[3];
      expect(wrapper).toBeDefined();
      expect(valueClickerMax).toBeDefined();
    });

    when(/^I click on decrement button$/, async () => {
      const buttons = wrapper.getAllByRole('button');
      expect(valueClickerMax.textContent).toBe('100');
      await fireEvent.click(buttons[3]);
    });

    then(/^I should get the result as decremented value$/, () => {
      expect(valueClickerMax.textContent).toBe('99');
    });
  });

  test('To check if confirm button is working', ({ given, when, then }) => {
    given(/^I set alarm limit values$/, async () => {
      const buttons = wrapper.getAllByRole('button');
      expect(wrapper).toBeDefined();
      await fireEvent.click(buttons[0]);
      await fireEvent.click(buttons[3]);
    });

    when(/^I click on confirm button$/, async () => {
      const confirmButton = wrapper.getByRole('button', { name: /confirm/i });
      await fireEvent.click(confirmButton);
    });

    then(/^I should get confirm action called with values I have set$/, () => {
      expect(commitChange).toHaveBeenCalledTimes(1);
      expect(commitChange).toBeCalledWith(1, 99);
    });
  });

  test('To check if alarm values are stored in redux', ({ given, when, then }) => {
    let actionCommitted: commitAction;

    given(/^I set alarm limit values$/, async () => {
      actionCommitted = updateCommittedState(ALARM_LIMITS, { rrMin: 10, rrMax: 20 });
    });

    when(/^I commit changes to redux action$/, async () => {
      store.dispatch(actionCommitted);
    });

    then(/^I should get action values verified with triggered one's$/, () => {
      const actions = store.getActions();
      const expectedPayload = {
        type: '@controller/ALARM_LIMITS_COMMITTED',
        update: { rrMin: 10, rrMax: 20 },
      };
      expect(actions).toEqual([expectedPayload]);
    });
  });
});
