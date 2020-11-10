import React from 'react';
import { makeStyles, Theme } from '@material-ui/core/styles';
import { Grid, Tabs, Tab, Typography } from '@material-ui/core';
import { PawGraphInfo, FlowGraphInfo, VolumeGraphInfo, PVLoopGraphInfo } from '../containers';
import { TabPanel, a11yProps } from '../../controllers/TabPanel';
import { PIPInfo, RRInfo, PEEPInfo, FiO2Info, TVInfo, IERatioInfo } from '../../info';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    height: '100%',
    width: '100%',
    display: 'grid',
    gridTemplateRows: '1fr 145px',
    gridGap: '10px',
  },
  topPanel: {
    alignItems: 'stretch',
  },
  topLeftPanel: {
    borderRadius: theme.panel.borderRadius,
    minWidth: '260px',
    backgroundColor: theme.palette.background.paper,
  },
  graphPanel: {
    borderRadius: theme.panel.borderRadius,
    marginLeft: theme.spacing(2),
    padding: theme.spacing(1),
    backgroundColor: theme.palette.background.paper,
    display: 'grid',
    gridTemplateRows: '30px 1fr',
    gridGap: 2,
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
    minHeight: 40,
  },
  bottomPanel: {
    // marginTop: theme.spacing(2),
  },
  bottomLeftPanel: {
    minWidth: '500px',
    marginRight: theme.spacing(2),
    backgroundColor: theme.palette.background.paper,
    borderRadius: theme.panel.borderRadius,
    height: '100%',
  },
  bottomRightPanel: {
    borderRadius: theme.panel.borderRadius,
    backgroundColor: theme.palette.background.paper,
    height: '100%',
  },
  bottomBorder: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
    height: '100%',
  },
  rightBorder: {
    borderRight: `2px dashed ${theme.palette.background.default}`,
    height: '100%',
  },
}));

/**
 * PressurControlMainView
 */
const PressureControlMainView = (): JSX.Element => {
  const classes = useStyles();
  const [value, setValue] = React.useState(0);

  const handleTabChange = (event: React.ChangeEvent<Record<string, unknown>>, newValue: number) => {
    setValue(newValue);
  };

  return (
    <Grid container className={classes.root}>
      <Grid container item xs={12} alignItems="stretch" className={classes.topPanel}>
        <Grid
          container
          item
          xs={3}
          direction="column"
          alignItems="stretch"
          className={classes.topLeftPanel}
        >
          <Grid item xs className={classes.bottomBorder}>
            <PIPInfo />
          </Grid>
          <Grid item xs>
            <PEEPInfo />
          </Grid>
        </Grid>
        <Grid container item xs direction="column" className={classes.graphPanel} wrap="nowrap">
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
              <Tab
                label="PV Loops"
                {...a11yProps(1)}
                className={classes.tab}
                classes={{ selected: classes.selectedTab }}
              />
              {/* NOTE: The tab below is disabled until their functionality is implemented. */}
              <Tab
                label="Compliance"
                {...a11yProps(2)}
                className={classes.tab}
                disabled
                classes={{ selected: classes.selectedTab }}
              />
            </Tabs>
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
              <PVLoopGraphInfo />
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
      </Grid>
      <Grid container item xs={12} className={classes.bottomPanel}>
        <Grid
          container
          item
          xs={7}
          justify="center"
          alignItems="stretch"
          className={classes.bottomLeftPanel}
        >
          <Grid item xs className={classes.rightBorder}>
            <RRInfo />
          </Grid>
          <Grid item xs style={{ height: '100%' }}>
            <FiO2Info />
          </Grid>
        </Grid>
        <Grid
          container
          item
          xs
          justify="center"
          alignItems="stretch"
          className={classes.bottomRightPanel}
        >
          <Grid item xs className={classes.rightBorder}>
            <TVInfo />
          </Grid>
          <Grid item xs>
            <IERatioInfo />
          </Grid>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default PressureControlMainView;
