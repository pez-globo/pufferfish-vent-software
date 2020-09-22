import React from 'react';
import { Grid, makeStyles, Theme } from '@material-ui/core';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100%',
    paddingTop: theme.spacing(1),
    paddingBottom: theme.spacing(2),
    paddingLeft: theme.spacing(2),
    paddingRight: theme.spacing(2),
    // border: '1px solid teal',
  },
  controlPanel: {
    justifyContent: 'space-between',
    alignItems: 'center',
    minHeight: 40,
    // border: '1px solid orange'
  },
  marginRight: {
    marginRight: theme.spacing(1),
  },
}));

interface Props {
  valueDisplay: React.ReactNode;
  valueModal: React.ReactNode;
  alarmModal?: React.ReactNode;
}

/**
 * Knob
 *
 * A re-usable "knob" component for adjusting alarms and updating values all-
 * in-one.
 */
export const Knob = ({ valueDisplay, valueModal, alarmModal }: Props): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid container direction="column" className={classes.root}>
      <Grid item xs style={{ width: '100%' }}>
        {valueDisplay}
      </Grid>
      <Grid container item direction="row" className={classes.controlPanel} wrap="nowrap">
        <Grid item xs={6} className={classes.marginRight}>
          {valueModal}
        </Grid>
        {alarmModal && (
          <Grid item xs={6}>
            {alarmModal}
          </Grid>
        )}
      </Grid>
    </Grid>
  );
};

export default Knob;
