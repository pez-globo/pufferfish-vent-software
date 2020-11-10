import React, { useCallback, useEffect } from 'react';
import { Button, Grid, Typography } from '@material-ui/core';
import { Link } from 'react-router-dom';
import { QUICKSTART_ROUTE } from '../navigation/constants';

export const LandingPage = (): JSX.Element => {
  return (
    <div
      style={{
        width: '100%',
        position: 'relative',
      }}
    >
      <img
        alt="Landing Page"
        style={{
          width: '100%',
          height: '100%',
          position: 'absolute',
          top: 0,
          bottom: 0,
          left: 0,
        }}
        src={`${process.env.PUBLIC_URL}/home.png`}
      />
      <Button
        style={{
          position: 'absolute',
          bottom: '30px',
          right: 0,
          marginRight: '20px',
        }}
        component={Link}
        to={QUICKSTART_ROUTE.path}
        variant="contained"
        color="primary"
      >
        Ventilation
      </Button>
    </div>
  );
};

export default LandingPage;
