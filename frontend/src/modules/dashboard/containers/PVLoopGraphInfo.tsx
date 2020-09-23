import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { AutoSizer } from 'react-virtualized';
import { StoreState } from '../../../store/types';
import { PVPoint } from '../../../store/controller/types';
import { getPVLoop } from '../../../store/controller/selectors';
import { Loop, Point } from '../components/Loop';
import { Axes } from '../components/Axes';

interface DataProps {
  data: PVPoint[];
}

const selector = createStructuredSelector<StoreState, DataProps>({
  data: getPVLoop,
});

interface AutoSizerProps {
  width: number;
  height: number;
}

interface WaveformProps extends DataProps, AutoSizerProps {
  strokeWidth: number;
}

const pointMapper = (data: PVPoint): Point => ({
  x: data.pressure,
  y: data.volume,
});

const PVLoop = ({ data, width, height, strokeWidth }: WaveformProps) => (
  <Loop
    type="pv"
    width={width}
    height={height}
    data={data.map(pointMapper)}
    strokeWidth={strokeWidth}
    xRangeMin={0}
    xRangeMax={60}
    yRangeMin={0}
    yRangeMax={1000}
  />
);

const CurrentLoop = connect(selector)(PVLoop);
const loops = ({ width, height }: AutoSizerProps) => (
  <React.Fragment>
    <CurrentLoop width={width} height={height} strokeWidth={4} />,
  </React.Fragment>
);

const PVLoopGraphInfo = (): JSX.Element => (
  <AutoSizer>
    {(props: AutoSizerProps) => {
      const { width, height } = props;
      return (
        <Axes
          width={width}
          height={height}
          waveforms={loops({ width, height })}
          xRangeMax={10000}
          yRangeMin={0}
          yRangeMax={60}
          title="PV"
          units="cm H2O"
        />
      );
    }}
  </AutoSizer>
);

export default PVLoopGraphInfo;
