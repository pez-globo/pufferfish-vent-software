import React, { RefObject } from 'react';
import { Grid, makeStyles, Theme } from '@material-ui/core';
import { ValueClicker } from '.';
import ValueDisplay from '../displays/ValueDisplay';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    justifyContent: 'center',
    height: '100%',
    padding: theme.spacing(2),
    alignItems: 'center',
  },
}));

interface Props {
  value: number;
  label: string;
  onClick: (value: number) => void;
  // An inclusive range can defined by [min, max].
  min?: number;
  max?: number;
  units?: string;
  reference: RefObject<HTMLDivElement>;
  referenceKey: string;
}

/**
 * ValueController
 *
 * A re-usable component for adjusting some value with a clicker.
 */
const ValueController = ({
  label,
  value,
  min = 0,
  max = 100,
  onClick,
  units,
  reference,
  referenceKey,
}: Props): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid ref={reference} container className={classes.root}>
      <Grid item xs>
        <ValueDisplay value={value} label={label} units={units} />
      </Grid>
      <Grid item>
        <ValueClicker
          referenceKey={referenceKey}
          value={value}
          onClick={onClick}
          min={min}
          max={max}
        />
      </Grid>
    </Grid>
  );
};

export default ValueController;
