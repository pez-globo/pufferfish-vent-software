import React from 'react';
import { Grid, makeStyles, Theme, Typography } from '@material-ui/core';
import TrendIcon from '../icons/TrendUpIcon';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100%',
    width: '100%',
    textAlign: 'center',
    // border: '1px solid red'
  },
  displayContainer: {
    padding: 0,
    alignItems: 'flex-start',
    // border: '1px solid orange',
  },
  valueContainer: {
    flexGrow: 1,
    // border: '1px solid yellow',
  },
  ControlLabel: {
    fontSize: '4.5rem',
    // border: '1px solid red',
    fontWeight: 'lighter',
    lineHeight: '.9',
    margin: '0 auto',
  },
  unitsLabel: {
    fontWeight: 100,
    paddingLeft: theme.spacing(0),
    // paddingTop: theme.spacing(4),
    color: theme.palette.grey[400],
    // border: '1px solid red'
  },
  labelItems: {
    alignItems: 'flex-start',
    // border: '1px solid red'
  },
}));

export interface Props {
  value: number;
  label: string;
  units?: string;
}

/**
 * ControlJumbotron
 *
 * A component for displaying values in a large way.
 */
export const ControlJumbotron = ({ value, label, units = '' }: Props): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid container className={classes.root}>
      <Grid item style={{ width: '100%' }} className={classes.labelItems}>
        <Typography variant="h3" style={{ fontWeight: 'bold', fontSize: '1.8rem' }}>
          {label}
        </Typography>
      </Grid>
      <Grid
        container
        item
        xs
        alignItems="center"
        className={classes.displayContainer}
        wrap="nowrap"
      >
        <Typography variant="h1" className={classes.ControlLabel}>
          {value !== undefined ? value.toFixed(0) : '--'}
        </Typography>
      </Grid>
      <Grid item style={{ width: '100%' }}>
        <Typography variant="h4" className={classes.unitsLabel} style={{ fontSize: '1.3rem' }}>
          {units}
        </Typography>
      </Grid>
    </Grid>
  );
};

export default ControlJumbotron;
