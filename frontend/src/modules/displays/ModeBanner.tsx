import React from 'react'
import { Grid, Typography, makeStyles, Theme } from '@material-ui/core'
import { getParametersRequestMode } from '../../store/controller/selectors';
import { VentilationMode } from '../../store/controller/proto/mcu_pb';
import { useSelector } from 'react-redux';

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        height: 80,
        maxHeight: 80        
    },
    normal: {
        color: theme.palette.background.paper,
        fontSize: 80,
    },
    screenSaver: {
        color: theme.palette.primary.contrastText,
        fontSize: 80,
    }
}))

export const getModeText = (mode: VentilationMode) => {
    switch (mode) {
        case VentilationMode.pc_ac:
            return 'Pressure Control (AC)'
        case VentilationMode.pc_simv:
            return 'Pressure Control (SIMV)'
        case VentilationMode.vc_ac:
            return 'Volume Control (AC)'
        case VentilationMode.vc_simv:
            return 'Volume Control (SIMV)'
        case VentilationMode.hfnc:
            return 'Non Invasive (NIV)'
        default:
            return 'No Mode Selected';
    }
}

export interface Props {
    bannerType: string,
}

/**
 * ModeBanner
 * 
 * A contianer for displaying the current mode in a large font banner. 
 * 
 * TODO: Hook up the `mode` prop to the redux store so we can display
 *       the current mode below.
 */
export const ModeBanner = ({bannerType}:Props) => {
    const classes = useStyles()
    const mode = useSelector(getParametersRequestMode)
    function color(bannerType:string) {
        if(bannerType==="screenSaver") return classes.screenSaver
        return classes.normal
    }
    return (
        <Grid container justify='center' alignItems='center' className={classes.root}>
            <Grid item>
                <Typography align='center' variant='h2' className={color(bannerType)}>
                    {getModeText(mode)}
                </Typography>
            </Grid>
        </Grid>
    )
}

export default ModeBanner