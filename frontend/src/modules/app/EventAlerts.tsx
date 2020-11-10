import React, { useState, useEffect, useRef, RefObject } from 'react';
import { Link } from 'react-router-dom';
import { Button, Snackbar, makeStyles, Theme, Grid, Typography, Popover } from '@material-ui/core';
import { Alert } from '@material-ui/lab';
import { useSelector } from 'react-redux';
import { BellIcon } from '../icons';
import { LogEventCode } from '../../store/controller/proto/mcu_pb';
import { BMIN, PERCENT } from '../info/units';
import { getActiveLogEventIds, getPopupEventLog } from '../../store/controller/selectors';

export const ALARM_EVENT_PATIENT = 'Patient';
export const ALARM_EVENT_SYSTEM = 'System';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100%',
  },
  alertStyles: {
    '& .MuiAlert-message': {
      width: '100%',
    },
    '& .MuiAlert-action': {
      position: 'absolute',
      right: '15px',
    },
  },
  controlPanel: {
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  marginRight: {
    marginRight: theme.spacing(1),
  },
  marginTop: {
    marginTop: theme.spacing(1),
    '& .MuiPopover-paper': {
      width: '300px',
      borderRadius: '10px',
    },
  },
  iconBadge: {
    position: 'absolute',
    top: -6,
    right: -6,
    lineHeight: '16px',
    padding: '0 6px 0px 5.4px',
    borderRadius: 15,
    alignItems: 'center',
    justifyContent: 'center',
    fontSize: '11px',
    backgroundColor: '#FF0000',
  },
  openButton: {
    width: '100%',
    // border: `1px solid ${theme.palette.common.black}`,
    backgroundColor: '#e0e0e052',
    color: 'white',
    boxShadow: 'none',
    fontSize: '12px',

    '&:hover': {
      backgroundColor: '#e0e0e052',
      // boxShadow: 'none',
    },
  },
}));

export const getEventType = (code: LogEventCode): { type: string; label: string; unit: string } => {
  switch (code) {
    case LogEventCode.fio2_too_low:
      return {
        type: ALARM_EVENT_PATIENT,
        label: 'fiO2 is too low',
        unit: PERCENT,
      };
    case LogEventCode.fio2_too_high:
      return {
        type: ALARM_EVENT_PATIENT,
        label: 'fiO2 is too high',
        unit: PERCENT,
      };
    case LogEventCode.rr_too_low:
      return {
        type: ALARM_EVENT_PATIENT,
        label: 'Respiratory Rate is too low',
        unit: BMIN,
      };
    case LogEventCode.rr_too_high:
      return {
        type: ALARM_EVENT_PATIENT,
        label: 'Respiratory Rate is too high',
        unit: BMIN,
      };
    case LogEventCode.spo2_too_low:
      return {
        type: ALARM_EVENT_PATIENT,
        label: 'spO2 is too low',
        unit: PERCENT,
      };
    case LogEventCode.spo2_too_high:
      return {
        type: ALARM_EVENT_PATIENT,
        label: 'spO2 is too high',
        unit: PERCENT,
      };
    case LogEventCode.battery_low:
      return {
        type: ALARM_EVENT_SYSTEM,
        label: 'Battery power is low',
        unit: PERCENT,
      };
    case LogEventCode.screen_locked:
      return {
        type: ALARM_EVENT_SYSTEM,
        label: 'Battery power is low',
        unit: PERCENT,
      };
    default:
      return { type: '', label: '', unit: '' };
  }
};

interface Props {
  path: string;
  label: string;
}

export const AlertToast = ({
  onClose,
  label,
}: {
  onClose(close: React.SyntheticEvent): void;
  label: string;
}): JSX.Element => {
  const classes = useStyles();
  return (
    <Alert
      className={classes.alertStyles}
      icon={false}
      onClose={onClose}
      variant="filled"
      severity="error"
    >
      <Grid container direction="column" className={classes.root}>
        <Grid item xs style={{ width: '100%', paddingBottom: '15px' }}>
          <Typography variant="h5">{label}</Typography>
        </Grid>
        <Grid container item direction="row" className={classes.controlPanel} wrap="nowrap">
          <Grid item xs={6} className={classes.marginRight}>
            <Button variant="contained" className={classes.openButton}>
              Reset Alarm
            </Button>
          </Grid>
          <Grid item xs={6}>
            <Button variant="contained" className={classes.openButton}>
              Silence for 2 min
            </Button>
          </Grid>
        </Grid>
      </Grid>
    </Alert>
  );
};

export const EventAlerts = ({ path, label }: Props): JSX.Element => {
  const classes = useStyles();
  const [alert, setAlert] = useState({ label: '' });
  const [alertCount, setAlertCount] = useState<number>(0);
  const [open, setOpen] = React.useState(false);
  const buttonRef = useRef(null);
  const handleClose = (event?: React.SyntheticEvent, reason?: string) => {
    setOpen(false);
    setAlertCount(0);
  };

  const popupEventLog = useSelector(getPopupEventLog);
  useEffect(() => {
    if (popupEventLog) {
      const eventType = getEventType(popupEventLog.code);
      if (eventType.type) {
        setOpen(true);
        setAlertCount(1);
        setAlert({ label: eventType.label });
      }
    }
  }, [popupEventLog]);

  return (
    <div>
      <Button
        style={{ marginRight: 12 }}
        component={Link}
        to={path}
        variant="contained"
        color="primary"
        ref={buttonRef}
      >
        <BellIcon />
        <div hidden={!(alertCount > 0)} className={classes.iconBadge}>
          {alertCount}
        </div>
      </Button>
      {label}
      {buttonRef && (
        <Popover
          className={classes.marginTop}
          open={open}
          anchorEl={buttonRef.current}
          anchorOrigin={{
            vertical: 'bottom',
            horizontal: 'left',
          }}
          transformOrigin={{
            vertical: 'top',
            horizontal: 'left',
          }}
        >
          <AlertToast label={alert.label} onClose={handleClose} />
        </Popover>
      )}
    </div>
  );
};

export default EventAlerts;
