import React from 'react';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Grid, Typography, Button } from '@material-ui/core';
import { useDispatch, useSelector } from 'react-redux';
import { updateCommittedParameter } from '../../store/controller/actions';
import { getParametersRequestMode } from '../../store/controller/selectors';
import { VentilationMode } from '../../store/controller/proto/mcu_pb';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    justifyContent: 'space-between',
    alignItems: 'stretch',
    marginBottom: theme.spacing(2),
  },
  modesPanel: {
    borderRadius: theme.panel.borderRadius,
    flexDirection: 'column',
    alignItems: 'stretch',
    marginRight: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
  },
  modeContainer: {
    minHeight: 100,
    padding: theme.spacing(2),
    // border: '1px solid red',
  },
  modeTitleItem: {
    paddingBottom: theme.spacing(2),
  },
  leftModeButtonOutline: {
    justifyContent: 'center',
    alignItems: 'center',
    marginRight: theme.spacing(2),
  },
  modeButtonOutline: {
    justifyContent: 'center',
    alignItems: 'center',
  },
  modeButton: {
    height: '60%',
    width: '100%',
    borderRadius: 16,
    border: `2px solid ${theme.palette.primary.main}`,
  },
  abbrevPanel: {
    flexDirection: 'column',
    borderRadius: theme.panel.borderRadius,
    backgroundColor: theme.palette.background.paper,
  },
  abbrevContanier: {
    minHeight: 64,
    padding: theme.spacing(2),
    // border: '1px solid red'
  },
  title: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
    minHeight: '60px',
    alignItems: 'center',
    padding: theme.spacing(2),
  },
  selected: {
    background: theme.palette.primary.main,
  },
}));

/**
 * ModesPage
 *
 * TODO: This page should be hooked up into the redux store so that it has access
 *       to the current ventilation `mode` and a dispatcher to update the mode.
 */
export const ModesPage = (): JSX.Element => {
  const classes = useStyles();
  const dispatch = useDispatch();
  const currentMode = useSelector(getParametersRequestMode);
  const updateMode = (mode: VentilationMode) => dispatch(updateCommittedParameter({ mode }));
  const buttonClass = (mode: VentilationMode) =>
    mode === currentMode ? `${classes.modeButton} ${classes.selected}` : `${classes.modeButton}`;
  return (
    <Grid container className={classes.root}>
      <Grid container item xs className={classes.modesPanel}>
        <Grid container item className={classes.title}>
          <Typography variant="h3">Modes</Typography>
        </Grid>
        <Grid container item xs>
          <Grid container item direction="column" className={classes.modeContainer}>
            <Grid item className={classes.modeTitleItem}>
              <Typography variant="h6">Pressure Controlled</Typography>
            </Grid>
            <Grid container item xs>
              <Grid container item xs className={classes.leftModeButtonOutline}>
                <Button
                  onClick={() => updateMode(VentilationMode.pc_ac)}
                  className={buttonClass(VentilationMode.pc_ac)}
                  variant="outlined"
                >
                  <Typography variant="h5">AC</Typography>
                </Button>
              </Grid>
              <Grid container item xs className={classes.modeButtonOutline}>
                <Button
                  onClick={() => updateMode(VentilationMode.pc_simv)}
                  className={buttonClass(VentilationMode.pc_simv)}
                  variant="outlined"
                >
                  <Typography variant="h5">SIMV</Typography>
                </Button>
              </Grid>
            </Grid>
          </Grid>
          <Grid container item direction="column" className={classes.modeContainer}>
            <Grid item className={classes.modeTitleItem}>
              <Typography variant="h6">Volume Controlled</Typography>
            </Grid>
            <Grid container item xs>
              <Grid container item xs className={classes.leftModeButtonOutline}>
                <Button
                  onClick={() => updateMode(VentilationMode.vc_ac)}
                  className={buttonClass(VentilationMode.vc_ac)}
                  variant="outlined"
                >
                  <Typography variant="h5">AC</Typography>
                </Button>
              </Grid>
              <Grid container item xs className={classes.modeButtonOutline}>
                <Button
                  onClick={() => updateMode(VentilationMode.vc_simv)}
                  className={buttonClass(VentilationMode.vc_simv)}
                  variant="outlined"
                >
                  <Typography variant="h5">SIMV</Typography>
                </Button>
              </Grid>
            </Grid>
          </Grid>
          <Grid container item direction="column" className={classes.modeContainer}>
            <Grid item className={classes.modeTitleItem}>
              <Typography variant="h6">Non-invasive</Typography>
            </Grid>
            <Grid container item xs>
              <Grid container item xs className={classes.leftModeButtonOutline}>
                <Button
                  onClick={() => updateMode(VentilationMode.niv)}
                  className={buttonClass(VentilationMode.niv)}
                  variant="outlined"
                >
                  <Typography variant="h5">NIV</Typography>
                </Button>
              </Grid>
              <Grid container item xs className={classes.modeButtonOutline}>
                <Button
                  onClick={() => updateMode(VentilationMode.hfnc)}
                  className={buttonClass(VentilationMode.hfnc)}
                  variant="outlined"
                >
                  <Typography variant="h5">HFNC</Typography>
                </Button>
              </Grid>
            </Grid>
          </Grid>
        </Grid>
      </Grid>
      <Grid container item xs={5} className={classes.abbrevPanel}>
        <Grid container item className={classes.title}>
          <Typography variant="h3">Abbreviations</Typography>
        </Grid>
        <Grid container item alignItems="center" className={classes.abbrevContanier}>
          <Grid item xs={2}>
            <Typography>
              <strong>AC</strong>:
            </Typography>
          </Grid>
          <Grid item xs>
            <Typography>Assisted Control</Typography>
          </Grid>
        </Grid>
        <Grid container item alignItems="center" className={classes.abbrevContanier}>
          <Grid item xs={2}>
            <Typography>
              <strong>SIMV</strong>:
            </Typography>
          </Grid>
          <Grid item xs>
            <Typography>Synchronized Intermittent Mandatory Ventilation</Typography>
          </Grid>
        </Grid>
        <Grid container item alignItems="center" className={classes.abbrevContanier}>
          <Grid item xs={2}>
            <Typography>
              <strong>NIV</strong>:
            </Typography>
          </Grid>
          <Grid item xs>
            <Typography>Non-invasive Ventilation</Typography>
          </Grid>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default ModesPage;
