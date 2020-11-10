import React, { useState } from 'react';
import { connect, useDispatch, useSelector } from 'react-redux';
import { Grid, makeStyles, Theme, Typography } from '@material-ui/core';
import { updateCommittedParameter, updateCommittedState } from '../../../store/controller/actions';
import { PARAMETER_STANDBY } from '../../../store/controller/types';
import { ValueModal, AlarmModal } from '../../controllers';
import { ClickHandler } from './ValueInfo';

interface Props {
  value: number;
  committedSetting: number;
  label: string;
  stateKey: string;
  units?: string;
}

interface ValueProps {
  value: number;
  label: string;
  units: string;
}

const useStyles = makeStyles((theme: Theme) => ({
  rootParent: {
    flexDirection: 'initial',
    height: '100%',
  },
  root: {
    padding: theme.spacing(1),
    paddingLeft: theme.spacing(2),
    paddingRight: theme.spacing(2),
    flexDirection: 'initial',
    height: '100%',
  },
  controlPanel: {
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  marginRight: {
    marginRight: theme.spacing(1),
  },
  displayContainer: {
    padding: 0,
    // border: '1px solid orange',
  },
  liveContainer: {
    justifyContent: 'center',
    alignItems: 'center',
    height: '100%',
    // border: '1px solid blue'
  },
  liveBox: {
    textAlign: 'center',
    width: '100%',
    fontSize: 14,
    borderRadius: 5,
    border: `2px solid ${theme.palette.primary.main}`,
  },
  valueContainer: {
    justifyContent: 'flex-start',
    alignItems: 'center',
    flexGrow: 1,
    // border: '1px solid yellow',
  },
  valueLabel: {
    fontSize: '4rem',
    lineHeight: '1',
    // border: '1px solid red',
  },
  unitsLabel: {
    paddingLeft: theme.spacing(1),
    paddingTop: theme.spacing(4),
    opacity: 0.8,
    // border: '1px solid red'
  },
}));

export const ValueControl = ({ value, label, units }: ValueProps): JSX.Element => {
  const classes = useStyles();
  return (
    <Grid container direction="column" className={classes.rootParent}>
      <Grid item xs style={{ width: '100%' }}>
        <Grid container direction="column" className={classes.root}>
          <Grid
            container
            item
            direction="row"
            justify="space-between"
            alignItems="center"
            wrap="nowrap"
          >
            <Grid item xs style={{ width: '100%' }}>
              <Typography variant="h5">{label}</Typography>
            </Grid>
          </Grid>
          <Grid
            container
            item
            xs
            justify="flex-start"
            alignItems="center"
            className={classes.displayContainer}
            wrap="nowrap"
          >
            <Typography align="center" variant="h2" className={classes.valueLabel}>
              {value !== undefined ? value.toFixed(0) : '--'}
            </Typography>
            {units !== '' && (
              <Typography align="center" variant="body1" className={classes.unitsLabel}>
                {units}
              </Typography>
            )}
          </Grid>
        </Grid>
      </Grid>
    </Grid>
  );
};

/**
 * Control Info
 *
 * Component for showing information.
 *
 */
const ControlInfo = (props: Props): JSX.Element => {
  const { value, label, units, stateKey, committedSetting } = props;
  const [open, setOpen] = useState(false);
  const dispatch = useDispatch();
  const doSetValue = (setting: number) => {
    dispatch(updateCommittedParameter({ [stateKey]: setting }));
    dispatch(updateCommittedState(PARAMETER_STANDBY, { [stateKey]: setting }));
  };
  const onClick = () => {
    setOpen(true);
  };
  const handleClick = ClickHandler(onClick, () => {
    return false;
  });
  const updateModalStatus = (status: boolean) => {
    setOpen(status);
  };
  return (
    <div
      style={{ outline: 'none' }}
      role="button"
      onKeyDown={() => null}
      onClick={handleClick}
      tabIndex={0}
    >
      <ValueControl value={value} label={label} units={units || ''} />
      <ValueModal
        updateModalStatus={updateModalStatus}
        openModal={open}
        disableSetNewButton={true}
        committedSetting={committedSetting}
        label={label}
        units={units}
        requestCommitSetting={doSetValue}
      />
    </div>
  );
};

export default ControlInfo;
