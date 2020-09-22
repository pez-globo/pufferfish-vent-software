import React from 'react';
import { Grid, Typography, makeStyles, Theme } from '@material-ui/core';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    // border: '1px solid yellow',
    justifyContent: 'space-between',
    alignItems: 'stretch',
    marginBottom: theme.spacing(2),
  },
  leftPanel: {
    flexDirection: 'column',
    marginRight: theme.spacing(2),
  },
  infoPanel: {
    flexDirection: 'column',
    justifyContent: 'center',
    borderRadius: theme.panel.borderRadius,
    padding: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
    marginBottom: theme.spacing(2),
  },
  rightPanel: {
    flexDirection: 'column',
    borderRadius: theme.panel.borderRadius,
    marginBottom: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
  },
  optionPanel: {
    padding: theme.spacing(2),
  },
  content: {
    fontFamily: 'sans-serif',
    fontWeight: 100,
  },
}));

/**
 * Props
 *
 * TODO: These props should be connected to the redux store.
 */
interface Props {
  model?: string;
  softwareVersion?: string;
  serialNumber?: string;
  batteryTime?: string;
}

/**
 * InfoTab
 *
 * TODO: Remove the default properties in the destructured `Props` argument below
 *       after hooking the `props` up to the redux store.
 */
export const InfoTab = ({
  model = '1.0',
  softwareVersion = '1.1',
  serialNumber = 'X1H4KJ348FKJHE43-1H495',
  batteryTime = '15 Minutes',
}: Props): JSX.Element => {
  const classes = useStyles();
  const infos = [
    { label: 'Serial Number', content: serialNumber },
    { label: 'Battery Time', content: batteryTime },
    { label: 'Model', content: model },
    { label: 'Software Version', content: softwareVersion },
  ];
  const options = [
    'Age',
    'Height',
    'PC-AC',
    'VC-AC',
    'PV Loops',
    'Plateau Pressure',
    'Sex',
    'NIV',
    'PC-SIMV',
    'VC-SIMV',
    'Compliance',
  ];

  return (
    <Grid container>
      <Grid container item xs className={classes.leftPanel}>
        {infos.map((info) => (
          <Grid container item xs className={classes.infoPanel}>
            <Grid item xs>
              <Typography variant="h5">{info.label}:</Typography>
            </Grid>
            <Grid item xs>
              <Typography variant="h5" className={classes.content}>
                {info.content}
              </Typography>
            </Grid>
          </Grid>
        ))}
      </Grid>
      <Grid container item xs className={classes.rightPanel}>
        <Grid item xs className={classes.optionPanel}>
          <Typography variant="h5">Options:</Typography>
          <br />
          {options.map((option) => (
            <Typography align="left" variant="h5" className={classes.content}>
              {option}
            </Typography>
          ))}
        </Grid>
      </Grid>
    </Grid>
  );
};

export default InfoTab;
