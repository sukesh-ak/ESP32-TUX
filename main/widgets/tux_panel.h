/**
 * @file tux_panel.h
 *
 */

#ifndef tux_panel_H
#define tux_panel_H

#ifdef __cplusplus
extern "C" {
#endif

    /*********************
     *      INCLUDES
     *********************/
#include "../../lvgl/src/core/lv_obj.h"

#if LV_USE_PANEL

/*Testing of dependencies*/
#if LV_USE_LABEL == 0
#error "tux_panel: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

/*********************
 *      DEFINES
 *********************/

 /**********************
  *      TYPEDEFS
  **********************/

    typedef struct {
        lv_obj_t obj;           // main panel
        lv_obj_t* title_panel;  // title panel
        lv_obj_t* title;        // title label
        lv_obj_t* content;      // content panel
    } tux_panel_t;

    extern const lv_obj_class_t tux_panel_class;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

     /**
      * Create a message box object
      * @param parent        pointer to parent 
      * @param title         the title of the content box
      * @param height        the height of the content box
      * @return              pointer to the panel object
      */
    lv_obj_t* tux_panel_create(lv_obj_t* parent, const char* title, lv_coord_t height);
    lv_obj_t* tux_panel_get_title(lv_obj_t* obj);
    lv_obj_t* tux_panel_get_title_panel(lv_obj_t* obj);
    lv_obj_t* tux_panel_get_content(lv_obj_t* obj);

    void tux_panel_set_title_color(lv_obj_t* obj,lv_color_t value);
    void tux_panel_set_title_bg_color(lv_obj_t* obj,lv_color_t value);
    void tux_panel_set_height(lv_obj_t* obj, lv_coord_t value);
    void tux_panel_set_content_bg_color(lv_obj_t* obj,lv_color_t value);
    
    /**********************
     *      MACROS
     **********************/

#endif /*LV_USE_panel*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*tux_panel_H*/
