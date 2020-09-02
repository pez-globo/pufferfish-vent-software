import React from 'react'
import { Grid, Tabs, Tab, Button } from '@material-ui/core/'
import { makeStyles, Theme } from '@material-ui/core/styles'
import { TabPanel, a11yProps } from '../controllers/TabPanel'
import { InfoTab, TestCalibrationTab, DisplayTab } from './containers'
import { FrontendDisplaySetting, SystemSettingRequest } from '../../store/controller/proto/mcu_pb'
import { useDispatch } from 'react-redux'
import { FRONTEND_DISPLAY_SETTINGS, SYSTEM_SETTINGS } from '../../store/controller/types'
import { updateCommittedState } from '../../store/controller/actions'

const useStyles = makeStyles((theme: Theme) => ({
    tabPanelContainer: {
        flexGrow: 1,
        justifyContent: 'space-between',
        alignItems: 'stretch',
        height: '100',
        // border: '1px solid green',
    },
    tabContainer: {
        minHeight: 80,
        // border: '1px solid red',
    },
    tabs: {
        // paddingTop: theme.spacing(1) 
        // border: '1px solid red',
        width: '100%',
    },
    tab: {
        borderRadius: 8,
        border: '2px solid ' + theme.palette.primary.main,
        margin: theme.spacing(1),
        zIndex: 1,
        minHeight: 40,
        minWidth: 220,
    },
    selectedTab:{color:theme.palette.primary.contrastText},
    tabIndicator: {
        borderRadius: 8,
        border: '2px solid ' + theme.palette.primary.main,
        zIndex: 0,
        marginBottom: theme.spacing(1),
        minHeight: 40,
        background: theme.palette.primary.main
    },
}))

enum TabType {
    INFO_TAB,
    TEST_CALIBRATION_TAB,
    DISPLAY_TAB
}
/** 
 * SettingsPage
 * 
 * A container storing various setting panels.
 */
export const SettingsPage = () => {
    const classes = useStyles()
    const [value, setValue] = React.useState(0)
    const dispatch = useDispatch()
    const [displaySetting, setDisplaySetting] = React.useState<FrontendDisplaySetting>()
    const [systemSetting, setSystemSetting] = React.useState<SystemSettingRequest>()

    const handleChange = (event: React.ChangeEvent<{}>, newValue: number) => {
        setValue(newValue)
    }

    const onSettingChange = (settings: any) => {
        setDisplaySetting(FrontendDisplaySetting.fromJSON({
            theme: settings.theme,
            unit: settings.unit,
        }))
        setSystemSetting(SystemSettingRequest.fromJSON({
            brightness: settings.brightness,
            date: settings.date,
        }))
    }

    const handleSubmit = () => {
        dispatch(updateCommittedState(FRONTEND_DISPLAY_SETTINGS, displaySetting as Object))
        dispatch(updateCommittedState(SYSTEM_SETTINGS, systemSetting as Object))
    }

    return (
        <Grid container direction='column'>
            <Grid container className={classes.tabPanelContainer}>
                <TabPanel value={value} index={TabType.INFO_TAB}>
                    <InfoTab />
                </TabPanel>
                <TabPanel value={value} index={TabType.TEST_CALIBRATION_TAB}>
                    <TestCalibrationTab />
                </TabPanel>
                <TabPanel value={value} index={TabType.DISPLAY_TAB}>
                    <DisplayTab onSettingChange={onSettingChange}/>
                </TabPanel>
            </Grid>
            <Grid container item justify='center' alignItems='center' className={classes.tabContainer}>
                <Tabs value={value} onChange={handleChange} className={classes.tabs} classes={{ indicator: classes.tabIndicator }}>
                    <Tab label='Info' {...a11yProps(0)} className={classes.tab}
                        classes={{ selected: classes.selectedTab }} />
                    <Tab label='Test & Calibration' {...a11yProps(1)} className={classes.tab}
                        classes={{ selected: classes.selectedTab }} />
                    <Tab label='Display' {...a11yProps(2)} className={classes.tab}
                        classes={{ selected: classes.selectedTab }} />
                        <Grid container item justify='flex-end' alignItems='center'>
                            {value === TabType.DISPLAY_TAB && <Button onClick={() => handleSubmit()} variant='contained' color='secondary'>
                                    Apply Changess
                                </Button>}
                        </Grid>
                </Tabs>
            </Grid>
        </Grid>
    )
}

export default SettingsPage