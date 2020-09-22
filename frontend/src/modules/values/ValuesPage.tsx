import React from 'react';
import { makeStyles, Theme } from '@material-ui/core/styles';
import Grid from '@material-ui/core/Grid';
import {
  RRInfo,
  PIPInfo,
  PEEPInfo,
  FiO2Info,
  IERatioInfo,
  TVInfo,
  InspTimeInfo,
  IPAbovePEEPInfo,
  PplatInfo,
  PawInfo,
  MVeInfo,
  EtCO2Info,
} from '../info';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    flexGrow: 1,
    justifyContent: 'center',
    alignItems: 'stretch',
    marginBottom: theme.spacing(2),
  },
  leftPanel: {
    borderRadius: theme.panel.borderRadius,
    backgroundColor: theme.palette.background.paper,
    marginRight: theme.spacing(2),
  },
  rightContainer: {
    justifyContent: 'space-between',
    flexWrap: 'nowrap',
    width: '100%',
  },
  topRightPanel: {
    borderRadius: theme.panel.borderRadius,
    backgroundColor: theme.palette.background.paper,
    marginBottom: theme.spacing(1),
    width: '100%',
  },
  bottomRightPanel: {
    borderRadius: theme.panel.borderRadius,
    backgroundColor: theme.palette.background.paper,
    marginTop: theme.spacing(2),
    width: '100%',
  },
  bottomBorder: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
  },
  rightBorder: {
    borderRight: `2px dashed ${theme.palette.background.default}`,
  },
}));

/**
 * ValuesPage
 */
export const ValuesPage = (): JSX.Element => {
  const classes = useStyles();

  return (
    <Grid container direction="row" className={classes.root} wrap="nowrap">
      <Grid container item xs={7} direction="column" className={classes.leftPanel}>
        <Grid container item xs className={classes.bottomBorder}>
          <Grid item xs className={classes.rightBorder}>
            <RRInfo />
          </Grid>
          <Grid item xs>
            <IPAbovePEEPInfo />
          </Grid>
        </Grid>
        <Grid container item xs className={classes.bottomBorder}>
          <Grid item xs className={classes.rightBorder}>
            <PIPInfo />
          </Grid>
          <Grid item xs>
            <InspTimeInfo />
          </Grid>
        </Grid>
        <Grid container item xs>
          <Grid item xs className={classes.rightBorder}>
            <PEEPInfo />
          </Grid>
          <Grid item xs>
            <FiO2Info />
          </Grid>
        </Grid>
      </Grid>
      <Grid container item xs={5} direction="column" className={classes.rightContainer}>
        <Grid container item xs direction="column" className={classes.topRightPanel}>
          <Grid container item xs className={classes.bottomBorder}>
            <Grid item xs className={classes.rightBorder}>
              <PawInfo />
            </Grid>
            <Grid item xs>
              <TVInfo />
            </Grid>
          </Grid>
          <Grid container item xs>
            <Grid item xs className={classes.rightBorder}>
              <MVeInfo />
            </Grid>
            <Grid item xs>
              <EtCO2Info />
            </Grid>
          </Grid>
        </Grid>
        <Grid item container className={classes.bottomRightPanel}>
          <Grid item xs={7} className={classes.rightBorder}>
            <PplatInfo />
          </Grid>
          <Grid item xs={5}>
            <IERatioInfo />
          </Grid>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default ValuesPage;
