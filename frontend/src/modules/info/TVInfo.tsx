import React from 'react'
import { connect } from 'react-redux'
import { createStructuredSelector } from 'reselect'
import {
  ValueDisplay, ValueProps
} from '../displays/ValueDisplay'
import { AlarmModal, Knob } from '../controllers'
import { StoreState } from '../../store/types'
import {
  getCycleMeasurementsVT
} from '../../store/controller/selectors'
import { LMIN } from './units'

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
  value: getCycleMeasurementsVT
})
const TVDisplay = connect(displaySelector)(ValueDisplay)

const label = 'TV'
const stateKey = "tv"
const units = LMIN

/**
 * TVInfo 
 * 
 * A `Knob`-based component for handling TV information.
 * 
 * TODO: Hook this component into the redux store with correct selectors.
 */
const TVInfo = () => (
  <Knob
    valueDisplay={<TVDisplay label={label} units={units} isLive={true} />}
    valueModal={null}
    alarmModal={<AlarmModal label={label} units={units} stateKey={stateKey} requestCommitRange={() => null} />}
  />
)

export default TVInfo