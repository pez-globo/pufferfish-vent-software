import React from 'react'
import { bindActionCreators } from 'redux'
import { connect } from 'react-redux'
import { createStructuredSelector } from 'reselect'
import {
  ValueDisplay, ValueProps
} from '../displays/ValueDisplay'
import {
  ValueModal, SettingAdjustProps
} from '../controllers/ValueModal'
import { AlarmModal, Knob } from '../controllers'

import store from '../../store'
import { StoreState } from '../../store/types'
import {
  getSensorMeasurementsFiO2,
  getParametersFiO2
} from '../../store/controller/selectors'
import { PERCENT } from './units'
import { updateCommittedParameter } from '../../store/controller/actions'

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getSensorMeasurementsFiO2
})
const FiO2Display = connect(displaySelector)(ValueDisplay)

const doSetFiO2 = (setting: number) => updateCommittedParameter({ fio2: setting })
const boundDoSetFiO2 = bindActionCreators(doSetFiO2, store.dispatch)

const settingSelector = createStructuredSelector<StoreState, SettingAdjustProps>({
  committedSetting: getParametersFiO2
})
const FiO2ValueModal = connect(settingSelector)(ValueModal)

const label = 'FiO2'
const stateKey = "fio2"
const units = PERCENT

/**
 * FiO2Info
 * 
 * A `Knob`-based component for handling FiO2 information.
 * 
 * TODO: Hook this component into the redux store with correct selectors.
 */
const FiO2Info = () => (
  <Knob
    valueDisplay={<FiO2Display label={label} units={units} isLive={true} />}
    valueModal={<FiO2ValueModal label={label} units={units} requestCommitSetting={boundDoSetFiO2} />}
    alarmModal={<AlarmModal label={label} units={units} stateKey={stateKey} requestCommitRange={() => null} />}
  />
)

export default FiO2Info