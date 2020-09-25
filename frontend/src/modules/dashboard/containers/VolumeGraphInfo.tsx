import React from 'react';
import { connect } from 'react-redux';
import { createStructuredSelector } from 'reselect';
import { AutoSizer } from 'react-virtualized';
import { StoreState } from '../../../store/types';
import { WaveformPoint } from '../../../store/controller/types';
import {
  getWaveformVolumeOld,
  getWaveformVolumeNewSegment,
} from '../../../store/controller/selectors';
import { Waveform } from '../components/Waveform';
import { Axes } from '../components/Axes';

interface DataProps {
  data: WaveformPoint[];
}

const oldSelector = createStructuredSelector<StoreState, DataProps>({
  data: getWaveformVolumeOld,
});
const newSegmentSelector = (segmentIndex: number) =>
  createStructuredSelector<StoreState, DataProps>({
    data: getWaveformVolumeNewSegment(segmentIndex),
  });

interface AutoSizerProps {
  width: number;
  height: number;
}

interface WaveformProps extends DataProps, AutoSizerProps {
  strokeWidth: number;
  fill: boolean;
}

const VolumeWaveform = ({ data, width, height, strokeWidth, fill }: WaveformProps) => (
  <Waveform
    type="volume"
    fill={fill}
    width={width}
    height={height}
    data={data}
    strokeWidth={strokeWidth}
    xRangeMax={10000}
    yRangeMin={0}
    yRangeMax={1000}
  />
);

const WaveformOld = connect(oldSelector)(VolumeWaveform);
const WaveformNew0 = connect(newSegmentSelector(0))(VolumeWaveform);
const WaveformNew1 = connect(newSegmentSelector(1))(VolumeWaveform);
const WaveformNew2 = connect(newSegmentSelector(2))(VolumeWaveform);
const WaveformNew3 = connect(newSegmentSelector(3))(VolumeWaveform);
const waveforms = ({ width, height }: AutoSizerProps) => (
  <React.Fragment>
    <WaveformOld width={width} height={height} strokeWidth={1} fill={false} />,
    <WaveformNew0 width={width} height={height} strokeWidth={4} fill={true} />
    <WaveformNew1 width={width} height={height} strokeWidth={4} fill={true} />
    <WaveformNew2 width={width} height={height} strokeWidth={4} fill={true} />
    <WaveformNew3 width={width} height={height} strokeWidth={4} fill={true} />
  </React.Fragment>
);

const VolumeGraphInfo = (): JSX.Element => (
  <AutoSizer>
    {(props: AutoSizerProps) => {
      const { width, height } = props;
      return (
        <Axes
          width={width}
          height={height}
          waveforms={waveforms({ width, height })}
          xRangeMax={10000}
          yRangeMin={0}
          yRangeMax={1000}
          title="Volume"
          units="mL"
        />
      );
    }}
  </AutoSizer>
);

export default VolumeGraphInfo;
