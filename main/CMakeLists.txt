idf_component_register(SRCS "main.cpp" "widgets/tux_panel.c" "fonts/font_fa_14.c"
                    	INCLUDE_DIRS "." "devices" "helpers" "pages" "widgets"
				REQUIRES LovyanGFX lvgl fatfs
				)
