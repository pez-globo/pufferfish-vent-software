import React, { Component } from 'react';
import { Route, RouteProps } from 'react-router-dom';

const NoLayoutRoute = ({ component: Component, ...rest }: RouteProps): JSX.Element | null => {
  if (!Component) return null;
  return <Route {...rest} render={(matchProps) => <Component {...matchProps} />} />;
};

export default NoLayoutRoute;
