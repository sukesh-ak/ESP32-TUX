idf_component_register(SRCS "main.cpp" 
					"widgets/tux_panel.c" 
					"images/dev_bg.c"

					"fonts/font_fa_14.c" 
					"fonts/font_fa_weather_32.c"
					"fonts/font_fa_weather_48.c"
					"fonts/font_fa_weather_56.c"
					#"fonts/font_fa_weather_64.c"
					# "fonts/font_robotomono_12.c"
					"fonts/font_robotomono_13.c"

					# "fonts/font_7seg_64.c"
					# "fonts/font_7seg_60.c"
					# "fonts/font_7seg_58.c"
					"fonts/font_7seg_56.c"

                INCLUDE_DIRS "." "devices" "helpers" "pages" "widgets" "images"
				REQUIRES LovyanGFX lvgl fatfs fmt SettingsConfig OpenWeatherMap
				)

spiffs_create_partition_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT)
#fatfs_create_spiflash_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT PRESERVE_TIME)