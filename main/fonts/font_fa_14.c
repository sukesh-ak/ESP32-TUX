/*******************************************************************************
 * Size: 14 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef FONT_FA_14
#define FONT_FA_14 1
#endif

#if FONT_FA_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+F013 "" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xa, 0xff, 0xa0, 0x0, 0x0, 0x0, 0x0,
    0xc, 0xff, 0xc0, 0x0, 0x0, 0x3, 0xd6, 0xdf,
    0xff, 0xfd, 0x6d, 0x30, 0xe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xe0, 0x5f, 0xff, 0xff, 0xaa, 0xff,
    0xff, 0xf5, 0x1a, 0xff, 0xf4, 0x0, 0x4f, 0xff,
    0xa1, 0x3, 0xff, 0xd0, 0x0, 0xd, 0xff, 0x30,
    0x4, 0xff, 0xf0, 0x0, 0xf, 0xff, 0x40, 0x4f,
    0xff, 0xfb, 0x22, 0xbf, 0xff, 0xf4, 0x2f, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf2, 0x9, 0xfe, 0xff,
    0xff, 0xff, 0xef, 0x90, 0x0, 0x50, 0x5e, 0xff,
    0xe5, 0x5, 0x0, 0x0, 0x0, 0xc, 0xff, 0xc0,
    0x0, 0x0, 0x0, 0x0, 0x4, 0x77, 0x40, 0x0,
    0x0,

    /* U+F294 "" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x19, 0x0,
    0x0, 0x0, 0x1, 0xf8, 0x0, 0x0, 0x0, 0x1f,
    0xf7, 0x0, 0x1b, 0x21, 0xf4, 0xe6, 0x0, 0xae,
    0x3f, 0x3d, 0x70, 0x0, 0xae, 0xfe, 0x90, 0x0,
    0x0, 0xbf, 0xb0, 0x0, 0x0, 0x1d, 0xfd, 0x10,
    0x0, 0x1d, 0xcf, 0xcd, 0x10, 0x1d, 0xb1, 0xf2,
    0xcc, 0x0, 0x60, 0xf, 0x7f, 0x40, 0x0, 0x0,
    0xff, 0x40, 0x0, 0x0, 0xf, 0x40, 0x0, 0x0,
    0x0, 0x40, 0x0, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 224, .box_w = 14, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 105, .adv_w = 140, .box_w = 9, .box_h = 15, .ofs_x = 0, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x281
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 61459, .range_length = 642, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t font_fa_14 = {
#else
lv_font_t font_fa_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -5,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if FONT_FA_14*/

