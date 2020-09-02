import React from 'react'
import { useLocation, Link } from 'react-router-dom'
import { makeStyles, Theme } from '@material-ui/core/styles'
import { Button, Grid, AppBar } from '@material-ui/core'
import { LOGS_ROUTE, DASHBOARD_ROUTE, QUICKSTART_ROUTE, SCREENSAVER_ROUTE } from '../navigation/constants'
import ModesDropdown from '../modes/ModesDropdown'
import ViewDropdown from '../dashboard/views/ViewDropdown'
import { BackIcon, BellIcon } from '../icons'
import ClockIcon from '../icons/ClockIcon'
import PowerFullIcon from '../icons/PowerFullIcon'
import { PERCENT } from '../info/units'
import { useSelector } from 'react-redux'
import { getClockTime } from '../../store/app/selectors'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        // border: '1px solid red',
    },
    marginRight: {
        marginRight: theme.spacing(2)
    },
    paddingRight: {
        paddingRight: theme.spacing(1)
    },
    toolContainer: {
        height: '100%',
        // border: '1px solid red'
    }
}))

/**
 * ToolBar
 *
 * A container for displaying buttons that handle system changes based on
 * various states and conditions such as ventilator state and current page route.
 */
export const ToolBar = () => {
    const classes = useStyles()
    // Store the route location so we can change button/breadcrumb displays
    // depending on the current route.
    const location = useLocation()
    const [isVentilatorOn, setIsVentilatorOn] = React.useState(false)
    const label = (isVentilatorOn) ? 'Pause Ventilation' : 'Start Ventilation'
    const toPath = (isVentilatorOn) ? QUICKSTART_ROUTE.path : DASHBOARD_ROUTE.path
    const isDisabled = !isVentilatorOn && location.pathname !== QUICKSTART_ROUTE.path
    const StartPauseButton =
        <Button
            component={Link}
            to={toPath}
            onClick={() => setIsVentilatorOn(!isVentilatorOn)}
            variant='contained'
            color='secondary'
            disabled={isDisabled}
        >
            {label}
        </Button>

    let tools = [<ModesDropdown />]
    if (location.pathname === DASHBOARD_ROUTE.path) {
        tools.push(<ViewDropdown />)
        tools.push(
            <div>
                <Button style={{marginRight: 12}} component={Link} to={LOGS_ROUTE.path} variant='contained' color='primary'>
                    <BellIcon />
                </Button>
                {LOGS_ROUTE.label}
            </div>
        )
    } else if (location.pathname === QUICKSTART_ROUTE.path) {
        tools.push(
            <Button variant='contained' color='primary' disabled>
                Last Patient Settings
            </Button>
        )
    } else if (isVentilatorOn && location.pathname !== SCREENSAVER_ROUTE.path) {
        tools.push(
            <Button component={Link} to={DASHBOARD_ROUTE.path} variant='contained' color='primary'>
                <BackIcon style={{paddingRight: 8}} />
                {DASHBOARD_ROUTE.label}
            </Button>
        )
    }
    const clockTime = useSelector(getClockTime)

    return (
        <AppBar
        color='transparent'
        elevation={0}
        position='static'>
            <Grid
                container
                direction='row'
                justify='space-between'
                alignItems='center'
                className={classes.root}
                wrap='nowrap'
            >
                <Grid
                    container
                    item
                    xs={8}
                    direction='row'
                    justify='flex-start'
                    alignItems='center'
                    wrap='nowrap'
                    zeroMinWidth
                    className={classes.toolContainer}
                >
                    {tools.map((tool) => (
                        <Grid item className={classes.marginRight}>
                            {tool}
                        </Grid>
                    ))}
                </Grid>
                <Grid
                    container
                    item
                    xs
                    direction='row'
                    justify='flex-end'
                    alignItems='center'
                    zeroMinWidth
                    className={classes.toolContainer}
                >
                    <Grid
                        container
                        item
                        xs
                        justify='flex-end'
                        alignItems='center'>
                        <span className={classes.paddingRight}>{`100${PERCENT}`}</span>
                        <PowerFullIcon style={{ fontSize: '2.5rem' }} />
                        <span className={classes.paddingRight}>{clockTime}</span>
                        <ClockIcon style={{ fontSize: '2.5rem' }} />
                    </Grid>
                    <Grid item >
                        {StartPauseButton}
                    </Grid>
                </Grid>
            </Grid>
        </AppBar>
    )
}

export default ToolBar