import React from 'react';
import { Grid, makeStyles, Theme, Typography } from '@material-ui/core';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    width: '100%',
    // border: '1px solid red'
  },
  displayContainer: {
    padding: 0,
    // border: '1px solid orange',
  },
  liveContainer: {
    justifyContent: 'center',
    alignItems: 'center',
    height: '100%',
    // border: '1px solid blue'
  },
  liveBox: {
    textAlign: 'center',
    width: '100%',
    fontSize: 14,
    borderRadius: 5,
    border: `2px solid ${theme.palette.primary.main}`,
  },
  valueContainer: {
    justifyContent: 'flex-start',
    alignItems: 'center',
    flexGrow: 1,
    // border: '1px solid yellow',
  },
  valueLabel: {
    fontSize: '4rem',
    lineHeight: '1',
    // border: '1px solid red',
  },
  unitsLabel: {
    paddingLeft: theme.spacing(1),
    paddingTop: theme.spacing(4),
    opacity: 0.8,
    // border: '1px solid red'
  },
}));

export interface ValueProps {
  value: number;
}

export interface Props {
  value: number;
  label: string;
  units?: string;
  isLive?: boolean;
}

/**
 * ValueDisplay
 *
 * A re-usable component for displaying numeric values.
 */
export const ValueDisplay = ({ value, label, units = '', isLive = false }: Props): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid container direction="column" className={classes.root}>
      <Grid
        container
        item
        direction="row"
        justify="space-between"
        alignItems="center"
        wrap="nowrap"
      >
        <Grid item xs style={{ width: '100%' }}>
          <Typography variant="h5">{label}</Typography>
        </Grid>
        {isLive && (
          <Grid container item xs={3} className={classes.liveContainer}>
            <Typography className={classes.liveBox}>Live</Typography>
          </Grid>
        )}
      </Grid>
      <Grid
        container
        item
        xs
        justify="flex-start"
        alignItems="center"
        className={classes.displayContainer}
        wrap="nowrap"
      >
        <Typography align="center" variant="h2" className={classes.valueLabel}>
          {value !== undefined && !Number.isNaN(value)
            ? value.toFixed(0).replace('/^-0$/', '0')
            : '--'}
        </Typography>
        {units !== '' && (
          <Typography align="center" variant="h5" className={classes.unitsLabel}>
            {units}
          </Typography>
        )}
      </Grid>
    </Grid>
  );
};

export default ValueDisplay;
