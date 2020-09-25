import React, { PropsWithChildren, ReactNode } from 'react';
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
  Typography,
  IconButton,
} from '@material-ui/core';
import CloseIcon from '@material-ui/icons/Close';

const useStyles = makeStyles((theme: Theme) => ({
  closeButton: {
    color: theme.palette.text.primary,
    border: `1px solid ${theme.palette.text.primary}`,
    borderRadius: 6,
  },
  dialogActions: {
    paddingBottom: theme.spacing(2),
  },
}));

interface Props {
  open: boolean;
  label: string | ReactNode;
  withAction?: boolean;
  showCloseIcon?: boolean;
  onClose?(): void;
  onConfirm?(): void;
}

interface ActionProps {
  onClose?(): void;
  onConfirm?(): void;
}

const ModalAction = ({ onClose, onConfirm }: ActionProps): JSX.Element => {
  return (
    <Grid container justify="center">
      <Grid container item xs justify="center">
        <Button onClick={onClose} variant="contained" color="primary" style={{ width: '90%' }}>
          Cancel
        </Button>
      </Grid>
      <Grid container item xs justify="center">
        <Button onClick={onConfirm} variant="contained" color="secondary" style={{ width: '90%' }}>
          Confirm
        </Button>
      </Grid>
    </Grid>
  );
};

export const ModalPopup = (props: PropsWithChildren<Props>): JSX.Element => {
  const classes = useStyles();
  const { open, onClose, label, showCloseIcon, children, withAction, onConfirm } = props;
  const [fullWidth] = React.useState(true);
  const [maxWidth] = React.useState<DialogProps['maxWidth']>('sm');
  return (
    <Dialog fullWidth={fullWidth} maxWidth={maxWidth} open={open} onClose={onClose}>
      <DialogTitle id="form-dialog-title">
        <Grid container alignItems="center">
          <Grid item xs>
            <Typography variant="h4">{label}</Typography>
          </Grid>
          <Grid item>
            {showCloseIcon && (
              <IconButton aria-label="close" className={classes.closeButton} onClick={onClose}>
                <CloseIcon />
              </IconButton>
            )}
          </Grid>
        </Grid>
      </DialogTitle>
      <DialogContent>{children}</DialogContent>
      <DialogActions className={classes.dialogActions}>
        {withAction && <ModalAction onClose={onClose} onConfirm={onConfirm} />}
      </DialogActions>
    </Dialog>
  );
};

export default ModalPopup;
