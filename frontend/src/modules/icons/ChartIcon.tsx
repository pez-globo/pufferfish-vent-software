import React from 'react'
import SvgIcon, { SvgIconProps } from '@material-ui/core/SvgIcon'
import { makeStyles, Theme } from '@material-ui/core/styles'

const useStyles = makeStyles((theme: Theme) => ({
    path: {
        stroke: theme.palette.text.primary,
    },
}))


const ChartIcon = (props: SvgIconProps) => {
    const classes = useStyles()
    return (
        <SvgIcon {...props} width="28" height="28" viewBox="0 0 28 28">
            <g id="Group_997" data-name="Group 997" transform="translate(-152 -409)">
                <g id="Rectangle_1662" data-name="Rectangle 1662" transform="translate(152 409)" fill="none" className={classes.path} stroke-width="2">
                    <rect width="28" height="28" rx="4" stroke="none" />
                    <rect x="1" y="1" width="26" height="26" rx="3" className={classes.path} />
                </g>
                <line id="Line_74" data-name="Line 74" y1="12" transform="translate(159.5 419.5)" fill="none" className={classes.path} stroke-width="3" />
                <line id="Line_75" data-name="Line 75" y1="17" transform="translate(166.5 414.5)" fill="none" className={classes.path} stroke-width="3" />
                <line id="Line_76" data-name="Line 76" y1="7" transform="translate(173.5 424.5)" fill="none" className={classes.path} stroke-width="3" />
            </g>
        </SvgIcon >
    )
}

export default ChartIcon