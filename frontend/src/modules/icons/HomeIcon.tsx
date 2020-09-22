import React from 'react';
import SvgIcon, { SvgIconProps } from '@material-ui/core/SvgIcon';
import { makeStyles, Theme } from '@material-ui/core/styles';

const useStyles = makeStyles((theme: Theme) => ({
  path: {
    stroke: theme.palette.text.primary,
  },
}));

const HomeIcon = (props: SvgIconProps): JSX.Element => {
  const classes = useStyles();
  return (
    <SvgIcon {...props} width="30.467" height="28.948" viewBox="0 0 30.467 28.948">
      <path
        id="Path_2121"
        data-name="Path 2121"
        d="M7454.993-976.127l-14.182,13.279h4.642v13.669h19.387v-13.669h4.438l-4.641-4.314Z"
        transform="translate(-7439.812 977.127)"
        fill="none"
        className={classes.path}
        strokeLinecap="round"
        strokeLinejoin="round"
        strokeWidth="2"
      />
    </SvgIcon>
  );
};

export default HomeIcon;
