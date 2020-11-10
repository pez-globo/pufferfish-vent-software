import React, { PropsWithChildren } from 'react';
import { Route, RouteProps } from 'react-router-dom';
import Sidebar from '../Sidebar';

const ScreensaverLayout = ({ children, ...rest }: PropsWithChildren<unknown>): JSX.Element => {
  return (
    <React.Fragment>
      <Sidebar />
      {children}
    </React.Fragment>
  );
};

const ScreensaverRoute = ({ component: Component, ...rest }: RouteProps): JSX.Element | null => {
  if (!Component) return null;
  return (
    <Route
      {...rest}
      render={(matchProps) => (
        <ScreensaverLayout>
          <Component {...matchProps} />
        </ScreensaverLayout>
      )}
    />
  );
};

export default ScreensaverRoute;
