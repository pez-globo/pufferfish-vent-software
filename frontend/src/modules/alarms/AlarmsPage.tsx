import React, { useEffect, useState } from 'react';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Typography, Button, Grid } from '@material-ui/core';
import Pagination from '@material-ui/lab/Pagination';
import { useDispatch, useSelector } from 'react-redux';
import ValueSlider from '../controllers/ValueSlider';
import ModeBanner from '../displays/ModeBanner';
import { getAlarmLimitsRequest, getParametersRequestMode } from '../../store/controller/selectors';
import { ALARM_LIMITS } from '../../store/controller/types';
import { updateCommittedState } from '../../store/controller/actions';
import { AlarmLimitsRequest, VentilationMode } from '../../store/controller/proto/mcu_pb';

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
    backgroundColor: theme.palette.background.paper,
  },
  leftContainer: {
    justifyContent: 'space-between',
    borderRight: `2px dashed ${theme.palette.background.default}`,
    padding: theme.spacing(2),
  },
  rightContainer: {
    justifyContent: 'space-between',
    padding: theme.spacing(2),
  },
  paginationContainer: {
    alignItems: 'center',
    width: '100%',
    // border: '1px solid blue'
  },
  applyButton: {
    width: '100%',
    border: '1px solid black',
  },
}));

// NOTE: Temporary Alarm until the UI team address interface concerns.
interface AlarmProps {
  label: string;
  min: number;
  max: number;
  stateKey: string;
  step?: number;
  alarmLimits: Record<string, number>;
  setAlarmLimits(alarmLimits: Partial<AlarmLimitsRequest>): void;
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
  const rangeValues: number[] = [alarmLimits[`${stateKey}Min`], alarmLimits[`${stateKey}Max`]];
  const setRangevalue = (range: number[]) => {
    setAlarmLimits({ [`${stateKey}Min`]: range[0], [`${stateKey}Max`]: range[1] });
  };
  return (
    <Grid container>
      <Grid item xs={12} style={{ paddingBottom: 20 }}>
        <Typography variant="h5">{label}</Typography>
      </Grid>
      <Grid item xs={12}>
        <ValueSlider
          min={min}
          max={max}
          step={step}
          onChange={setRangevalue}
          rangeValues={rangeValues}
        />
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
        { label: 'RR', stateKey: 'rr' },
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

  const alarmLimitsRequest = useSelector(getAlarmLimitsRequest);
  const dispatch = useDispatch();
  const currentMode = useSelector(getParametersRequestMode);

  const [alarmLimits, setAlarmLimits] = useState(alarmLimitsRequest as Record<string, number>);
  const updateAlarmLimits = (data: Partial<AlarmLimitsRequest>) =>
    setAlarmLimits({ ...alarmLimits, ...data } as Record<string, number>);
  const applyChanges = () => dispatch(updateCommittedState(ALARM_LIMITS, alarmLimits));
  const alarmConfig = alarmConfiguration(currentMode);

  useEffect(() => {
    setPageCount(Math.ceil(alarmConfig.length / itemsPerPage));
  }, [alarmConfig]);

  return (
    <Grid container direction="column" className={classes.root}>
      <Grid container item xs direction="row" className={classes.panel}>
        <Grid container item xs={3} direction="column" className={classes.leftContainer}>
          <Grid item>
            <Typography variant="h3">Alarms</Typography>
          </Grid>
          <Grid container direction="column" className={classes.paginationContainer}>
            <Grid item xs style={{ marginBottom: 10 }}>
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
            <Grid item style={{ width: '100%' }}>
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
        <Grid container item xs direction="column" className={classes.rightContainer}>
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
      {/* Title */}
      <Grid item>
        <ModeBanner bannerType="normal" />
      </Grid>
    </Grid>
  );
};

export default AlarmsPage;
