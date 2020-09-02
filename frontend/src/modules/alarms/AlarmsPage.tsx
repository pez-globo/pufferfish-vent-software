import React, { useState } from 'react'
import { makeStyles, Theme } from '@material-ui/core/styles'
import { Typography, Button, Grid } from '@material-ui/core'
import Pagination from '@material-ui/lab/Pagination'
import ValueSlider from '../controllers/ValueSlider'
import ModeBanner from '../displays/ModeBanner'
import { useDispatch, useSelector } from 'react-redux'
import { getAlarmLimitsRequest } from '../../store/controller/selectors'
import { ALARM_LIMITS } from '../../store/controller/types'
import { updateCommittedState } from '../../store/controller/actions'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        justifyContent: 'space-between',
        alignItems: 'stretch',
        flexWrap: 'nowrap'
    },
    panel: {
        borderRadius: theme.panel.borderRadius,
        flexWrap: 'nowrap',
        marginBottom: theme.spacing(2),
        backgroundColor: theme.palette.background.paper
    },
    leftContainer: {
        justifyContent: 'space-between',
        borderRight: '2px dashed ' + theme.palette.background.default,
        padding: theme.spacing(2),
    },
    rightContainer: {
        justifyContent: 'space-between',
        padding: theme.spacing(2)
    },
    paginationContainer: {
        alignItems: 'center',
        width: '100%',
        // border: '1px solid blue'
    },
    applyButton: {
        width: '100%',
        border: '1px solid black',
    },
}))

// NOTE: Temporary Alarm until the UI team address interface concerns.
interface AlarmProps {
    label: string,
    min: number,
    max: number,
    stateKey: string,
    step?: number,
    alarmLimits: any,
    setAlarmLimits(alarmLimits: Object): any
}

interface AlarmConfigurationProps {
    page: number,
    alarmLimits: any,
    setAlarmLimits(alarmLimits: Object): any
}

const Alarm = ({ label, min, max, stateKey, step, alarmLimits, setAlarmLimits }: AlarmProps) => {
    const rangeValues = [ alarmLimits[`${stateKey}Min`], alarmLimits[`${stateKey}Max`] ]
    const setRangevalue = (range: number[]) => {
        setAlarmLimits({ [`${stateKey}Min`]: range[0], [`${stateKey}Max`]: range[1] })
    }
    return (
        <Grid container>
            <Grid item xs={12} style={{paddingBottom: 20}}>
                <Typography variant='h5'>{label}</Typography>
            </Grid>
            <Grid item xs={12}>
                <ValueSlider min={min} max={max} step={step} onChange={setRangevalue} rangeValues={rangeValues}/>
            </Grid>
        </Grid>
    )
}

const AlarmConfiguration = ({ page, alarmLimits, setAlarmLimits }: AlarmConfigurationProps) => {
    const classes = useStyles()
    /**
     * TODO: Fill in the `ValueSlider` placeholders with sliders that actually
     *       change the alarm states in the redux store.
     */
    const page1 =
    <Grid container item xs direction='column' className={classes.rightContainer}>
        <Alarm label='RR' min={0} max={100} stateKey="rr" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
        <Alarm label='TV' min={0} max={100} stateKey="tv" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
        <Alarm label='Flow' min={0} max={100} stateKey="flow" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
        <Alarm label='MVe' min={0} max={100} stateKey="mve" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
        <Alarm label='Apnea' min={0} max={100} stateKey="apnea" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
    </Grid>

    const page2 =
        <Grid container item xs direction='column' className={classes.rightContainer}>
            <Alarm label='Pressure above PEEP' min={0} max={100} stateKey="ipAbovePeep" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
            <Alarm label='PAW' min={0} max={100} stateKey="paw" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
            <Alarm label='PiP' min={0} max={100} stateKey="pip" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
            <Alarm label='PEEP' min={0} max={100} stateKey="peep" alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
            <Alarm label='Insp. Time' min={0} max={100} stateKey="inspTime" step={0.5} alarmLimits={alarmLimits} setAlarmLimits={setAlarmLimits} />
        </Grid>

    return (
        <React.Fragment>
            {page === 1 ? page1 : page2}
        </React.Fragment>
    )
}

/**
 * AlarmsPage
 *
 * A container for housing all alarm configurations.
 */
export const AlarmsPage = () => {
    const classes = useStyles()
    const [page, setPage] = React.useState(1);

    const handleChange = (event: React.ChangeEvent<unknown>, value: number) => {
        setPage(value)
    }

    const alarmLimitsRequest: any = useSelector(getAlarmLimitsRequest)
    const dispatch = useDispatch()
    const [alarmLimits, setAlarmLimits] = useState(alarmLimitsRequest)
    const updateAlarmLimits = (data: Object) => setAlarmLimits({...alarmLimits, ...data})
    const applyChanges = () => dispatch(updateCommittedState(ALARM_LIMITS, alarmLimits))

    return (
        <Grid container direction='column' className={classes.root}>
            <Grid container item xs direction='row' className={classes.panel}>
                <Grid container item xs={3} direction='column' className={classes.leftContainer}>
                    <Grid item>
                        <Typography variant='h3'>Alarms</Typography>
                    </Grid>
                    <Grid container direction='column' className={classes.paginationContainer}>
                        <Grid item xs style={{ marginBottom: 10 }}>
                            <Pagination count={2} page={page} onChange={handleChange} variant='outlined' shape="rounded" size="large" />
                        </Grid>
                        <Grid item style={{ width: '100%' }}>
                            <Button onClick={applyChanges} color='secondary' variant='contained' className={classes.applyButton}>
                                Apply Changes
                            </Button>
                        </Grid>
                    </Grid>
                </Grid>
                {/* Right Container for Storing Alarm Slides */}
                <AlarmConfiguration setAlarmLimits={updateAlarmLimits} alarmLimits={alarmLimits} page={page}/>
            </Grid>
            {/* Title */}
            <Grid item>
                <ModeBanner bannerType='normal'/>
            </Grid>
        </Grid>
    )
}

export default AlarmsPage