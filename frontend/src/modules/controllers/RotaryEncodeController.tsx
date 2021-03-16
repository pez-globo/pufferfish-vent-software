import React, { useCallback, useEffect, useRef } from 'react';
import { useSelector, shallowEqual } from 'react-redux';
import { getRotaryEncoder } from '../../store/controller/selectors';

interface Props {
  value: number;
  onClick: (value: number) => void;
  min?: number;
  max?: number;
  isActive: boolean;
}

/**
 * RotaryEncodeController
 *
 * A re-usable "RotaryEncoder" component for adjusting ValueClicker value
 */
export const RotaryEncodeController = ({
  value,
  onClick,
  min = 0,
  max = 100,
  isActive,
}: Props): JSX.Element => {
  const rotaryEncoder = useSelector(getRotaryEncoder, shallowEqual);
  const isInitialMount = useRef(true);

  const updateRotaryData = useCallback(
    () => {
      if (isInitialMount.current) {
        isInitialMount.current = false;
      } else if (!Number.isNaN(rotaryEncoder?.stepDiff)) {
        const stepDiff = rotaryEncoder?.stepDiff || 0;
        const valueClone = value >= min ? value : min;
        const newValue = valueClone + stepDiff;
        if (newValue < min) {
          onClick(min);
        } else if (newValue > max) {
          onClick(max);
        } else {
          onClick(newValue);
        }
        // if (rotaryEncoder.buttonPressed) {
        //   handleConfirm();
        // }
      }
    },
    // eslint-disable-next-line react-hooks/exhaustive-deps
    [rotaryEncoder?.step, rotaryEncoder?.buttonPressed, min, max, value],
  );

  useEffect(() => {
    if (isActive) {
      updateRotaryData();
    }
  }, [updateRotaryData, isActive]);

  return <React.Fragment />;
};

export default RotaryEncodeController;
