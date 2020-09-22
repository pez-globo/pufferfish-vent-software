import { combineReducers, createStore, applyMiddleware } from 'redux';
import createSagaMiddleware from 'redux-saga';
import { composeWithDevTools } from 'redux-devtools-extension';

import { appReducer } from './app/reducers';
import { controllerReducer } from './controller/reducers';
import { controllerSaga } from './controller/saga';

const reducers = combineReducers({
  app: appReducer,
  controller: controllerReducer,
});

const sagaMiddleware = createSagaMiddleware();
const store = createStore(reducers, composeWithDevTools(applyMiddleware(sagaMiddleware)));

sagaMiddleware.run(controllerSaga);

export default store;
