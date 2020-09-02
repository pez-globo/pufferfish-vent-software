import React from 'react'
import { Grid, Button, makeStyles, Theme } from '@material-ui/core'
import KeyboardArrowUp from '@material-ui/icons/KeyboardArrowUp'
import KeyboardArrowDown from '@material-ui/icons/KeyboardArrowDown'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        justifyContent: 'center',
        alignItems: 'center',
        height: '100%',
        width: '100%',
        // border: '1px dashed red',
    },
    iconButton: {
        border: '1px solid ' + theme.palette.common.black,
        borderRadius: 8,
        height: '100%',
        minHeight: 55,
        minWidth: 78,
        marginLeft: theme.spacing(1)
    },
    marginBottom: { marginBottom: theme.spacing(2) },
    marginRight: { marginRight: theme.spacing(1) }
}))

type Direction = "column" | "column-reverse" | "row" | "row-reverse" | undefined

interface Props {
    value: number,
    onClick: (value: number) => void,
    // An inclusive range defined by [min, max]
    min?: number,
    max?: number,
    direction?: Direction,
    step?: number
}

/**
 * ValueClicker
 *
 * A re-usable component for simple increment and decrement value adjustments.
 */
export const ValueClicker = ({ value, min = 0, max = 100, onClick, direction = 'column', step = 1 }: Props) => {
    const classes = useStyles()

    const update = (step: number) => () => {
        let change = value + step
        if (change > max) {
            change = min
        } else if (change < min) {
            change = max
        }
        return onClick(change)
    }

    return (
        <Grid container direction={direction} className={classes.root} wrap='nowrap'>
            <Grid item className={direction === 'row' ? classes.marginRight : classes.marginBottom}>
                <Button onClick={update(step)} variant='contained' color='primary' className={classes.iconButton}>
                    <KeyboardArrowUp fontSize='large' />
                </Button>
            </Grid>
            <Grid item>
                <Button onClick={update(-step)} variant='contained' color='primary' className={classes.iconButton}>
                    <KeyboardArrowDown fontSize='large' />
                </Button>
            </Grid>
        </Grid>
    )
}

export default ValueClicker