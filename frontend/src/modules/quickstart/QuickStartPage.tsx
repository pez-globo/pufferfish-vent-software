import React from 'react'
import { makeStyles, Theme } from '@material-ui/core/styles'
import {
    Grid,
    Typography,
    FormControl,
    RadioGroup,
    FormControlLabel,
    Radio
} from '@material-ui/core'
import TestTool from '../controllers/TestTool'
import ValueClicker from '../controllers/ValueController'
import ModeBanner from '../displays/ModeBanner'
import ToggleValue from '../displays/ToggleValue'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        justifyContent: 'space-between',
        height: '100%',
        flexWrap: 'nowrap',
        // border: '1px solid red',
    },
    topPanel: {
        flexWrap: 'nowrap',
        borderRadius: theme.panel.borderRadius,
        marginBottom: theme.spacing(2),
        justifyContent: 'space-between',
        alignItems: 'stretch',
        backgroundColor: theme.palette.background.paper
    },
    standby: {
        padding: theme.spacing(2),
        borderRight: '2px dashed ' + theme.palette.background.default
    },
    padding: {
        padding: theme.spacing(2),
        // border: '1px solid red'
    },
    paddingTop: {
        paddingTop: theme.spacing(5)
    },
    middleContainer: {
        flexWrap: 'nowrap',
        width: '100%',
        marginBottom: theme.spacing(2),
        // border: '1px solid teal',
    },
    middleLeftPanel: {
        borderRadius: theme.panel.borderRadius,
        marginRight: theme.spacing(2),
        backgroundColor: theme.palette.background.paper
    },
    middleRightPanel: {
        borderRadius: theme.panel.borderRadius,
        backgroundColor: theme.palette.background.paper
    },
    bottomBorder: {
        borderBottom: '2px dashed ' + theme.palette.background.default
    },
    rightBorder: {
        borderRight: '2px dashed ' + theme.palette.background.default
    }
}))

enum PatientSex { MALE, FEMALE }
enum PatientAge { ADULT, PEDIATRIC }

class QuickStartPage extends React.Component {

    render() {
        const classes = useStyles()
        const [patientSex, setPatientSex] = React.useState(PatientSex.MALE)
        const [patientAge, setPatientAge] = React.useState(PatientAge.ADULT)
        const [patientHeight, setPatientHeight] = React.useState(62)
        const [patientCircuitTestDate] = React.useState(new Date())
        const [preUseCheckDate] = React.useState(new Date())
        const [PEEP, setPEEP] = React.useState(5)
        const [RR, setRR] = React.useState(18)
        const [TV, setTV] = React.useState(500)

        const [FiO2, setFiO2] = React.useState(100)
        return (
            <Grid container direction='column' className={classes.root}>
                <Grid container item className={classes.topPanel}>
                    <Grid item xs className={classes.standby}>
                        <Typography variant='h3'>Standby</Typography>
                        <Typography variant='body1'>Patient Not Ventilated</Typography>
                    </Grid>
                    <Grid item xs className={classes.padding}>
                        <Typography variant='h6'>Age:</Typography>
                        <Grid item xs className={classes.paddingTop}>
                            <ToggleValue toggleBetween={[{ label: "Adult", value: PatientAge.ADULT }, { label: "Pediatric", value: PatientAge.PEDIATRIC }]} onToggle={(selected: number) => setPatientAge(selected as PatientAge)} selected={patientAge} />
                        </Grid>
                    </Grid>
                    <Grid item xs className={classes.padding}>
                        <Typography variant='h6'>Sex:</Typography>
                        <Grid item xs className={classes.paddingTop}>
                            <ToggleValue toggleBetween={[{ label: "Male", value: PatientSex.MALE }, { label: "Female", value: PatientSex.FEMALE }]} onToggle={(selected: number) => setPatientSex(selected as PatientSex)} selected={patientSex} />
                        </Grid>
                    </Grid>
                    <Grid item xs={3}>
                        <ValueClicker
                            label='Height'
                            units='i'
                            onClick={setPatientHeight}
                            value={patientHeight}
                            min={6}
                            max={120}
                        />
                    </Grid>
                </Grid>
                <Grid container item xs className={classes.middleContainer}>
                    <Grid container item xs direction='column' className={classes.middleLeftPanel}>
                        <Grid item xs className={classes.bottomBorder}>
                            <TestTool
                                label='Pre-Use Check'
                                isSuccess={true}
                                timestamp={preUseCheckDate}
                            />
                        </Grid>
                        <Grid item xs>
                            <TestTool
                                label='Patient Circuit Test'
                                isSuccess={true}
                                timestamp={patientCircuitTestDate}
                            />
                        </Grid>
                    </Grid>
                    <Grid container item xs={8} direction='column' className={classes.middleRightPanel}>
                        <Grid container item xs direction='row' className={classes.bottomBorder}>
                            <Grid item xs className={classes.rightBorder}>
                                <ValueClicker
                                    label='PEEP'
                                    units='cm H2O'
                                    value={PEEP}
                                    onClick={setPEEP}
                                />
                            </Grid>
                            <Grid item xs>
                                <ValueClicker
                                    label='RR'
                                    units='cm H2O'
                                    value={RR}
                                    onClick={setRR}
                                />
                            </Grid>
                        </Grid>
                        <Grid container item xs direction='row'>
                            <Grid item xs className={classes.rightBorder}>
                                <ValueClicker
                                    label='FiO2'
                                    units='%'
                                    value={FiO2}
                                    onClick={setFiO2}
                                />
                            </Grid>
                            <Grid item xs>
                                <ValueClicker
                                    label='TV'
                                    units='mL'
                                    value={TV}
                                    onClick={setTV}
                                />
                            </Grid>
                        </Grid>
                    </Grid>
                </Grid>
                <Grid item>
                    <ModeBanner bannerType='normal'/>
                </Grid>
            </Grid >
        )
    }
}

export default QuickStartPage