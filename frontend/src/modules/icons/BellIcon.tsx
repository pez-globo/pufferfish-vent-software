import React from 'react';
import SvgIcon, { SvgIconProps } from '@material-ui/core/SvgIcon';

const BellIcon = (props: SvgIconProps): JSX.Element => {
  return (
    <SvgIcon {...props} width="16.47" height="20" viewBox="0 0 16.47 20">
      <path
        id="Path_979"
        data-name="Path 979"
        d="M14.207,34.207A2.431,2.431,0,0,0,16.414,32H12A2.431,2.431,0,0,0,14.207,34.207Z"
        transform="translate(-5.972 -14.207)"
        fillRule="evenodd"
      />
      <path
        id="Path_980"
        data-name="Path 980"
        d="M15.613,7.593c0-2.049-1.886-2.915-4.2-3.14V2.6a.638.638,0,0,0-.672-.6H9.731a.638.638,0,0,0-.672.6V4.453c-2.316.225-4.2,1.091-4.2,3.14C4.857,16.78,2,16.37,2,17.587v1.1H18.47v-1.1C18.47,16.3,15.613,16.78,15.613,7.593Z"
        transform="translate(-2 -1.998)"
        fillRule="evenodd"
      />
    </SvgIcon>
  );
};

export default BellIcon;
