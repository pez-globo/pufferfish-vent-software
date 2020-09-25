import React from 'react';
import Grid from '@material-ui/core/Grid';
import { makeStyles, Theme } from '@material-ui/core/styles';

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    // flexGrow: 1,
    height: '78%',
    width: '100%',
    paddingLeft: 20,
    paddingRight: 20,
  },
}));

interface Props {
  children?: React.ReactNode;
  index: number;
  value: number;
}

export const a11yProps = (index: number): Record<string, unknown> => {
  return {
    id: `tab-${index}`,
    'aria-controls': `tabpanel-${index}`,
  };
};

export const TabPanel = (props: Props): JSX.Element => {
  const { children = null, value, index, ...other } = props;
  const classes = useStyles();

  return (
    <div
      role="tabpanel"
      hidden={value !== index}
      id={`tabpanel-${index}`}
      aria-labelledby={`tab-${index}`}
      {...other}
      className={classes.root}
    >
      {value === index && (
        <Grid
          container
          direction="row"
          alignItems="stretch"
          wrap="nowrap"
          style={{ height: '100%' }}
        >
          {children}
        </Grid>
      )}
    </div>
  );
};

export default TabPanel;
