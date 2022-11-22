idf_component_register(SRCS "OpenWeatherMap.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp-tls json SettingsConfig)

project (OpenWeatherMap)                    