import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { AutoSizer } from 'react-virtualized';
import { StoreState } from '../../../store/types';
import { WaveformPoint } from '../../../store/controller/types';
import { getWaveformFlowOld, getWaveformFlowNewSegment } from '../../../store/controller/selectors';
import { Waveform } from '../components/Waveform';
import { Axes } from '../components/Axes';

interface DataProps {
  data: WaveformPoint[];
}

const oldSelector = createStructuredSelector<StoreState, DataProps>({
  data: getWaveformFlowOld,
});
const newSegmentSelector = (segmentIndex: number) =>
  createStructuredSelector<StoreState, DataProps>({
    data: getWaveformFlowNewSegment(segmentIndex),
  });

interface AutoSizerProps {
  width: number;
  height: number;
}

interface WaveformProps extends DataProps, AutoSizerProps {
  strokeWidth: number;
  fill: boolean;
}

const FlowWaveform = ({ data, width, height, strokeWidth, fill }: WaveformProps) => (
  <Waveform
    type="flow"
    fill={fill}
    width={width}
    height={height}
    data={data}
    strokeWidth={strokeWidth}
    xRangeMax={10000}
    yRangeMin={-150}
    yRangeMax={150}
  />
);

const WaveformOld = connect(oldSelector)(FlowWaveform);
const WaveformNew0 = connect(newSegmentSelector(0))(FlowWaveform);
const WaveformNew1 = connect(newSegmentSelector(1))(FlowWaveform);
const WaveformNew2 = connect(newSegmentSelector(2))(FlowWaveform);
const WaveformNew3 = connect(newSegmentSelector(3))(FlowWaveform);
const waveforms = ({ width, height }: AutoSizerProps) => (
  <React.Fragment>
    <WaveformOld width={width} height={height} strokeWidth={1} fill={false} />,
    <WaveformNew0 width={width} height={height} strokeWidth={4} fill={true} />
    <WaveformNew1 width={width} height={height} strokeWidth={4} fill={true} />
    <WaveformNew2 width={width} height={height} strokeWidth={4} fill={true} />
    <WaveformNew3 width={width} height={height} strokeWidth={4} fill={true} />
  </React.Fragment>
);

const FlowGraphInfo = (): JSX.Element => (
  <AutoSizer>
    {(props: AutoSizerProps) => {
      const { width, height } = props;
      return (
        <Axes
          width={width}
          height={height}
          waveforms={waveforms({ width, height })}
          xRangeMax={10000}
          yRangeMin={-150}
          yRangeMax={150}
          title="Flow"
          units="L/min"
        />
      );
    }}
  </AutoSizer>
);

export default FlowGraphInfo;
