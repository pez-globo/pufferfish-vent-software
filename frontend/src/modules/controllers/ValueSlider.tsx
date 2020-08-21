import React from 'react'
import { Grid, Typography, Slider, withStyles, makeStyles } from '@material-ui/core'

const useStyles = makeStyles({
    root: {
        width: '100%',
        // border: '1px solid red'
    },
})

const boxShadow =
    '0 3px 1px rgba(0,0,0,0.1),0 4px 8px rgba(0,0,0,0.13),0 0 0 1px rgba(0,0,0,0.02)'

const StyledSlider = withStyles({
    root: {
        color: '#3880ff',
        height: 2,
        padding: '15px 0',
    },
    thumb: {
        height: 28,
        width: 28,
        backgroundColor: '#fff',
        boxShadow: boxShadow,
        marginTop: -14,
        marginLeft: -14,
        '&:focus, &:hover, &$active': {
            boxShadow: '0 3px 1px rgba(0,0,0,0.1),0 4px 8px rgba(0,0,0,0.3),0 0 0 1px rgba(0,0,0,0.02)',
            // Reset on touch devices, it doesn't add specificity
            '@media (hover: none)': {
                boxShadow: boxShadow,
            },
        },
    },
    active: {},
    valueLabel: {
        left: 'calc(-50% + 12px)',
        top: -22,
        '& *': {
            background: 'transparent',
            // color: '#000',
        },
    },
    track: {
        height: 2,
    },
    rail: {
        height: 2,
        opacity: 0.5,
        backgroundColor: '#bfbfbf',
    },
    mark: {
        backgroundColor: '#bfbfbf',
        height: 8,
        width: 1,
        marginTop: -3,
    },
    markActive: {
        opacity: 1,
        backgroundColor: 'currentColor',
    },
})(Slider)

interface Props {
    min: number,
    max: number,
    label?: string,  // TODO: This prop should be removed.
    onChange?(values: number[]): any,
    rangeValues?: number[],
}

export const ValueSlider = ({ min, max, onChange, rangeValues }: Props) => {
    const classes = useStyles()
    const [value, setValue] = React.useState<number[]>([min, max])

    const handleChange = (event: any, newValue: number | number[]) => {
        setValue(newValue as number[])
        if (onChange) {
            onChange(newValue as number[])
        }
    }

    return (
        <Grid container direction='column' className={classes.root} >
            <Grid container item justify='center' alignItems='stretch'>
                <Grid container item xs justify='center' alignItems='center'>
                    <Typography align='center'>{min}</Typography>
                </Grid>
                <Grid container item xs={8} alignItems='center'>
                    <StyledSlider
                        valueLabelDisplay='on'
                        value={rangeValues || value}
                        onChange={handleChange}
                        defaultValue={60}
                    />
                </Grid>
                <Grid container item xs justify='center' alignItems='center'>
                    <Typography align='center'>{max}</Typography>
                </Grid>
            </Grid>
        </Grid>
    )
}

export default ValueSlider