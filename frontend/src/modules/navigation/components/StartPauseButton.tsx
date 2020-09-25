import React from 'react';
import { Link, useLocation } from 'react-router-dom';
import { Button, makeStyles, Theme } from '@material-ui/core';
import { QUICKSTART_ROUTE, DASHBOARD_ROUTE } from '../constants';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    minHeight: 80,
    maxHeight: 80,
    height: 80,
    // border: '1px solid red',
  },
}));

/**
 * StartPauseButton
 *
 * A button component for handling the starting & pausing of the ventilator.
 *
 * TODO: The `isVentilatorOn` state should be hooked into redux.
 */
export const StartPauseButton = (): JSX.Element => {
  const [isVentilatorOn, setIsVentilatorOn] = React.useState(false);
  const location = useLocation();
  const label = isVentilatorOn ? 'Pause Ventilation' : 'Start Ventilation';
  const toPath = isVentilatorOn ? QUICKSTART_ROUTE.path : DASHBOARD_ROUTE.path;
  const isDisabled = !isVentilatorOn && location.pathname !== QUICKSTART_ROUTE.path;

  return (
    <Button
      component={Link}
      to={toPath}
      onClick={() => setIsVentilatorOn(!isVentilatorOn)}
      disabled={isDisabled}
      style={{ border: '1px solid black' }}
    >
      {label}
    </Button>
  );
};

export default StartPauseButton;
