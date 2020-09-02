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
const MVeDisplay = connect(displaySelector)(ValueDisplay)

const label = 'MVe'
const stateKey = "mve"
const units = LMIN

/**
 * MVe
 * 
 * A `Knob`-based component for handling IP Above PEEP information.
 * 
 * TODO: Hook this component into the redux store with correct selectors.
 */
const MVeInfo = () => (
    <Knob
        valueDisplay={<MVeDisplay label={label} units={units} isLive={true} />}
        valueModal={null}
        alarmModal={<AlarmModal label={label} units={units} stateKey={stateKey} requestCommitRange={() => null} />}
    />
)

export default MVeInfo