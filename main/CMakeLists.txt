idf_component_register(SRCS "main.cpp" 
					"widgets/tux_panel.c" 
					"images/dev_bg.c"

					# Status icons like BLE
					"fonts/font_fa_14.c" 

					# Weather icons like clouds etc
					# "fonts/font_fa_weather_32.c"
					"fonts/font_fa_weather_42.c"
					# "fonts/font_fa_weather_48.c"
					# "fonts/font_fa_weather_56.c"
					# "fonts/font_fa_weather_64.c"

					# Mototype font used in device info
					# "fonts/font_robotomono_12.c"
					"fonts/font_robotomono_13.c"

					# 7Seg font used for time
					# "fonts/font_7seg_64.c"
					# "fonts/font_7seg_60.c"
					# "fonts/font_7seg_58.c"
					"fonts/font_7seg_56.c"

                INCLUDE_DIRS "." "devices" "helpers" "pages" "widgets" "images"
				REQUIRES LovyanGFX lvgl fatfs fmt SettingsConfig OpenWeatherMap
				#EMBED_TXTFILES ../server_certs/ca_cert.pem
				)

spiffs_create_partition_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT)
#fatfs_create_spiflash_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT PRESERVE_TIME)