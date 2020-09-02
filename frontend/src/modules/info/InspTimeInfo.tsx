import React from 'react'
import { connect } from 'react-redux'
import { createStructuredSelector } from 'reselect'
import {
    ValueDisplay, ValueProps
} from '../displays/ValueDisplay'
import { ValueModal, AlarmModal, Knob } from '../controllers'
import { StoreState } from '../../store/types'
import {
    getCycleMeasurementsVT
} from '../../store/controller/selectors'
import { SECOND } from './units'

const displaySelector = createStructuredSelector<StoreState, ValueProps>({
    value: getCycleMeasurementsVT
})
const InspTimeDisplay = connect(displaySelector)(ValueDisplay)

const label = 'Insp. Time'
const stateKey = "inspTime"
const sliderStepValue = 0.5
const units = SECOND

/**
 * InspTimeInfo
 * 
 * A `Knob`-based component for handling Inspiration Time information.
 * 
 * TODO: Hook this component into the redux store with correct selectors.
 */
const InspTimeInfo = () => (
    <Knob
        valueDisplay={<InspTimeDisplay label={label} units={units} isLive={true} />}
        valueModal={<ValueModal label={label} units={units} committedSetting={1.7} requestCommitSetting={() => null} />}
        alarmModal={<AlarmModal label={label} units={units} stateKey={stateKey} step={sliderStepValue} requestCommitRange={() => null} />}
    />
)

export default InspTimeInfo