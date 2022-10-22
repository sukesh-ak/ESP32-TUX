﻿/**
 * @file lv_mbox.h
 *
 */

#ifndef LV_PANEL_H
#define LV_PANEL_H

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
#error "lv_panel: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
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
    } lv_panel_t;

    extern const lv_obj_class_t lv_panel_class;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

     /**
      * Create a message box object
      * @param parent        pointer to parent 
      * @param title         the title of the content box
      * @param txt           the text of the content box
      * @return              pointer to the panel object
      */
    lv_obj_t* lv_panel_create(lv_obj_t* parent, const char* title, lv_coord_t height);
    lv_obj_t* lv_panel_get_title(lv_obj_t* obj);
    lv_obj_t* lv_panel_get_title_panel(lv_obj_t* obj);
    lv_obj_t* lv_panel_get_content(lv_obj_t* obj);

    void lv_panel_set_title_color(lv_obj_t* obj,lv_color_t value);
    void lv_panel_set_title_bg_color(lv_obj_t* obj,lv_color_t value);

    /**********************
     *      MACROS
     **********************/

#endif /*LV_USE_panel*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_panel_H*/
