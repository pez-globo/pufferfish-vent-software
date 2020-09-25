import React from 'react';
import { Button, Grid, Typography, makeStyles, Theme } from '@material-ui/core';
import CheckCircleIcon from '@material-ui/icons/CheckCircle';
import CancelIcon from '@material-ui/icons/Cancel';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    justifyContent: 'center',
    alignItems: 'stretch',
    height: '100%',
    padding: theme.spacing(2),
  },
  startButton: {
    width: '100%',
    border: '1px solid black',
    borderRadius: 8,
    minHeight: 44,
  },
  viewMoreButton: {
    width: '100%',
    border: '1px solid black',
    borderRadius: 8,
    minHeight: 44,
  },
  marginRight: {
    marginRight: theme.spacing(2),
  },
  successIcon: {
    color: theme.palette.success.main,
  },
  cancelIcon: {
    color: theme.palette.error.main,
  },
  dateTime: {
    marginBottom: 30,
  },
  time: {
    marginRight: 75,
    fontFamily: 'sans-serif',
    fontWeight: 100,
  },
  date: {
    fontFamily: 'sans-serif',
    fontWeight: 100,
  },
}));

interface Props {
  label: string;
  isSuccess: boolean;
  timestamp: Date;
}

/**
 * TestTool
 *
 * A component for performing tests and calibrations.
 */
export const TestTool = ({ label, isSuccess, timestamp }: Props): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid container direction="column" className={classes.root}>
      <Grid container item xs direction="row" justify="space-between">
        <Grid item>
          <Typography variant="h5">{label}</Typography>
        </Grid>
        <Grid item>
          {isSuccess ? (
            <CheckCircleIcon className={classes.successIcon} />
          ) : (
            <CancelIcon className={classes.cancelIcon} />
          )}
        </Grid>
      </Grid>
      <Grid container item xs direction="row" justify="flex-start" className={classes.dateTime}>
        <Grid item>
          <Typography className={classes.time}>
            {timestamp.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })}
          </Typography>
        </Grid>
        <Grid item>
          <Typography className={classes.date}>
            {timestamp.toLocaleDateString([], {
              month: '2-digit',
              day: '2-digit',
              year: 'numeric',
            })}
          </Typography>
        </Grid>
      </Grid>
      <Grid container item direction="row" justify="space-evenly" alignItems="center">
        <Grid item xs className={classes.marginRight}>
          <Button variant="contained" color="secondary" className={classes.startButton}>
            Start
          </Button>
        </Grid>
        <Grid item xs>
          <Button variant="contained" color="primary" className={classes.viewMoreButton}>
            View More
          </Button>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default TestTool;
