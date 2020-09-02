import React from 'react'
import { makeStyles, Theme } from '@material-ui/core/styles'
import { Grid } from '@material-ui/core'
import ModeBanner from '../displays/ModeBanner'
import ValueJumbotron from './ValueJumbotron'
import { CMH20, PERCENT, BMIN } from '../info/units'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        // border: '1px solid yellow',
    },
    gridItems: {
        border: '1px solid white',
        paddingLeft: theme.spacing(2),
        paddingRight: theme.spacing(2),
        paddingTop: theme.spacing(1),
        paddingBottom: theme.spacing(2)
    },
}))

/**
 * ScreensaverPage
 *
 * A farview screensaver page.
 */
export const ScreensaverPage = () => {
    const classes = useStyles()

    return (
        <Grid container direction='column' justify='space-between'>
            <Grid container xs justify='space-evenly' alignItems='stretch' className={classes.root} wrap="nowrap">
                <Grid item xs={3} className={classes.gridItems}>
                    <ValueJumbotron value={0} label='PIP' units={CMH20} />
                </Grid>
                <Grid item xs={3} className={classes.gridItems}>
                    <ValueJumbotron value={0} label='PEEP' units={CMH20} />
                </Grid>
                <Grid item xs={3} className={classes.gridItems}>
                    <ValueJumbotron value={0} label='RR' units={BMIN} />
                </Grid>
                <Grid item xs={3} className={classes.gridItems}>
                    <ValueJumbotron value={0} label='FiO2' units={PERCENT} />
                </Grid>
            </Grid>
            <Grid item>
                <ModeBanner bannerType='screenSaver'/>
            </Grid>
        </Grid>
    )
}

export default ScreensaverPage