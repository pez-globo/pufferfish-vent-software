import React from 'react'
import Grid from '@material-ui/core/Grid'
import { makeStyles, Theme } from '@material-ui/core/styles'

const useStyles = makeStyles((theme: Theme) => ({
    root: {
        //flexGrow: 1,
        height:'78%',
        width:'100%',
        paddingLeft: 20,
        paddingRight:20
    }
}))

interface Props {
    children?: React.ReactNode
    index: any
    value: any
}

export const a11yProps = (index: any) => {
    return {
        id: `tab-${index}`,
        'aria-controls': `tabpanel-${index}`,
    }
}

export const TabPanel = (props: Props) => {
    const { children, value, index, ...other } = props
    const classes = useStyles()

    return (
        <div
            role='tabpanel'
            hidden={value !== index}
            id={`tabpanel-${index}`}
            aria-labelledby={`tab-${index}`}
            {...other}
            className={classes.root}
        >
            {value === index && (
                <Grid
                    container
                    direction='row'
                    alignItems='stretch'
                    wrap='nowrap'
                    style={{ height: '100%' }}
                >
                    {children}
                </Grid>
            )}
        </div>
    )
}

export default TabPanel