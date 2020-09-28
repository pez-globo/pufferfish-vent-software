import React, { useEffect } from 'react';
import { makeStyles, Theme, Grid, Button, Typography } from '@material-ui/core';
import { useSelector } from 'react-redux';
import ValueClicker from './ValueClicker';
import ModalPopup from './ModalPopup';
import { getRotaryEncoder } from '../../store/controller/selectors';

const useStyles = makeStyles((theme: Theme) => ({
  contentContainer: {
    height: '100%',
    minHeight: '300px',
    border: `2px dashed ${theme.palette.background.default}`,
    borderRadius: theme.panel.borderRadius,
    marginRight: theme.spacing(2),
    paddingTop: theme.spacing(2),
    paddingLeft: theme.spacing(2),
    paddingRight: theme.spacing(4),
  },
  borderBottom: {
    borderBottom: `2px dashed ${theme.palette.background.default}`,
  },
  openButton: {
    width: '100%',
    border: `1px solid ${theme.palette.common.black}`,
  },
  unitsLabel: {
    paddingTop: theme.spacing(4),
    paddingLeft: theme.spacing(2),
    opacity: 0.8,
    // border: '1px solid red'
  },
}));

export interface SettingAdjustProps {
  committedSetting: number;
}

interface Props {
  label: string;
  units: string;
  committedSetting: number;
  disableSetNewButton?: boolean;
  requestCommitSetting(setting: number): void;
}

export const ValueModal = ({
  label,
  units,
  committedSetting,
  disableSetNewButton = false,
  requestCommitSetting,
}: Props): JSX.Element => {
  const classes = useStyles();
  const rotaryEncoder = useSelector(getRotaryEncoder);
  const [open, setOpen] = React.useState(false);
  const [value, setValue] = React.useState(committedSetting);

  const handleOpen = () => {
    setOpen(true);
  };

  const handleClose = () => {
    setOpen(false);
  };

  const handleConfirm = () => {
    requestCommitSetting(value);
    setOpen(false);
  };

  const updateRotaryData = () => {
    if (open) {
      if (rotaryEncoder.stepDiff) {
        const newValue = value + rotaryEncoder.stepDiff;
        // TODO: Replace 0/100 with respective min/max value
        if (newValue < 0) {
          setValue(0);
        } else if (newValue > 100) {
          setValue(100);
        } else {
          setValue(newValue);
        }
      }
      if (rotaryEncoder.buttonPressed) {
        handleConfirm();
      }
    }
  };

  useEffect(() => {
    updateRotaryData();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [rotaryEncoder.step, rotaryEncoder.buttonPressed]);

  function pipClarify(label: string) {
    if (label === 'PIP') return '*not PEEP compensated';
    return '';
  }

  return (
    <Grid container direction="column" alignItems="center" justify="center">
      <Grid container item xs>
        {!disableSetNewButton && (
          <Button
            onClick={handleOpen}
            color="primary"
            variant="contained"
            className={classes.openButton}
          >
            Set New
          </Button>
        )}
      </Grid>
      <ModalPopup
        withAction={true}
        label="Set New"
        open={open}
        onClose={handleClose}
        onConfirm={handleConfirm}
      >
        <Grid container direction="row">
          <Grid container item xs direction="column" className={classes.contentContainer}>
            <Grid item>
              <Typography variant="h4">
                {label}
                <Typography variant="h6" style={{ opacity: 0.8 }}>
                  {pipClarify(label)}
                </Typography>
              </Typography>
            </Grid>
            <Grid container item xs wrap="nowrap">
              <Grid container item alignItems="baseline">
                <Typography align="left" style={{ fontSize: '9.5rem' }}>
                  {value.toFixed(0)}
                </Typography>
                <Typography align="center" variant="h5" className={classes.unitsLabel}>
                  {units}
                </Typography>
              </Grid>
            </Grid>
          </Grid>
          <Grid item>
            <ValueClicker value={value} min={0} max={100} onClick={setValue} />
          </Grid>
        </Grid>
      </ModalPopup>
    </Grid>
  );
};

export default ValueModal;
