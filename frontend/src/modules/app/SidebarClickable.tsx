import React from 'react';
import { Link } from 'react-router-dom';
import { Button, Grid, Typography, makeStyles, Theme, ButtonBase } from '@material-ui/core';
import { useDispatch, useSelector } from 'react-redux';
import KeyboardArrowLeftIcon from '@material-ui/icons/KeyboardArrowLeft';
import { DASHBOARD_ROUTE, QUICKSTART_ROUTE, SCREENSAVER_ROUTE } from '../navigation/constants';
import ShutdownIcon from '../icons/ShutdownIcon';
import { getParameters, getParametersRequest } from '../../store/controller/selectors';
import { HomeIcon } from '../icons';
import { updateState } from '../../store/controller/actions';
import { MessageType } from '../../store/controller/types';
import Navigation from '../navigation/Navigation';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    width: 220,
    height: '100%',
    backgroundColor: theme.palette.background.paper,
    padding: '15px',
    display: 'Grid',
    gridTemplateRows: '35px 1fr',
  },
  shutdownButton: {
    '&:hover': { background: theme.palette.error.dark },
    backgroundColor: '#010010',
    color: theme.palette.text.primary,
    height: 80,
    width: '100%',
    borderRadius: 0,
  },
  screensaverButton: {
    height: 80,
    width: '100%',
    minWidth: 0,
    borderRadius: 0,
  },
  navMenu: {
    height: '100%',
  },
  homeButton: {
    display: 'flex',
  },
  homeSaver: {
    borderRadius: 0,
    backgroundColor: '#09253c',
    '&:hover': {
      backgroundColor: '#09253c',
    },
  },
  saverButtons: {
    display: 'grid',
    gridTemplateColumns: '1fr 1fr',
  },
  sideMenu: {
    height: '100%',
    padding: '15px 30px 0px',
    display: 'grid',
    gridTemplateRows: '1fr 80px',
  },
}));

/**
 * SidebarClickable
 *
 * The main FullPageNavigation panel of the application.
 */
const SidebarClickable = ({
  toggleStatus,
}: {
  toggleStatus(value: boolean): void;
}): JSX.Element => {
  const classes = useStyles();
  const dispatch = useDispatch();

  const lockScreen = () => {
    toggleStatus(false);
    dispatch(updateState(MessageType.ScreenStatus, { lock: true }));
  };

  const parameterRequest = useSelector(getParametersRequest);

  return (
    <Grid
      container
      direction="column"
      justify="space-between"
      alignItems="stretch"
      className={classes.root}
      wrap="nowrap"
    >
      <Grid item className={classes.saverButtons}>
        <Grid>
          <Button
            component={Link}
            to={parameterRequest.ventilating ? DASHBOARD_ROUTE.path : QUICKSTART_ROUTE.path}
            variant="contained"
            color="primary"
            className={classes.homeSaver}
            disableElevation
          >
            <div className={classes.homeButton}>
              <HomeIcon fontSize="small" style={{ fontSize: '1rem', marginRight: '10px' }} />
              <Typography variant="subtitle2" align="center">
                Home
              </Typography>
            </div>
          </Button>
        </Grid>
        <Grid>
          <Button
            onClick={() => toggleStatus(false)}
            variant="contained"
            color="primary"
            className={classes.homeSaver}
            style={{ float: 'right' }}
            disableElevation
          >
            <div className={classes.homeButton}>
              <KeyboardArrowLeftIcon style={{ fontSize: '1rem' }} />
            </div>
          </Button>
        </Grid>
      </Grid>
      <Grid item className={classes.sideMenu}>
        <Grid item className={classes.navMenu}>
          <Navigation fullPage={true} />
        </Grid>
        <Grid item>
          <Button
            variant="contained"
            onClick={lockScreen}
            className={classes.shutdownButton}
            disableElevation
          >
            <div>
              <ShutdownIcon style={{ fontSize: '1.5rem' }} />
              <br />
              <Typography variant="subtitle2" style={{ color: 'white' }} align="center">
                Lock Screen
              </Typography>
            </div>
          </Button>
        </Grid>
      </Grid>
    </Grid>
  );
};

export default SidebarClickable;
