import React from 'react'
import ConfirmationModal from './ConfirmationModal'
import { makeStyles, Theme, Grid, Button, Typography } from '@material-ui/core'
import ValueClicker from './ValueClicker'

const useStyles = makeStyles((theme: Theme) => ({
    contentContainer: {
        height: '100%',
        minHeight: '300px',
        border: '2px dashed ' + theme.palette.background.default,
        borderRadius: theme.panel.borderRadius,
        marginRight: theme.spacing(2),
        paddingTop: theme.spacing(2),
        paddingLeft: theme.spacing(2),
        paddingRight: theme.spacing(4),
    },
    borderBottom: {
        borderBottom: '2px dashed ' + theme.palette.background.default
    },
    openButton: {
        width: '100%',
        border: '1px solid ' + theme.palette.common.black
    },
    unitsLabel: {
        paddingTop: theme.spacing(4),
        paddingLeft: theme.spacing(2),
        opacity: .8,
        // border: '1px solid red'
    },
}))

export interface SettingAdjustProps {
    committedSetting: number
}

interface Props {
    label: string,
    units: string,
    committedSetting: number,
    requestCommitSetting(setting: number): any,
}

export const ValueModal = (
    { label, units, committedSetting, requestCommitSetting }: Props
) => {
    const classes = useStyles()
    const [open, setOpen] = React.useState(false)
    const [value, setValue] = React.useState(committedSetting)

    const handleOpen = () => {
        setOpen(true)
    }

    const handleClose = () => {
        setOpen(false)
    }

    const handleConfirm = () => {
        requestCommitSetting(value)
    }

    function pipClarify (label:string) {
        if (label==='PIP') return '*not PEEP compensated'
        return ''
    }

    return (
        <Grid container direction='column' alignItems='center' justify='center'>
            <Grid container item xs>
                <Button onClick={handleOpen} color="primary" variant='contained' className={classes.openButton}>
                    Set New
                </Button>
            </Grid>
            <ConfirmationModal label='Set New' open={open} onClose={handleClose} onConfirm={handleConfirm}>
                <Grid container direction='row'>
                    <Grid container item xs direction='column' className={classes.contentContainer}>
                        <Grid item>
                            <Typography variant='h4'>
                                {label}
                                <Typography variant='h6' style={{opacity:.8}}>
                                {pipClarify(label)}
                                </Typography>
                            </Typography>
                        </Grid>
                        <Grid container item xs wrap='nowrap'>
                            <Grid container item alignItems='baseline'>
                                <Typography align='left' style={{ fontSize: '9.5rem' }}>
                                    {value.toFixed(0)}
                                </Typography>
                                <Typography align='center' variant='h5' className={classes.unitsLabel}>
                                    {units}
                                </Typography>
                            </Grid>
                        </Grid>
                    </Grid>
                    <Grid item>
                        <ValueClicker
                            value={value}
                            min={0} max={100}
                            onClick={setValue}
                        />
                    </Grid>
                </Grid>
            </ConfirmationModal>
        </Grid >
    )
}

export default ValueModal