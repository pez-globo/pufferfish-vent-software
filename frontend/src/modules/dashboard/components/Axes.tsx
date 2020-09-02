import React from 'react'
import { useTheme, Theme } from '@material-ui/core'
import { Grid } from '@vx/grid'
import { Group } from '@vx/group'
import { AxisLeft, AxisRight, AxisBottom } from '@vx/axis'
import { Line } from '@vx/shape'
import { scaleTime, scaleLinear } from '@vx/scale'
import { GridRows, GridColumns } from '@vx/grid'

// responsive utils for axis ticks
function numTicksForHeight(height: number) {
  if (height <= 300) return 3
  if (300 < height && height <= 600) return 5
  return 10
}

function numTicksForWidth(width: number) {
  if (width <= 300) return 2
  if (300 < width && width <= 400) return 5
  return 10
}

export interface Props {
  width: number,
  height: number,
  waveformOld: React.ReactNode,
  waveformNew: React.ReactNode,
  margin: {
    left: number,
    top: number,
    bottom: number,
    right: number
  },
  xRangeMin: number,
  xRangeMax: number,
  yRangeMin: number,
  yRangeMax: number,
  title: string,
  units: string
}

export const Axes = ({
  width, height, margin, waveformOld, waveformNew,
  xRangeMin, xRangeMax, yRangeMin, yRangeMax, title, units
}: Props) => {

  const theme = useTheme()

  // bounds
  const xMax = width - margin.left - margin.right
  const yMax = height - margin.top - margin.bottom

  // scales
  const xScale = scaleTime({
    domain: [new Date(xRangeMin), new Date(xRangeMax)],
    range: [0, xMax]
  })
  const yScale = scaleLinear({
    domain: [yRangeMin, yRangeMax],
    range: [yMax, 0],
    nice: true
  })

  return (
    <svg width={width} height={height}>
      <text x={margin.left/3} y={margin.top/2}
      fill={theme.typography.body1.color}
      fontSize={28}>
        {title}
      </text>
      <text x={margin.left/3+80} y={margin.top/2} 
      fill={theme.typography.body1.color}
      opacity={0.8}
      fontSize={18} >
        -   {units}
      </text>
      <Grid
        top={margin.top}
        left={margin.left}
        xScale={xScale}
        yScale={yScale}
        stroke={theme.typography.body1.color}
        strokeOpacity={0.21}
        width={xMax}
        height={yMax}
        numTicksRows={numTicksForHeight(height)}
        numTicksColumns={numTicksForWidth(width)}
      />
      <Group>
        {waveformOld}
        {waveformNew}
      </Group>
      <Group left={margin.left}>
        <GridRows scale={yScale} width={xMax} height={yMax} stroke='rgba(255,255,255,0)' />
        <GridColumns scale={xScale} width={xMax} height={yMax} stroke='rgba(255,255,255,0)' />
        <AxisLeft
          top={margin.top}
          left={0}
          scale={yScale}
          numTicks={numTicksForHeight(height)}
          label='Axis Left Label'
          labelProps={{
            fill:'rgba(255,255,255,0)',
            textAnchor: 'middle',
            fontSize: 12,
          }}
          stroke='rgba(255,255,255,0)'
          tickStroke='rgba(255,255,255,0)'
          tickLabelProps={() => ({
            fill:theme.typography.body1.color,
            opacity:0.5,
            textAnchor: 'end',
            fontSize: 10,
            dx: '-0.25em',
            dy: '0.25em'
          })}
          tickComponent={({ formattedValue, ...tickProps }) => (
            <text {...tickProps}>{formattedValue}</text>
          )}
        />
        <AxisBottom
          top={height - margin.bottom}
          left={0}
          scale={xScale}
          numTicks={numTicksForWidth(width)}
          tickLabelProps={() => ({
            fill:theme.typography.body1.color,
            opacity:0.5,
            fontSize: 14,
            textAnchor: 'start',
          })}
          label='Time'
        >
          {axis => {
            const tickLabelSize = 12
            const tickRotate = 0
            const tickColor = theme.typography.body1.color
            const axisCenter = (axis.axisToPoint.x - axis.axisFromPoint.x) / 2
            return (
              <g className='my-custom-bottom-axis'>
                {axis.ticks.map((tick, i) => {
                  const tickX = tick.to.x
                  const tickY = tick.to.y + tickLabelSize + axis.tickLength
                  return (
                    <Group key={`vx-tick-${tick.value}-${i}`} className={'vx-axis-tick'}>
                      <Line from={tick.from} to={tick.to}/>
                      <text
                        transform={`translate(${tickX}, ${tickY}) rotate(${tickRotate})`}
                        fontSize={tickLabelSize}
                        textAnchor='middle'
                        fill={tickColor}
                        opacity={0.5}
                      >
                        {tick.formattedValue}
                      </text>
                    </Group>
                  )
                })}
                <text textAnchor='middle' transform={`translate(${axisCenter}, 50)`} fontSize='8'>
                  {axis.label}
                </text>
              </g>
            )
          }}
        </AxisBottom>
      </Group>
    </svg>
  )
}

Axes.defaultProps = {
  margin: {
    top: 45,
    bottom: 40,
    left: 40,
    right: 10
  },
  xRangeMin: 0
}
