import React from 'react';
import { useTheme, Theme } from '@material-ui/core';
import { fade } from '@material-ui/core/styles/colorManipulator';
import { Grid } from '@vx/grid';
import { curveLinear } from '@vx/curve';
import { LinePath } from '@vx/shape';
import { Group } from '@vx/group';
import { LinearGradient } from '@vx/gradient';
import { scaleTime, scaleLinear } from '@vx/scale';
import { Axes } from './Axes';

export interface Point {
  x: number;
  y: number;
}

// accessors
const x = ({ x }: Point) => x;
const y = ({ y }: Point) => y;

export interface Props {
  width: number;
  height: number;
  data: Point[];
  margin: {
    left: number;
    top: number;
    bottom: number;
    right: number;
  };
  strokeWidth: number;
  xRangeMin: number;
  xRangeMax: number;
  yRangeMin: number;
  yRangeMax: number;
  type: string;
}

export const Loop = ({
  width,
  height,
  margin,
  data,
  strokeWidth,
  xRangeMin,
  xRangeMax,
  yRangeMin,
  yRangeMax,
  type,
}: Props): JSX.Element => {
  const theme = useTheme();

  // bounds
  const xMax = width - margin.left - margin.right;
  const yMax = height - margin.top - margin.bottom;

  // scales
  const xScale = scaleTime({
    domain: [xRangeMin, xRangeMax],
    range: [0, xMax],
  });
  const yScale = scaleLinear({
    domain: [yRangeMin, yRangeMax],
    range: [yMax, 0],
    nice: true,
  });
  const yScale1 = scaleLinear({
    domain: [yRangeMin, yRangeMax],
    range: [height - margin.bottom, 0],
  });
  function findAxis(type: string) {
    if (type === 'pv') return yScale1;
    return yScale;
  }

  return (
    <svg width={width} height={height}>
      <Group>
        <LinePath
          data={data}
          x={(d) => xScale(x(d)) + margin.left}
          y={(d) => yScale(y(d)) + margin.top}
          stroke={theme.palette.info.main}
          strokeWidth={strokeWidth}
          curve={curveLinear}
        />
      </Group>
    </svg>
  );
};

Loop.defaultProps = Axes.defaultProps;
