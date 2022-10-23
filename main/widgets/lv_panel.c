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
    .width_def = LV_PCT(100), // LV_DPI_DEF * 2,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_panel_t)
};

const lv_obj_class_t lv_panel_content_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_obj_t)
};

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/

void default_panel_styles()
{
    //lv_theme_t * current_theme = 
}

lv_obj_t * lv_panel_create(lv_obj_t* parent, const char* title, lv_coord_t height)
{
    LV_LOG_INFO("begin");

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_panel_class,parent);
    lv_obj_class_init_obj(obj);

    lv_panel_t * _panel = (lv_panel_t *)obj;
    LV_ASSERT_MALLOC(obj);
    if (obj == NULL) return NULL;

    // Widget Container
    lv_obj_set_height(obj,height);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP);
    
    bool has_title = title && strlen(title) > 0;
    if (has_title) {

        // Title Panel
        _panel->title_panel = lv_obj_create(obj);
        lv_obj_set_style_pad_hor(_panel->title_panel,5,0);
        lv_obj_set_style_pad_ver(_panel->title_panel,2,0);
        lv_obj_set_style_radius(_panel->title_panel,3,0);
        lv_obj_set_size(_panel->title_panel,LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        //lv_obj_set_x(_panel->title_panel,100);

        // Title Label
        _panel->title = lv_label_create(_panel->title_panel);
        lv_label_set_text(_panel->title, title);
        //lv_label_set_long_mode(_panel->title, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_width(_panel->title, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(_panel->title, 3,0);
        lv_obj_set_style_align(_panel->title,LV_ALIGN_TOP_LEFT,0);

        // Add an event callback for this later (set _panel->title to LV_PCT(100))
        // lv_obj_t * icon_edit = lv_label_create(_panel->title_panel);
        // lv_obj_set_style_align(icon_edit,LV_ALIGN_TOP_RIGHT,0);
        // lv_label_set_text(icon_edit, LV_SYMBOL_SETTINGS);
    }

    _panel->content = lv_obj_class_create_obj(&lv_panel_content_class, obj);
    lv_obj_set_style_pad_all(_panel->content,5,0);

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

lv_obj_t * lv_panel_get_title(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_panel_t * _panel = (lv_panel_t *)obj;
    return _panel->title;
}

lv_obj_t* lv_panel_get_content(lv_obj_t* obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_panel_t * _panel = (lv_panel_t *)obj;
    return _panel->content;
}

lv_obj_t* lv_panel_get_title_panel(lv_obj_t* obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_panel_t * _panel = (lv_panel_t *)obj;
    return _panel->title_panel;
}
#endif /*LV_USE_PANEL*/
