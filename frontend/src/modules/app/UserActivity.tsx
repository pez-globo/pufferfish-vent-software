import React, { useCallback, useEffect, useState } from 'react';
import { Redirect, useHistory } from 'react-router-dom';

const IdleTimer = ({ timeout, onTimeOut }: { timeout: number; onTimeOut(): void }): JSX.Element => {
  const idle = useCallback(() => {
    if (onTimeOut) {
      onTimeOut();
    }
  }, [onTimeOut]);

  useEffect(() => {
    let idleTimeout: NodeJS.Timeout;

    const setTimeouts = () => {
      idleTimeout = setTimeout(idle, timeout);
    };

    const clearTimeouts = () => {
      if (idleTimeout) {
        clearTimeout(idleTimeout);
      }
    };

    const events = ['load', 'mousemove', 'mousedown', 'click', 'scroll', 'keypress'];

    const resetTimeout = () => {
      clearTimeouts();
      setTimeouts();
    };

    events.forEach((event: string) => {
      window.addEventListener(event, resetTimeout);
    });

    setTimeouts();
    return () => {
      events.forEach((event: string) => {
        window.removeEventListener(event, resetTimeout);
        clearTimeouts();
      });
    };
  }, [idle, timeout]);
  return <React.Fragment />;
};

export const UserActivity = (): JSX.Element => {
  const [idleTimeout] = useState(10 * 60 * 1000);
  const history = useHistory();
  const onTimeOut = () => {
    history.push('/screensaver');
  };
  return <IdleTimer timeout={idleTimeout} onTimeOut={onTimeOut} />;
};

export default UserActivity;
