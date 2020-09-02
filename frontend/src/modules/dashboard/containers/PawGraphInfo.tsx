import React from 'react'
import { connect } from 'react-redux'
import { createStructuredSelector } from 'reselect'
import { AutoSizer } from 'react-virtualized'
import { StoreState } from '../../../store/types'
import { WaveformPoint } from '../../../store/controller/types'
import {
  getWaveformPawOld,
  getWaveformPawNew
} from '../../../store/controller/selectors'
import { Waveform } from '../components/Waveform'
import { Axes } from '../components/Axes'


interface DataProps {
  data: WaveformPoint[]
}


const oldSelector = createStructuredSelector<StoreState, DataProps>({
  data: getWaveformPawOld,
})
const newSelector = createStructuredSelector<StoreState, DataProps>({
  data: getWaveformPawNew,
})


interface AutoSizerProps {
  width: number,
  height: number
}


interface WaveformProps extends DataProps, AutoSizerProps {
  strokeWidth: number,
  fill: boolean
}


const PawWaveform = ({ data, width, height, strokeWidth, fill }: WaveformProps) => (
  <Waveform
    type={'paw'}
    fill={fill}
    width={width} height={height} data={data} strokeWidth={strokeWidth}
    xRangeMax={10000} yRangeMin={0} yRangeMax={60}
  />
)


const WaveformOld = connect(oldSelector)(PawWaveform)
const WaveformNew = connect(newSelector)(PawWaveform)

const PawGraphInfo = () => (
  <AutoSizer>
    {({ width, height }: AutoSizerProps) => (
      <Axes
        width={width} height={height}
        waveformOld={<WaveformOld width={width} height={height} strokeWidth={1} fill={false}/>}
        waveformNew={<WaveformNew width={width} height={height} strokeWidth={4} fill={true}/>}
        xRangeMax={10000} yRangeMin={0} yRangeMax={60}
        title={"Paw"} units={"cm H2O"}
      />
    )}
  </AutoSizer>
)

export default PawGraphInfo