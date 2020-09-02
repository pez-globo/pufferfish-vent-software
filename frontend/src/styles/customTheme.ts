import { createMuiTheme, ThemeOptions } from '@material-ui/core/styles';

// Override Mui's theme typings to include custom theme properties.
declare module '@material-ui/core/styles/createMuiTheme' {
    interface Theme {
        panel: {
            borderRadius: React.CSSProperties['borderRadius'],
        }
    }
    interface ThemeOptions {
        panel?: {
            borderRadius?: React.CSSProperties['borderRadius']
        }
    }
}

function createCustomTheme(options: ThemeOptions) {
    return createMuiTheme({
        // Put custom theme properties below, but before `...options`.
        panel: {
            borderRadius: 16
        },
        // End custom attributes.
        ...options
    })
}

export const darkTheme = createCustomTheme({
    palette: {
        type: 'dark',
        background: {
            paper: '#0b2e4c',
            default: 'black',
        },
        primary: {
            light: '#254764',
            main: '#0053b3',
            dark: '#124876',
            contrastText: 'white'
        },
        secondary: {
            main: '#4CD964',
        },
        info:{
            main: '#B2C0FC'
        }
    },
    typography: {
        fontFamily: [
            'Roboto',
            'sans-serif',
        ].join(','),
        body1: {
            color: 'white'
        },
        subtitle2: {
            fontSize: 12,
        },
        button: {
            textTransform: 'none'
        }
    },
    mixins: {
        toolbar: {
            minHeight: 80
        }
    },
})

export const lightTheme = createCustomTheme({
    palette: {
        type: 'light',
        background: {
            paper: '#CEDEF0',
            default: 'white',
        },
        primary: {
            light: '#96B3D3',
            main: '#0053b1',
            dark: '#A9C2D8',
            contrastText: 'white'
        },
        secondary: {
            main: '#53d769',
        },
        info:{
            main:'#2c396f'
        }
    },
    typography: {
        fontFamily: [
            'Roboto',
            'sans-serif',
        ].join(','),
        body1:{
            color: 'black'
        },
        subtitle2: {
            fontSize: 12
        },

        button: {
            textTransform: 'none'
        }
    },
    mixins: {
        toolbar: {
            minHeight: 80
        }
    },
})