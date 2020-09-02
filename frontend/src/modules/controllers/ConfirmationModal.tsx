import React, { PropsWithChildren } from 'react'
import {
    Button,
    Dialog,
    DialogTitle,
    DialogContent,
    DialogActions,
    DialogProps,
    Grid,
    makeStyles,
    Theme,
    Typography
} from '@material-ui/core'

const useStyles = makeStyles((theme: Theme) => ({
    closeButton: {
        color: theme.palette.text.primary,
        border: '1px solid ' + theme.palette.text.primary,
        borderRadius: 6
    },
    dialogActions: {
        paddingBottom: theme.spacing(2)
    },
    buttons: {
        minHeight: 54,
        borderRadius: 6,
        width: '90%', 
        fontSize: 20
    }
}))

interface Props {
    open: boolean,
    label: string,
    onClose(): any,
    onConfirm(): any,
}

export const ConfirmationModal = (props: PropsWithChildren<Props>) => {
    const classes = useStyles()
    const [fullWidth] = React.useState(true)
    const [maxWidth] = React.useState<DialogProps['maxWidth']>('sm')

    return (
        <Dialog fullWidth={fullWidth} maxWidth={maxWidth} open={props.open} onClose={props.onClose}>
            <DialogTitle id='form-dialog-title'>
                <Grid container alignItems='center'>
                    <Grid item xs>
                        <Typography variant='h4'>
                            {props.label}
                        </Typography>
                    </Grid>
                    {/* <Grid item>
                        <IconButton aria-label="close" className={classes.closeButton} onClick={props.onClose}>
                            <CloseIcon />
                        </IconButton>
                    </Grid> */}
                </Grid>
            </DialogTitle>
            <DialogContent>
                {props.children}
            </DialogContent>
            <DialogActions className={classes.dialogActions}>
                <Grid container justify='center'>
                    <Grid container item xs justify='center'>
                        <Button onClick={props.onClose} variant='contained' color='primary' className={classes.buttons}>
                            Cancel
                        </Button>
                    </Grid>
                    <Grid container item xs justify='center'>
                        <Button onClick={props.onConfirm} variant='contained' color='secondary' className={classes.buttons}>
                            Confirm
                        </Button>
                    </Grid>
                </Grid>
            </DialogActions>
        </Dialog>
    )
}

export default ConfirmationModal