import React from 'react';
import SvgIcon, { SvgIconProps } from '@material-ui/core/SvgIcon';

const TrendUpIcon = (props: SvgIconProps): JSX.Element => {
  return (
    <SvgIcon {...props} width="44" height="44" viewBox="0 0 44 44">
      <svg xmlns="http://www.w3.org/2000/svg" width="44" height="44" viewBox="0 0 44 44">
        <g id="Group_1063" data-name="Group 1063" transform="translate(-244.184 -617.502)">
          <g id="Icon" transform="translate(244 617.502)">
            <g
              id="Icon.BG"
              transform="translate(0.184)"
              fill="none"
              stroke="#4cd964"
              strokeWidth="3"
            >
              <circle cx="22" cy="22" r="22" stroke="none" />
              <circle cx="22" cy="22" r="20.5" fill="none" />
            </g>
          </g>
          <g
            id="Group_835"
            data-name="Group 835"
            transform="matrix(0.998, 0.07, -0.07, 0.998, 258.357, 633.983)"
          >
            <path
              id="Path_2081"
              data-name="Path 2081"
              d="M0,11.109,5.406,4.868,8.447,7.416,14.638,0"
              fill="none"
              stroke="#4cd964"
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth="3"
            />
            <path
              id="Path_2082"
              data-name="Path 2082"
              d="M0,.269,5.35,0l.689,4.948"
              transform="translate(9.368 0)"
              fill="none"
              stroke="#4cd964"
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth="3"
            />
          </g>
        </g>
      </svg>
    </SvgIcon>
  );
};

export default TrendUpIcon;
