import React, { useEffect } from 'react';
import {
  Grid,
  Typography,
  FormControl,
  FormControlLabel,
  RadioGroup,
  Radio,
  Box,
  Button,
  makeStyles,
  Theme,
} from '@material-ui/core';
import { useSelector } from 'react-redux';
import ValueController from '../../controllers/ValueController';
import { getClock } from '../../../store/app/selectors';

import {
  getFrontendDisplaySetting,
  getSystemSettingRequest,
} from '../../../store/controller/selectors';
import { ToggleValue } from '../../displays/ToggleValue';
import { DECIMAL_RADIX } from '../../app/AppConstants';
import { ThemeVariant, Unit } from '../../../store/controller/proto/frontend_pb';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    marginBottom: theme.spacing(2),
  },
  periodButton: {
    width: '100%',
    border: `1px solid ${theme.palette.common.black}`,
    borderRadius: 8,
    height: '100%',
    minHeight: 55,
    minWidth: 78,
  },
  themeButton: {
    marginBottom: theme.spacing(2),
    width: 100,
  },
  leftPanel: {
    backgroundColor: theme.palette.background.paper,
    borderRadius: theme.panel.borderRadius,
  },
  rightPanel: {
    height: '100%',
    marginLeft: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
    borderRadius: theme.panel.borderRadius,
  },
  borderBottom: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
  },
  leftContainer: {
    borderTop: `2px dashed ${theme.palette.background.default}`,
    paddingLeft: theme.spacing(2),
  },
  date: {
    paddingRight: theme.spacing(5),
  },
  dateTime: {
    paddingTop: 10,
    fontFamily: 'sans-serif',
    fontWeight: 100,
  },
  rightBorder: {
    borderRight: `2px dashed ${theme.palette.background.default}`,
  },
  periodFormControl: {
    width: '100%',
    padding: theme.spacing(2),
  },
  selected: {
    background: theme.palette.primary.main,
  },
}));

/**
 * props
 */
// interface props {
// }

// Periods for 12-Hour Based Time Format
enum Period {
  AM,
  PM,
}

// Returns the number of days in a month for a given year.
const getDaysInMonth = (month: number, year: number) => {
  const days: number = new Date(year, month, 0).getDate();
  return days;
};
// Converts a 24-hour formatted hour to 12-hour period based format.
const to12HourClock = (hour: number) => {
  return hour % 12 || 12;
};
// Converts a 12-hour formatted hour to 24-hour based format.
const to24HourClock = (hour: number, period: Period) => {
  return period === Period.PM ? hour + 12 : hour;
};

interface Props {
  onSettingChange(settings: Record<string, unknown>): void;
}
/**
 * DisplayTab
 *
 * TODO: Hook this up to the redux state to persist changes across the system's state.
 *       We need to make sure that updates to dat
 */
export const DisplayTab = ({ onSettingChange }: Props): JSX.Element => {
  const classes = useStyles();
  const systemSettings = useSelector(getSystemSettingRequest);
  const displaySettings = useSelector(getFrontendDisplaySetting);
  const [brightness, setBrightness] = React.useState(systemSettings.brightness);
  const [theme, setTheme] = React.useState(displaySettings.theme);
  const [unit, setUnit] = React.useState(displaySettings.unit);
  const clock = useSelector(getClock);
  const [date, setDate] = React.useState<Date>(new Date(systemSettings.date * 1000));
  const [period, setPeriod] = React.useState(date.getHours() >= 12 ? Period.PM : Period.AM);
  const [minute, setMinute] = React.useState(date.getMinutes());
  const [hour, setHour] = React.useState(to12HourClock(date.getHours())); // Note: `date.hours()` is 24-hour formatted.
  const [day, setDay] = React.useState(date.getDate());
  const [month, setMonth] = React.useState(
    date.getMonth() + 1,
  ); /* Note: `getMonth()` returns a value in [0, 11] */
  const [year, setYear] = React.useState(date.getFullYear());

  useEffect(() => {
    const dateChange = new Date(year, month - 1, day, to24HourClock(hour, period), minute);
    onSettingChange({
      brightness,
      theme,
      unit,
      date: parseInt((dateChange.getTime() / 1000).toFixed(0), DECIMAL_RADIX),
    });
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [date, period, minute, hour, day, month, year, unit, theme, brightness]);

  const handleMonthChange = (change: number) => {
    const maxDaysInMonth = getDaysInMonth(change, year);
    // Update `day` component if its value > max days in the new month.
    if (day > maxDaysInMonth) {
      setDay(maxDaysInMonth);
    }
    setMonth(change);
  };
  const buttonClass = (updatedPeriod: Period) =>
    updatedPeriod === period
      ? `${classes.periodButton} ${classes.selected}`
      : `${classes.periodButton}`;

  return (
    <Grid container className={classes.root}>
      <Grid container item xs={4}>
        <Grid container item direction="column" className={classes.leftPanel}>
          {/* Brightness */}
          <Grid container item xs direction="row">
            <ValueController
              value={brightness}
              label="Brightness"
              units="%"
              onClick={setBrightness}
              min={0}
              max={100}
            />
          </Grid>
          {/* Color TODO: This should be a toggle switch. */}
          <Grid
            container
            item
            xs
            direction="column"
            justify="center"
            className={classes.leftContainer}
          >
            <Typography variant="h5" className={classes.root}>
              Color
            </Typography>
            <ToggleValue
              toggleBetween={[
                { label: 'Dark UI', value: ThemeVariant.dark },
                { label: 'Light UI', value: ThemeVariant.light },
              ]}
              onToggle={(selected: number) => setTheme(selected as ThemeVariant)}
              selected={theme}
            />
          </Grid>
          {/* Unit TODO: This should be a toggle switch. */}
          <Grid
            container
            item
            xs
            direction="column"
            justify="center"
            className={classes.leftContainer}
          >
            <Typography variant="h5" className={classes.root}>
              Unit
            </Typography>
            <ToggleValue
              toggleBetween={[
                { label: 'Imperial', value: Unit.imperial },
                { label: 'Metric', value: Unit.metric },
              ]}
              onToggle={(selected: number) => setUnit(selected as Unit)}
              selected={unit}
            />
          </Grid>
          {/* Date & Time */}
          <Grid
            container
            item
            xs
            direction="row"
            justify="flex-start"
            alignItems="center"
            className={classes.leftContainer}
          >
            <Box className={classes.date}>
              <Typography variant="h5">Date:</Typography>
              <Typography className={classes.dateTime}>
                {clock
                  .toLocaleDateString([], { month: '2-digit', day: '2-digit', year: 'numeric' })
                  .replace('/', ' - ')
                  .replace('/', ' - ')}
              </Typography>
            </Box>
            <Box>
              <Typography variant="h5">Time:</Typography>
              <Typography className={classes.dateTime}>{clock.toLocaleTimeString()}</Typography>
            </Box>
          </Grid>
        </Grid>
      </Grid>
      {/* Right Panel: Date & Time */}
      <Grid container item xs={8}>
        <Grid container item xs direction="column" className={classes.rightPanel}>
          <Grid container item xs alignItems="stretch" className={classes.borderBottom}>
            <Grid item xs className={classes.rightBorder}>
              <ValueController value={hour} label="Hour" onClick={setHour} min={1} max={12} />
            </Grid>
            <Grid item xs>
              <ValueController value={minute} label="Minute" onClick={setMinute} min={0} max={59} />
            </Grid>
            <Grid container item justify="center" alignItems="center" xs={3}>
              <FormControl component="fieldset" className={classes.periodFormControl}>
                <Grid container item className={classes.root}>
                  <Button
                    onClick={(event) => setPeriod(Period.AM)}
                    variant="outlined"
                    className={buttonClass(Period.AM)}
                  >
                    <Typography variant="h6">AM</Typography>
                  </Button>
                </Grid>
                <Grid container item>
                  <Button
                    onClick={(event) => setPeriod(Period.PM)}
                    variant="outlined"
                    className={buttonClass(Period.PM)}
                  >
                    <Typography variant="h6">PM</Typography>
                  </Button>
                </Grid>
              </FormControl>
            </Grid>
          </Grid>
          <Grid container item xs direction="row" className={classes.borderBottom}>
            <Grid item xs className={classes.rightBorder}>
              <ValueController
                value={month}
                label="Month"
                onClick={handleMonthChange}
                min={1}
                max={12}
              />
            </Grid>
            <Grid item xs>
              <ValueController
                value={day}
                label="Day"
                onClick={setDay}
                min={1}
                max={getDaysInMonth(month, year)}
              />
            </Grid>
            <Grid item xs={3} />
          </Grid>
          <Grid container item xs direction="row" className={classes.borderBottom}>
            <Grid item xs className={classes.rightBorder}>
              <ValueController value={year} label="Year" onClick={setYear} min={year} max={year} />
            </Grid>
            {/* Moved Apply button out of the tab */}
            {/* <Grid container item xs justify='center' alignItems='center' >
                        <Button variant='contained' color='secondary'>
                            Apply Changes
                        </Button>
                    </Grid> */}
          </Grid>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default DisplayTab;
