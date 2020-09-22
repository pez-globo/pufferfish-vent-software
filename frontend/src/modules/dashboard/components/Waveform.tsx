import React from 'react';
import { useTheme, Theme } from '@material-ui/core';
import { fade } from '@material-ui/core/styles/colorManipulator';
import { Grid } from '@vx/grid';
import { curveLinear } from '@vx/curve';
import { LinePath, AreaClosed } from '@vx/shape';
import { Group } from '@vx/group';
import { LinearGradient } from '@vx/gradient';
import { scaleTime, scaleLinear } from '@vx/scale';
import { Axes } from './Axes';

interface DateValue {
  date: Date;
  value: number;
}

// accessors
const x = ({ date }: DateValue) => date;
const y = ({ value }: DateValue) => value;

export interface Props {
  width: number;
  height: number;
  data: DateValue[];
  margin: {
    left: number;
    top: number;
    bottom: number;
    right: number;
  };
  strokeWidth: number;
  fill: boolean;
  xRangeMin: number;
  xRangeMax: number;
  yRangeMin: number;
  yRangeMax: number;
  type: string;
}

export const Waveform = ({
  width,
  height,
  margin,
  data,
  strokeWidth,
  fill,
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
    domain: [new Date(xRangeMin), new Date(xRangeMax)],
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
  const yScale2 = scaleLinear({
    domain: [yRangeMin, yRangeMax],
    range: [(height - margin.bottom + margin.top) / 2, 0],
  });
  function findAxis(type: string) {
    if (type === 'flow') return yScale2;
    if (type === 'paw' || type === 'volume') return yScale1;
    return yScale;
  }
  function fillF(fill: boolean) {
    if (fill) return fade(theme.palette.info.main, 0.5);
    return 'rgba(0,0,0,0)';
  }

  return (
    <svg width={width} height={height}>
      <LinearGradient
        from={theme.palette.info.main}
        to={theme.palette.info.main}
        toOpacity={0}
        id="gradient"
      />
      <Group>
        <AreaClosed
          data={data}
          x={(d) => xScale(x(d)) + margin.left}
          y={(d) => yScale(y(d)) + margin.top}
          yScale={findAxis(type)}
          fill={fillF(fill)}
          strokeWidth={strokeWidth}
          curve={curveLinear}
        />
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

Waveform.defaultProps = Axes.defaultProps;
