import React from 'react';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Grid, Tabs, Tab, Typography, Button } from '@material-ui/core';
import zIndex from '@material-ui/core/styles/zIndex';
import { useSelector } from 'react-redux';
import { PawGraphInfo, FlowGraphInfo, VolumeGraphInfo } from '../containers';
import { TabPanel, a11yProps } from '../../controllers/TabPanel';
import ControlInfo from '../containers/ControlInfo';
import {
  getCycleMeasurementsPEEP,
  getCycleMeasurementsRR,
  getParametersFiO2,
  getParametersFlow,
  getParametersPEEP,
  getROXIndex,
  getSensorMeasurementsFiO2,
  getSensorMeasurementsFlow,
  getSensorMeasurementsSpO2,
} from '../../../store/controller/selectors';
import { BMIN, CMH20, LMIN, PERCENT } from '../../info/units';
import ValueInfo from '../containers/ValueInfo';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100%',
    width: '100%',
  },
  topPanel: {
    alignItems: 'stretch',
  },
  topLeftPanel: {
    borderRadius: theme.panel.borderRadius,
    minWidth: '260px',
    backgroundColor: theme.palette.background.paper,
    display: 'grid',
    gridTemplateColumns: '1fr',
    // gridTemplateRows: '1fr 60px',
    overflow: 'hidden',
  },
  graphPanel: {
    borderRadius: theme.panel.borderRadius,
    // marginLeft: theme.spacing(2),
    // marginRight: theme.spacing(2),
    padding: theme.spacing(1),
    backgroundColor: theme.palette.background.paper,
    display: 'grid',
    gridTemplateRows: '1fr 30px',
    gridGap: 15,
  },
  tabs: {
    width: '100%',
    paddingTop: theme.spacing(0),
    paddingBottom: theme.spacing(1),
    minHeight: 30,
  },
  tab: {
    borderRadius: 8,
    border: `2px solid ${theme.palette.primary.main}`,
    margin: '0px 8px',
    zIndex: 1,
    minHeight: 0,
    padding: 0,
  },

  selectedTab: { color: theme.palette.primary.contrastText, lineHeight: '1.4' },

  tabIndicator: {
    borderRadius: 8,
    border: `2px solid ${theme.palette.primary.main}`,
    background: theme.palette.primary.main,
    // marginBottom: theme.spacing(1),
    zIndex: 0,
    minHeight: 30,
  },
  bottomPanel: {
    // marginTop: theme.spacing(2),
    display: 'grid',
    // gridTemplateColumns: '1fr 100px',
  },
  bottomLeftPanel: {
    minWidth: '500px',
    // marginRight: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
    borderRadius: '16px 0px 0px 16px',
    height: '100%',
  },
  bottomRightPanel: {
    borderRadius: '0px 16px 16px 0px',
    backgroundColor: '#06172e',
    height: '100%',
    display: 'flex',
    fontSize: '15px',
    alignItems: 'center',
    justifyContent: 'center',
    textAlign: 'center',
  },
  bottomBorder: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
    height: '100%',
  },
  rightBorder: {
    borderRight: `2px dashed ${theme.palette.background.default}`,
    height: '100%',
  },
  graphMainPanel: {
    display: 'grid',
    gridTemplateRows: '1fr 111px',
    gridGap: 15,
    paddingLeft: '15px',
  },
  buttonPosition: {
    position: 'absolute',
    right: '35px',
    zIndex: 2,
  },
  moreValues: {
    backgroundColor: '#06172e',
    borderRadius: '0px 0px 16px 16px',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    fontSize: '15px',
  },
}));

/**
 * HFNCMainView
 */
const HFNCMainView = (): JSX.Element => {
  const classes = useStyles();
  const [value, setValue] = React.useState(0);

  const handleTabChange = (event: React.ChangeEvent<Record<string, unknown>>, newValue: number) => {
    setValue(newValue);
  };

  return (
    <Grid container className={classes.root}>
      <Grid container item xs={12} alignItems="stretch" className={classes.topPanel}>
        <Grid item xs={4} justify="center" alignItems="stretch" className={classes.topLeftPanel}>
          <Grid container item justify="center" alignItems="stretch">
            <ValueInfo
              mainContainer={{
                value: useSelector(getSensorMeasurementsSpO2),
                label: 'SpO2',
                stateKey: 'spo2',
                units: PERCENT,
              }}
            />
          </Grid>
          <Grid container item justify="center" alignItems="stretch">
            <ValueInfo
              mainContainer={{
                value: useSelector(getCycleMeasurementsRR),
                label: 'RR',
                stateKey: 'rr',
                units: BMIN,
              }}
            />
          </Grid>
          <Grid container item justify="center" alignItems="stretch">
            <ValueInfo
              mainContainer={{
                value: useSelector(getROXIndex),
                label: 'ROX Index',
                stateKey: '',
              }}
            />
          </Grid>
        </Grid>
        <Grid container item xs direction="column" className={classes.graphMainPanel} wrap="nowrap">
          <Grid container item xs direction="column" className={classes.graphPanel} wrap="nowrap">
            <Grid>
              <Grid className={classes.buttonPosition}>
                <Button variant="contained" color="primary" disableElevation>
                  <Typography variant="subtitle2" align="center">
                    More Waveforms
                  </Typography>
                </Button>
              </Grid>
              <TabPanel value={value} index={0}>
                <Grid container item xs justify="space-between" style={{ height: '100%' }}>
                  <Grid item container style={{ height: '34%' }}>
                    <PawGraphInfo />
                  </Grid>
                  <Grid item style={{ height: '0%' }} />
                  <Grid item container style={{ height: '37%' }}>
                    <FlowGraphInfo />
                  </Grid>
                  <Grid item style={{ height: '0%' }} />
                  <Grid item container style={{ height: '33%' }}>
                    <VolumeGraphInfo />
                  </Grid>
                </Grid>
              </TabPanel>
              <TabPanel value={value} index={1}>
                <Grid container item xs style={{ height: '100%' }}>
                  <Grid item xs>
                    <Typography>PV Loops</Typography>
                  </Grid>
                </Grid>
              </TabPanel>
              <TabPanel value={value} index={2}>
                <Grid container item xs style={{ height: '100%' }}>
                  <Grid item xs>
                    <Typography>Compliance</Typography>
                  </Grid>
                </Grid>
              </TabPanel>
            </Grid>
            <Grid container item direction="row" justify="center" alignItems="center">
              <Tabs
                value={value}
                onChange={handleTabChange}
                variant="fullWidth"
                className={classes.tabs}
                classes={{ indicator: classes.tabIndicator }}
              >
                <Tab
                  label="Waveforms"
                  {...a11yProps(0)}
                  className={classes.tab}
                  classes={{ selected: classes.selectedTab }}
                />
                {/* NOTE: The 2 tabs below are disabled until their functionality is implemented. */}
                <Tab
                  label="Trends"
                  {...a11yProps(1)}
                  className={classes.tab}
                  disabled
                  classes={{ selected: classes.selectedTab }}
                />
                <Tab
                  label="Loops"
                  {...a11yProps(2)}
                  className={classes.tab}
                  disabled
                  classes={{ selected: classes.selectedTab }}
                />
              </Tabs>
            </Grid>
          </Grid>
          <Grid container item xs={12} className={classes.bottomPanel}>
            <Grid
              container
              item
              xs={12}
              justify="center"
              alignItems="stretch"
              className={classes.bottomLeftPanel}
            >
              <Grid item xs className={classes.rightBorder}>
                <ControlInfo
                  value={useSelector(getSensorMeasurementsFiO2)}
                  label="FiO2"
                  stateKey="fio2"
                  units={PERCENT}
                  committedSetting={useSelector(getParametersFiO2)}
                />
              </Grid>
              <Grid item xs className={classes.rightBorder}>
                <ControlInfo
                  value={useSelector(getSensorMeasurementsFlow)}
                  label="Flow Rate"
                  stateKey="flow"
                  units={LMIN}
                  committedSetting={useSelector(getParametersFlow)}
                />
              </Grid>
            </Grid>
          </Grid>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default HFNCMainView;
