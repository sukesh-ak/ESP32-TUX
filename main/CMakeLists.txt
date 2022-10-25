idf_component_register(SRCS "main.cpp" 
							"widgets/tux_panel.c" 
							"fonts/font_fa_14.c" 

							"fonts/font_7seg_64.c"

							"images/dev_bg.c"
							"images/weather_clear_night.c"
							"images/weather_clear_day.c"
							"images/weather_cloudy_snowing.c"

                    	INCLUDE_DIRS "." "devices" "helpers" "pages" "widgets" "images"
				REQUIRES LovyanGFX lvgl fatfs
				)
