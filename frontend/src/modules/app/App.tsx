import React, { Component } from 'react'
import { connect, useSelector } from 'react-redux'
import { createStructuredSelector } from 'reselect'
import { IntlProvider } from 'react-intl'
import { CssBaseline } from '@material-ui/core'
import { MuiThemeProvider } from '@material-ui/core/styles'
import { StoreState } from '../../store/types'
import { initialize } from '../../store/app/actions'
import { getLocale, getMessages } from '../../store/app/selectors'
import Layout from './Layout'
import { darkTheme, lightTheme } from '../../styles/customTheme'
import { ThemeVariant } from '../../store/controller/proto/mcu_pb'
import { getFrontendDisplaySetting } from '../../store/controller/selectors'

interface Props {
  initialize: typeof initialize,
  locale: string,
  messages: Record<string, string>,
}

class App extends Component<Props> {
  componentDidMount() {
    const { initialize } = this.props
    initialize()
  }

  render() {
    const { locale, messages }: Props = this.props
    const displaySetting = useSelector(getFrontendDisplaySetting)
    const theme = displaySetting.theme === ThemeVariant.dark ? darkTheme : lightTheme
    return (
      <IntlProvider key={locale} locale={locale} messages={messages}>
        <MuiThemeProvider theme={theme}>
          <CssBaseline />
          <Layout />
        </MuiThemeProvider>
      </IntlProvider>
    )
  }
}

const selector = createStructuredSelector<StoreState, { locale: string, messages: Record<string, string> }>({
  locale: getLocale,
  messages: getMessages,
})

const actions = {
  initialize,
}

export default connect(selector, actions)(App)