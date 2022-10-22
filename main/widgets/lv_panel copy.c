/**
 * @file lv_panel.c
 *
 */

 /*********************
  *      INCLUDES
  *********************/
#include "lv_panel.h"
#if LV_USE_PANEL

#include "../../components/lvgl/src/widgets/label/lv_label.h"
#include "../../components/lvgl/src/misc/lv_assert.h"
#include "../../components/lvgl/src/core/lv_disp.h"
#include "../../components/lvgl/src/layouts/flex/lv_flex.h"

  /*********************
   *      DEFINES
   *********************/
#define MY_CLASS    &lv_panel_class

   /**********************
    *      TYPEDEFS
    **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_panel_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_panel_t)
};

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/

void default_panel_styles()
{

}

lv_obj_t * lv_panel_create(lv_obj_t* parent, const char* title, lv_coord_t height)
{
    LV_LOG_INFO("begin");

    // Main panel
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_height(container, height);
    lv_obj_set_width(container, LV_PCT(100));
    lv_obj_set_style_pad_top(container,0,0);
    lv_obj_set_style_pad_bottom(container,3,0);
    //lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_bg_opa(container,LV_OPA_50,0);



    LV_ASSERT_MALLOC(container);
    if (container == NULL) return NULL;

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    
    // bool has_title = title && strlen(title) > 0;
    // if (has_title) {

        // Title Panel
        lv_obj_t* title_panel = lv_obj_create(container);

        lv_obj_set_style_pad_hor(title_panel,5,0);
        lv_obj_set_style_pad_ver(title_panel,2,0);
        lv_obj_set_style_radius(title_panel,3,0);
        lv_obj_set_style_border_width(title_panel,1,0);
        lv_obj_set_style_border_side(title_panel,LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_LEFT,0);
        lv_obj_set_size(title_panel,LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_size(title_panel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

        // Title Label
        lv_obj_t* obj_title = lv_label_create(title_panel);
        lv_label_set_text(obj_title, title);
        lv_label_set_long_mode(obj_title, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_width(obj_title, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(obj_title, 3,0);
//    }

    // Widget container panel
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_panel_class,container);
    lv_obj_class_init_obj(obj);

    lv_panel_t * _panel = (lv_panel_t *)obj;

    lv_obj_set_size(_panel,lv_pct(100), height-42);
    //lv_obj_set_style_border_width(_panel,1,0);

    return obj;
}

void lv_panel_set_title_color(lv_obj_t* obj,lv_color_t value)
{
    lv_panel_t* _panel = (lv_panel_t*)obj;
    lv_obj_set_style_text_color(_panel->title,value,0);
}

void lv_panel_set_title_bg_color(lv_obj_t* obj,lv_color_t value)
{
    lv_panel_t* _panel = (lv_panel_t*)obj;
    lv_obj_set_style_bg_color(_panel->title_panel,value,0);
}

#endif /*LV_USE_PANEL*/
