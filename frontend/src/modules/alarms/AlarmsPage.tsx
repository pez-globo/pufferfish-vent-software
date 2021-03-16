import { Button, Grid, Typography } from '@material-ui/core';
import { makeStyles, Theme, useTheme } from '@material-ui/core/styles';
import Pagination from '@material-ui/lab/Pagination';
import React, { RefObject, useEffect, useRef, useState } from 'react';
import { useDispatch, useSelector } from 'react-redux';
import { updateCommittedState } from '../../store/controller/actions';
import { AlarmLimitsRequest, VentilationMode, Range } from '../../store/controller/proto/mcu_pb';
import {
  getAlarmLimitsRequestStandby,
  getParametersRequestMode,
} from '../../store/controller/selectors';
import { ALARM_LIMITS, ALARM_LIMITS_STANDBY } from '../../store/controller/types';
import { setActiveRotaryReference } from '../app/Service';
import { ValueClicker } from '../controllers';
import ValueSlider from '../controllers/ValueSlider';
import ModeBanner from '../displays/ModeBanner';
import useRotaryReference from '../utils/useRotaryReference';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    justifyContent: 'space-between',
    alignItems: 'stretch',
    flexWrap: 'nowrap',
  },
  panel: {
    borderRadius: theme.panel.borderRadius,
    flexWrap: 'nowrap',
    marginBottom: theme.spacing(2),
  },
  leftContainer: {
    justifyContent: 'space-between',
    marginRight: theme.spacing(2),
  },
  paginationContainer: {
    alignItems: 'center',
    width: 'auto',
    marginLeft: 'auto',
    // border: '1px solid blue'
  },
  applyButton: {
    border: '1px solid black',
  },
  panelBg: {
    justifyContent: 'space-between',
    backgroundColor: theme.palette.background.paper,
    borderRadius: theme.panel.borderRadius,
  },
  infoPanel: {
    flexDirection: 'column',
    justifyContent: 'center',
    borderRadius: theme.panel.borderRadius,
    padding: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
    marginBottom: theme.spacing(2),
  },
  panelTitle: {
    padding: '5px 16px',
    borderBottom: `2px dashed ${theme.palette.background.default}`,
  },
  panelSlider: {
    padding: '5px 16px',
    borderTop: `2px dashed ${theme.palette.background.default}`,
  },
  numberButton: {
    padding: '5px 0px',
    backgroundColor: 'transparent',
    border: `1px solid ${theme.palette.common.white}`,
    marginRight: theme.spacing(1),
    borderRadius: 8,
    height: '100%',
    minWidth: 55,

    '&:hover': {
      backgroundColor: 'transparent',
    },
  },
  alarmValue: {
    marginRight: theme.spacing(3),
    border: `1px solid ${theme.palette.text.primary}`,
    minWidth: 75,
    paddingLeft: theme.spacing(1),
    paddingRight: theme.spacing(1),
    borderRadius: 8,
  },
}));

// NOTE: Temporary Alarm until the UI team address interface concerns.
interface AlarmProps {
  label: string;
  min: number;
  max: number;
  stateKey: string;
  step?: number;
  alarmLimits: Record<string, Range>;
  setAlarmLimits(alarmLimits: Partial<AlarmLimitsRequest>): void;
}

enum SliderType {
  LOWER,
  UPPER,
}

const Alarm = ({
  label,
  min,
  max,
  stateKey,
  step,
  alarmLimits,
  setAlarmLimits,
}: AlarmProps): JSX.Element => {
  const classes = useStyles();
  const theme = useTheme();
  const { initRefListener } = useRotaryReference(theme);
  const rangeValues: number[] = [alarmLimits[stateKey].lower, alarmLimits[stateKey].upper];
  const [refs] = React.useState<Record<string, RefObject<HTMLDivElement>>>({
    [`${stateKey}_LOWER`]: useRef(null),
    [`${stateKey}_HIGHER`]: useRef(null),
  });
  const setRangevalue = (range: number[]) => {
    setAlarmLimits({ [stateKey]: { lower: range[0], upper: range[1] } });
  };
  const onClick = (value: number, type: SliderType) => {
    setActiveRotaryReference(
      type === SliderType.LOWER ? `${stateKey}_LOWER` : `${stateKey}_HIGHER`,
    );
    setAlarmLimits({
      [stateKey]: {
        lower: type === SliderType.LOWER ? value : rangeValues[0],
        upper: type === SliderType.UPPER ? value : rangeValues[1],
      },
    });
  };

  useEffect(() => {
    initRefListener(refs);
  }, [initRefListener, refs]);

  return (
    <Grid container item xs={6} style={{ padding: '10px 12px' }}>
      <Grid container item xs className={classes.panelBg}>
        <Grid item xs={12} className={classes.panelTitle}>
          <Typography variant="h5">{label}</Typography>
        </Grid>
        <Grid
          container
          alignItems="center"
          justify="center"
          item
          xs={6}
          style={{ borderRight: `2px dashed ${theme.palette.background.default}` }}
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
                {rangeValues[0] !== undefined ? Number(rangeValues[0]) : '--'}
              </Typography>
            </Grid>
            <Grid item>
              <ValueClicker
                referenceKey={`${stateKey}_LOWER`}
                value={rangeValues[0]}
                onClick={(value: number) => onClick(value, SliderType.LOWER)}
                min={min}
                max={max}
                direction="column"
              />
            </Grid>
          </Grid>
        </Grid>
        <Grid container alignItems="center" justify="center" item xs={6}>
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
            <Grid alignItems="center" item className={classes.alarmValue}>
              <Typography align="center" variant="h4">
                {rangeValues[1] !== undefined ? Number(rangeValues[1]) : '--'}
              </Typography>
            </Grid>
            <Grid item>
              <ValueClicker
                referenceKey={`${stateKey}_HIGHER`}
                value={rangeValues[1]}
                onClick={(value: number) => onClick(value, SliderType.UPPER)}
                min={min}
                max={max}
                direction="column"
              />
            </Grid>
          </Grid>
        </Grid>
        <Grid item xs={12} className={classes.panelSlider}>
          <ValueSlider
            disabled={true}
            min={min}
            max={max}
            step={step}
            onChange={setRangevalue}
            rangeValues={rangeValues}
          />
        </Grid>
      </Grid>
    </Grid>
  );
};

interface AlarmConfiguration {
  label: string;
  min?: number;
  max?: number;
  stateKey: string;
  step?: number;
}

const alarmConfiguration = (ventilationMode: VentilationMode): Array<AlarmConfiguration> => {
  switch (ventilationMode) {
    case VentilationMode.pc_ac:
    case VentilationMode.pc_simv:
    case VentilationMode.vc_ac:
    case VentilationMode.vc_simv:
    case VentilationMode.niv:
      return [
        { label: 'RR', stateKey: 'rr' },
        { label: 'TV', stateKey: 'tv' },
        { label: 'Flow', stateKey: 'flow' },
        { label: 'MVe', stateKey: 'mve' },
        { label: 'Apnea', stateKey: 'apnea' },
        { label: 'Pressure above PEEP', stateKey: 'ipAbovePeep' },
        { label: 'PAW', stateKey: 'paw' },
        { label: 'PiP', stateKey: 'pip' },
        { label: 'PEEP', stateKey: 'peep' },
        { label: 'Insp. Time', stateKey: 'inspTime', step: 0.5 },
      ];
    case VentilationMode.hfnc:
    default:
      return [
        { label: 'FiO2', stateKey: 'fio2' },
        { label: 'SpO2', stateKey: 'spo2' },
        { label: 'HR', stateKey: 'hr', max: 200 },
      ];
  }
};

/**
 * AlarmsPage
 *
 * A container for housing all alarm configurations.
 */
export const AlarmsPage = (): JSX.Element => {
  const classes = useStyles();
  const itemsPerPage = 5;
  const [page, setPage] = React.useState(1);
  const [pageCount, setPageCount] = React.useState(1);
  const handleChange = (event: React.ChangeEvent<unknown>, value: number) => {
    setPage(value);
  };

  const alarmLimitsRequest = useSelector(getAlarmLimitsRequestStandby);
  const dispatch = useDispatch();
  const currentMode = useSelector(getParametersRequestMode);
  const [alarmLimits, setAlarmLimits] = useState(alarmLimitsRequest as Record<string, Range>);
  const updateAlarmLimits = (data: Partial<AlarmLimitsRequest>) => {
    setAlarmLimits({ ...alarmLimits, ...data } as Record<string, Range>);
    dispatch(updateCommittedState(ALARM_LIMITS_STANDBY, alarmLimits));
  };
  const applyChanges = () => dispatch(updateCommittedState(ALARM_LIMITS, alarmLimits));
  const alarmConfig = alarmConfiguration(currentMode);

  useEffect(() => {
    setPageCount(Math.ceil(alarmConfig.length / itemsPerPage));
  }, [alarmConfig]);

  const OnClickPage = () => {
    setActiveRotaryReference(null);
  };

  return (
    <Grid container direction="column" className={classes.root} onClick={OnClickPage}>
      <Grid container item xs direction="column" className={classes.panel}>
        <Grid container item xs direction="row">
          <Grid container spacing={3} style={{ margin: '-10px -12px' }}>
            {alarmConfig.slice((page - 1) * itemsPerPage, page * itemsPerPage).map((alarm) => {
              const key = `alarm-config-${alarm.stateKey}`;
              return (
                <Alarm
                  key={key}
                  label={alarm.label}
                  min={alarm.min || 0}
                  max={alarm.max || 100}
                  stateKey={alarm.stateKey}
                  step={alarm.step || 1}
                  alarmLimits={alarmLimits}
                  setAlarmLimits={updateAlarmLimits}
                />
              );
            })}
          </Grid>
        </Grid>
        <Grid container item xs>
          {/* <Grid item>
            <Typography variant="h3">Alarms</Typography>
          </Grid> */}
          <Grid container className={classes.paginationContainer}>
            {pageCount > 1 && (
              <Grid item>
                <Pagination
                  count={pageCount}
                  page={page}
                  onChange={handleChange}
                  defaultPage={1}
                  variant="outlined"
                  shape="rounded"
                  size="large"
                />
              </Grid>
            )}
            <Grid item style={{ textAlign: 'right' }}>
              <Button
                onClick={applyChanges}
                color="secondary"
                variant="contained"
                className={classes.applyButton}
              >
                Apply Changes
              </Button>
            </Grid>
          </Grid>
        </Grid>
        {/* Right Container for Storing Alarm Slides */}
        <Grid item xs>
          <ModeBanner bannerType="normal" />
        </Grid>
      </Grid>
      {/* Title */}
    </Grid>
  );
};

export default AlarmsPage;
