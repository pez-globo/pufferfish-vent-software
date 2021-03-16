import { Button, Grid, makeStyles, Theme, Typography, useTheme } from '@material-ui/core';
import React, { RefObject, useCallback, useEffect, useRef } from 'react';
import { shallowEqual, useDispatch, useSelector } from 'react-redux';
import { updateCommittedState } from '../../store/controller/actions';
import { getAlarmLimitsRequest } from '../../store/controller/selectors';
import { Range } from '../../store/controller/proto/mcu_pb';
import { ALARM_LIMITS, ALARM_LIMITS_STANDBY } from '../../store/controller/types';
import ModalPopup from './ModalPopup';
import ValueClicker from './ValueClicker';
import ValueSlider from './ValueSlider';
import useRotaryReference from '../utils/useRotaryReference';
import { setActiveRotaryReference } from '../app/Service';

const useStyles = makeStyles((theme: Theme) => ({
  contentContainer: {
    width: '100%',
    height: '100%',
    minHeight: '290px',
    border: `2px dashed ${theme.palette.background.default}`,
    borderRadius: theme.panel.borderRadius,
    margin: '10px 0px',
  },
  borderBottom: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
  },
  openButton: {
    width: '100%',
    border: `1px solid ${theme.palette.common.black}`,
    padding: '5px 16px',
  },
  alarmValue: {
    marginRight: theme.spacing(3),
    border: `1px solid ${theme.palette.text.primary}`,
    minWidth: 75,
    paddingLeft: theme.spacing(1),
    paddingRight: theme.spacing(1),
    borderRadius: 8,
  },
  head: {
    '& h3': {
      lineHeight: '1.5',
      padding: '0px 15px',
    },
  },
}));

export interface AlarmAdjustProps {
  committedMin: number;
  committedMax: number;
}

interface Props {
  label: string;
  units?: string;
  committedMin?: number;
  committedMax?: number;
  disableAlarmButton?: boolean;
  updateModalStatus?(status: boolean): void;
  onModalClose?(status: boolean): void;
  requestCommitRange(min: number, max: number): void;
  stateKey: string;
  step?: number;
  openModal?: boolean;
  contentOnly?: boolean;
  labelHeading?: boolean;
  alarmRangeValues?: number[];
}

export const AlarmModal = ({
  label,
  committedMin = 0,
  committedMax = 100,
  disableAlarmButton = false,
  updateModalStatus,
  requestCommitRange,
  onModalClose,
  openModal = false,
  units = '',
  stateKey,
  step,
  contentOnly = false,
  labelHeading = false,
  alarmRangeValues = [],
}: Props): JSX.Element => {
  const classes = useStyles();
  const dispatch = useDispatch();
  const theme = useTheme();
  const { initRefListener } = useRotaryReference(theme);
  const [open, setOpen] = React.useState(false);
  const [min] = React.useState(committedMin);
  const [max] = React.useState(committedMax);
  const alarmLimits: Record<string, Range> = useSelector(
    getAlarmLimitsRequest,
    shallowEqual,
  ) as Record<string, Range>;
  const [rangeValue, setRangeValue] = React.useState<number[]>([
    alarmRangeValues.length ? alarmRangeValues[0] : alarmLimits[stateKey]?.lower,
    alarmRangeValues.length ? alarmRangeValues[1] : alarmLimits[stateKey]?.upper,
  ]);
  const [refs] = React.useState<Record<string, RefObject<HTMLDivElement>>>({
    [`${stateKey}_LOWER`]: useRef(null),
    [`${stateKey}_HIGHER`]: useRef(null),
  });

  const initSetValue = useCallback(() => {
    setOpen(openModal);
  }, [openModal]);

  useEffect(() => {
    initRefListener(refs);
  }, [initRefListener, refs]);

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
    if (onModalClose) {
      onModalClose(false);
    }
  };

  const handleConfirm = () => {
    dispatch(
      updateCommittedState(ALARM_LIMITS, {
        [stateKey]: {
          lower: rangeValue[0],
          upper: rangeValue[1],
        },
      }),
    );
    dispatch(
      updateCommittedState(ALARM_LIMITS_STANDBY, {
        [stateKey]: {
          lower: rangeValue[0],
          upper: rangeValue[1],
        },
      }),
    );
    requestCommitRange(rangeValue[0], rangeValue[1]);
    handleClose();
  };

  useEffect(() => {
    requestCommitRange(rangeValue[0], rangeValue[1]);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [requestCommitRange, JSON.stringify(rangeValue)]);

  const OnClickPage = () => {
    setActiveRotaryReference(null);
  };

  const modalContent = (
    <Grid
      container
      direction="column"
      alignItems="stretch"
      className={classes.contentContainer}
      onClick={OnClickPage}
    >
      {labelHeading && (
        <Grid container item xs className={`${classes.borderBottom} ${classes.head}`}>
          <Typography align="left" variant="h3">
            {label}
          </Typography>
        </Grid>
      )}
      <Grid container item xs>
        <Grid
          container
          item
          xs
          justify="center"
          alignItems="center"
          style={{ borderRight: `2px dashed ${theme.palette.background.default}` }}
          className={classes.borderBottom}
        >
          <Grid
            ref={refs[`${stateKey}_LOWER`]}
            container
            item
            xs
            justify="center"
            alignItems="center"
            style={{
              padding: theme.spacing(2),
              height: '100%',
            }}
          >
            <Grid alignItems="center" item className={classes.alarmValue}>
              <Typography align="center" variant="h4">
                {rangeValue[0] !== undefined ? Number(rangeValue[0]) : '--'}
              </Typography>
            </Grid>
            <Grid item>
              <ValueClicker
                referenceKey={`${stateKey}_LOWER`}
                value={rangeValue[0]}
                step={step}
                min={committedMin}
                max={committedMax}
                onClick={(value) => setRangeValue(Object.assign([], rangeValue, { 0: value }))}
                direction="column"
              />
            </Grid>
          </Grid>
        </Grid>
        <Grid
          container
          item
          xs
          justify="center"
          alignItems="center"
          className={classes.borderBottom}
        >
          <Grid
            ref={refs[`${stateKey}_HIGHER`]}
            container
            item
            xs
            justify="center"
            alignItems="center"
            style={{
              padding: theme.spacing(2),
              height: '100%',
            }}
          >
            <Grid item className={classes.alarmValue}>
              <Typography align="center" variant="h4">
                {rangeValue[1] !== undefined ? Number(rangeValue[1]) : '--'}
              </Typography>
            </Grid>
            <Grid item>
              <ValueClicker
                referenceKey={`${stateKey}_HIGHER`}
                value={rangeValue[1]}
                step={step}
                min={committedMin}
                max={committedMax}
                onClick={(value) => setRangeValue(Object.assign([], rangeValue, { 1: value }))}
                direction="column"
              />
            </Grid>
          </Grid>
        </Grid>
      </Grid>
      <Grid container item xs alignItems="center">
        <ValueSlider
          disabled={true}
          rangeValues={rangeValue}
          onChange={setRangeValue}
          min={min}
          max={max}
          step={step}
        />
      </Grid>
    </Grid>
  );

  return contentOnly ? (
    modalContent
  ) : (
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
        <span hidden={true}>{units}</span>
      </Grid>
      <ModalPopup
        withAction={true}
        label={`${label} - Alarm`}
        open={open}
        onClose={handleClose}
        onConfirm={handleConfirm}
      >
        {modalContent}
      </ModalPopup>
    </Grid>
  );
};

export default AlarmModal;
