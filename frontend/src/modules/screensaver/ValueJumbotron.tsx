import React from 'react'
import { Grid, makeStyles, Theme, Typography } from '@material-ui/core'
import TrendIcon from '../icons/TrendUpIcon'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        height: '100%',
        width: '100%',
        // border: '1px solid red'
    },
    displayContainer: {
        padding: 0,
        alignItems: 'flex-start'
        // border: '1px solid orange',
    },
    valueContainer: {
        flexGrow: 1,
        // border: '1px solid yellow',
    },
    valueLabel: {
        fontSize: '7.5rem',
        // border: '1px solid red',
    },
    unitsLabel: {
        paddingLeft: theme.spacing(0),
        paddingTop: theme.spacing(4),
        color: theme.palette.grey[400],
        // border: '1px solid red'
    },
    labelItems: {
        alignItems: 'flex-start',
        // border: '1px solid red'
    },
}))

export interface Props {
    value: number,
    label: string,
    units?: string
}

/**
 * ValueJumbotron
 *
 * A component for displaying values in a large way.
 */
export const ValueJumbotron = ({ value, label, units = '' }: Props) => {
    const classes = useStyles()

    return (
        <Grid container direction='column' className={classes.root}>
            <Grid item style={{ width: '100%' }} className={classes.labelItems}>
                <Typography variant='h3' style={{ fontWeight: 'bold' }}>
                    {label}
                </Typography>
            </Grid>
            <Grid container item xs alignItems='center' className={classes.displayContainer} wrap='nowrap'>
                <Typography align='left' variant='h1' className={classes.valueLabel}>
                    {value !== undefined ? value.toFixed(0) : '--'}
                </Typography>
            </Grid>
            <Grid item className={classes.unitsLabel}>
                <TrendIcon style={{ fontSize: '2rem' }} />
            </Grid>
            <Grid item>
                <Typography align='left' variant='h4' className={classes.unitsLabel}>
                    {units}
                </Typography>
            </Grid>
        </Grid>
    )
}

export default ValueJumbotron