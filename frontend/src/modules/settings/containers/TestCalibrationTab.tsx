import React from 'react';
import { Grid, makeStyles, Theme } from '@material-ui/core';
import TestTool from '../../controllers/TestTool';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    // border: '1px solid yellow',
    justifyContent: 'space-between',
    alignItems: 'stretch',
  },
  item: {
    borderRadius: theme.panel.borderRadius,
    // padding: theme.spacing(1),
    marginBottom: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
  },
  leftPanel: {
    marginRight: theme.spacing(2),
  },
}));

/**
 * InfoTab
 *
 * TODO: Remove the default properties in the destructured `Props` argument below
 *       after hooking the `props` up to the redux store.
 */
export const TestCalibrationTab = (): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid container>
      <Grid container item xs direction="column" className={classes.leftPanel}>
        <Grid item xs className={classes.item}>
          <TestTool label="Pre-Use Check" isSuccess={true} timestamp={new Date()} />
        </Grid>
        <Grid item xs className={classes.item}>
          <TestTool label="Tightness" isSuccess={false} timestamp={new Date()} />
        </Grid>
        <Grid item xs className={classes.item}>
          <TestTool label="O2" isSuccess={true} timestamp={new Date()} />
        </Grid>
      </Grid>
      <Grid container item xs direction="column">
        <Grid item xs className={classes.item}>
          <TestTool label="Patient Circuit Test" isSuccess={true} timestamp={new Date()} />
        </Grid>
        <Grid container item xs className={classes.item}>
          <TestTool label="Flow Sensor" isSuccess={true} timestamp={new Date()} />
        </Grid>
        <Grid container item xs className={classes.item}>
          <TestTool label="Pressure Sensor" isSuccess={false} timestamp={new Date()} />
        </Grid>
      </Grid>
    </Grid>
  );
};

export default TestCalibrationTab;
