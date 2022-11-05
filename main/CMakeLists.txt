idf_component_register(SRCS "main.cpp" 
							"widgets/tux_panel.c" 
							"fonts/font_fa_14.c" 
							"fonts/font_robotomono_12.c"
							"fonts/font_robotomono_13.c"

							"fonts/font_7seg_64.c"

							"images/dev_bg.c"
							"images/weather_clear_night.c"
							"images/weather_clear_day.c"
							"images/weather_cloudy_snowing.c"
							"images/tux_logo.c"

                    	INCLUDE_DIRS "." "devices" "helpers" "pages" "widgets" "images"
				REQUIRES LovyanGFX lvgl fatfs fmt app_update
				)

# Create a SPIFFS image from the contents of the 'spiffs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
spiffs_create_partition_image(storage ../fatfs FLASH_IN_PROJECT)