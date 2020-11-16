import React from 'react';
import { withStyles } from '@material-ui/core/styles';
import { Button, Menu, MenuProps, MenuItem, MenuList, makeStyles, Theme } from '@material-ui/core';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import FiberManualRecord from '@material-ui/icons/FiberManualRecord';
import ArrowDropDown from '@material-ui/icons/ArrowDropDown';

const views = ['Main View', 'Data View', 'Waveform View'];

const useStyles = makeStyles((theme: Theme) => ({
  button: {
    backgroundColor: theme.palette.primary.main,
    color: '#FFF',
    width: 180,
    '&:hover': {
      backgroundColor: theme.palette.primary.main,
    },
  },
  text: {
    marginRight: -25,
  },
  arrow: {
    marginLeft: 10,
    marginRight: -50,
    color: 'white',
  },
}));

const StyledMenu = withStyles((theme: Theme) => ({
  paper: {
    backgroundColor: theme.palette.primary.main,
    marginTop: '5px',
    minWidth: 180,
  },
}))((props: MenuProps) => (
  <Menu
    elevation={0}
    getContentAnchorEl={null}
    anchorOrigin={{
      vertical: 'bottom',
      horizontal: 'center',
    }}
    transformOrigin={{
      vertical: 'top',
      horizontal: 'center',
    }}
    {...props}
  />
));

const StyledMenuItem = withStyles((theme) => ({
  root: {
    borderTop: '1px solid black',
    '& .MuiListItemIcon-root, & .MuiListItemText-primary': {
      color: theme.palette.primary.main,
    },
    color: 'white',
    '&:focus': {
      '& .MuiListItemIcon-root, & .MuiListItemText-primary': {
        color: theme.palette.secondary.main,
      },
    },
  },
}))(MenuItem);

/**
 * ViewDropdown
 *
 * TODO: This component may rely on a redux state that stores the current
 *       view.
 */
export const ViewDropdown = (): JSX.Element => {
  const classes = useStyles();
  const [anchorEl, setAnchorEl] = React.useState<null | HTMLElement>(null);
  const [selectedIndex, setSelectedIndex] = React.useState(0);

  const handleClick = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget);
  };

  const handleItemClick = (event: React.MouseEvent<HTMLElement, MouseEvent>, index: number) => {
    setSelectedIndex(index);
    handleClose();
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  function addBorder(view: number) {
    if (view > 0) return { borderTop: '1px solid black' };
    if (view === 0) return { borderTop: '0px solid black' };
    return {};
  }

  return (
    <div>
      {/* <Button
        className={classes.button}
        disableElevation
        aria-haspopup="true"
        onClick={handleClick}
      >
        {views[selectedIndex]}
        <ListItemIcon className={classes.arrow}>
          <ArrowDropDown fontSize="default" />
        </ListItemIcon>
      </Button> */}
      <StyledMenu
        id="simple-menu"
        anchorEl={anchorEl}
        keepMounted
        open={Boolean(anchorEl)}
        onClose={handleClose}
      >
        {views.map((view, index) => (
          <StyledMenuItem
            key={view}
            selected={index === selectedIndex}
            onClick={(event) => handleItemClick(event, index)}
            style={addBorder(index)}
          >
            <ListItemIcon className={classes.text}>
              <FiberManualRecord fontSize="inherit" />
            </ListItemIcon>
            {view}
          </StyledMenuItem>
        ))}
      </StyledMenu>
    </div>
  );
};

export default ViewDropdown;
