/**
 * plugins/vuetify.js
 *
 * Framework documentation: https://vuetifyjs.com`
 */

// Styles
import '@mdi/font/css/materialdesignicons.css'
import 'vuetify/styles'

// Composables
import { createVuetify } from 'vuetify'

const lightMode = {
  dark: false,
  colors: {
    primary : "#006064",
    onPrimary : "#FFFFFF",
    primaryContainer : "#80CBC4",
    onPrimaryContainer : "#F5F5F5",
    secondary : "#0060AC",
    onSecondary : "#FFFFFF",
    secondaryContainer : "#D4E3FF",
    onSecondaryContainer : "#001C39",
    tertiary : "#006064",
    onTertiary : "#FFFFFF",
    tertiaryContainer : "#F48FB1",
    onTertiaryContainer : "#F5F5F5",
    error : "#BA1A1A",
    errorContainer : "#FFDAD6",
    onError : "#FFFFFF",
    onErrorContainer : "#410002",
    background : "#F5F5F5", // #FEFBFF
    onBackground : "#1B1B1F",
    surface : "#B2DFDB", // #F0F0F0  , #FFFBFE
    onSurface : "#1B1B1F",
    surfaceVariant : "#E1E2EC",
    onSurfaceVariant : "#44474F",
    outline : "#75777F",
    inverseOnSurface : "#F2F0F4",
    inverseSurface : "#303034",
    inversePrimary : "#AEC6FF",
    shadow : "#000000",
    surfaceTint : "#335CA8",
    outlineVariant : "#C5C6D0",
    scrim : "#000000"
  },
}

const darkMode = {
  dark: true,
  colors: {
    home  : "#FFFFFF",
    primary : "#9CCC65",
    onPrimary : "#000000",
    primaryContainer : "#9CCC65",
    onPrimaryContainer : "#000000",
    secondary : "#B2DFDB",
    onSecondary : "#00315D",
    secondaryContainer : "#D4E3FF",
    onSecondaryContainer : "#FFFFFF",
    tertiary : "#DFBBDE",
    onTertiary : "#402843",
    tertiaryContainer : "#B2DFDB",
    onTertiaryContainer : "#000000",
    error : "#FFB4AB",
    errorContainer : "#93000A",
    onError : "#690005",
    onErrorContainer : "#FFDAD6",
    background : "#121212",//"#152028", //"#232B32",//"#121212", // "#1B1B1F"
    onBackground : "#E3E2E6",
    surface : "#1B1B1F",
    onSurface : "#E3E2E6",
    surfaceVariant : "#44474F",
    onSurfaceVariant : "#C5C6D0",
    outline : "#8E9099",
    inverseOnSurface : "#1B1B1F",
    inverseSurface : "#E3E2E6",
    inversePrimary : "#335CA8",
    shadow : "#000000",
    surfaceTint : "#AEC6FF",
    outlineVariant : "#44474F",
    scrim : "#000000"
  },
}


// https://vuetifyjs.com/en/introduction/why-vuetify/#feature-guides
export default createVuetify({
  theme: {
    defaultTheme: 'lightMode',
    themes: {
      lightMode,
      darkMode 
    },
  } 
})
