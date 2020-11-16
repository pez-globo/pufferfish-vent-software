import React, { useCallback, useEffect } from 'react';
import { makeStyles, Theme, Grid, Button, Typography } from '@material-ui/core';
import { useDispatch, useSelector } from 'react-redux';
import ValueSlider from './ValueSlider';
import ValueClicker from './ValueClicker';
import { getAlarmLimitsRequest } from '../../store/controller/selectors';
import { ALARM_LIMITS, ALARM_LIMITS_STANDBY } from '../../store/controller/types';
import { updateCommittedState } from '../../store/controller/actions';
import ModalPopup from './ModalPopup';

const useStyles = makeStyles((theme: Theme) => ({
  contentContainer: {
    width: '100%',
    height: '100%',
    minHeight: '300px',
    border: `2px dashed ${theme.palette.background.default}`,
    borderRadius: theme.panel.borderRadius,
  },
  borderBottom: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
  },
  openButton: {
    width: '100%',
    border: `1px solid ${theme.palette.common.black}`,
    padding: '5px 16px',
  },
  alarmContainer: {
    // border: '1px solid red',
  },
  alarmValue: {
    marginRight: theme.spacing(3),
    border: `1px solid ${theme.palette.text.primary}`,
    minWidth: 100,
    paddingLeft: theme.spacing(1),
    paddingRight: theme.spacing(1),
    borderRadius: 8,
  },
}));

export interface AlarmAdjustProps {
  committedMin: number;
  committedMax: number;
}

interface Props {
  label: string;
  units: string;
  committedMin?: number;
  committedMax?: number;
  disableAlarmButton?: boolean;
  updateModalStatus?(status: boolean): void;
  requestCommitRange(min: number, max: number): void;
  stateKey: string;
  step?: number;
  openModal?: boolean;
}

export const AlarmModal = ({
  label,
  units,
  committedMin = 0,
  committedMax = 100,
  disableAlarmButton = false,
  updateModalStatus,
  requestCommitRange,
  openModal = false,
  stateKey,
  step,
}: Props): JSX.Element => {
  const classes = useStyles();
  const [open, setOpen] = React.useState(false);
  const [min] = React.useState(committedMin);
  const [max] = React.useState(committedMax);
  const alarmLimits: Record<string, number> = useSelector(getAlarmLimitsRequest) as Record<
    string,
    number
  >;
  const [rangeValue, setRangeValue] = React.useState<number[]>([
    alarmLimits[`${stateKey}Min`],
    alarmLimits[`${stateKey}Max`],
  ]);
  const dispatch = useDispatch();

  const initSetValue = useCallback(() => {
    setOpen(openModal);
  }, [openModal]);

  useEffect(() => {
    initSetValue();
  }, [initSetValue]);

  useEffect(() => {
    if (updateModalStatus) {
      updateModalStatus(open);
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [open]);

  const handleOpen = () => {
    setOpen(true);
  };

  const handleClose = () => {
    setOpen(false);
  };

  const handleConfirm = () => {
    dispatch(
      updateCommittedState(ALARM_LIMITS, {
        [`${stateKey}Min`]: rangeValue[0],
        [`${stateKey}Max`]: rangeValue[1],
      }),
    );
    dispatch(
      updateCommittedState(ALARM_LIMITS_STANDBY, {
        [`${stateKey}Min`]: rangeValue[0],
        [`${stateKey}Max`]: rangeValue[1],
      }),
    );
    requestCommitRange(rangeValue[0], rangeValue[1]);
    handleClose();
  };

  const setSortedRangeValue = (rangeValue: number[]) => {
    setRangeValue(rangeValue.sort());
  };

  return (
    <Grid container direction="column" alignItems="center" justify="center">
      <Grid container item xs>
        {!disableAlarmButton && (
          <Button
            onClick={handleOpen}
            color="primary"
            variant="contained"
            className={classes.openButton}
          >
            Alarm
          </Button>
        )}
      </Grid>
      <ModalPopup
        withAction={true}
        label={`${label} - Alarm`}
        open={open}
        onClose={handleClose}
        onConfirm={handleConfirm}
      >
        <Grid
          container
          direction="column"
          alignItems="stretch"
          className={classes.contentContainer}
        >
          <Grid
            container
            item
            xs
            justify="center"
            alignItems="center"
            className={`${classes.alarmContainer} ${classes.borderBottom}`}
          >
            <Grid item className={classes.alarmValue}>
              <Typography align="center" variant="h3">
                {rangeValue[0] !== undefined ? Number(rangeValue[0]) : '--'}
              </Typography>
            </Grid>
            <Grid item>
              <ValueClicker
                value={rangeValue[0]}
                step={step}
                min={committedMin}
                max={committedMax}
                onClick={(value) => setRangeValue(Object.assign([], rangeValue, { 0: value }))}
                direction="row"
              />
            </Grid>
          </Grid>
          <Grid
            container
            item
            xs
            justify="center"
            alignItems="center"
            className={`${classes.alarmContainer} ${classes.borderBottom}`}
          >
            <Grid item className={classes.alarmValue}>
              <Typography align="center" variant="h3">
                {rangeValue[1] !== undefined ? Number(rangeValue[1]) : '--'}
              </Typography>
            </Grid>
            <Grid item>
              <ValueClicker
                value={rangeValue[1]}
                step={step}
                min={committedMin}
                max={committedMax}
                onClick={(value) => setRangeValue(Object.assign([], rangeValue, { 1: value }))}
                direction="row"
              />
            </Grid>
          </Grid>
          <Grid container item xs alignItems="center">
            <ValueSlider
              rangeValues={rangeValue}
              onChange={setRangeValue}
              min={min}
              max={max}
              step={step}
            />
          </Grid>
        </Grid>
      </ModalPopup>
    </Grid>
  );
};

export default AlarmModal;
