/*
MIT License

Copyright (c) 2022 Sukesh Ashok Kumar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "lv_conf.h"
#include <lvgl.h>


#define LV_TICK_PERIOD_MS 1
    
/*********************
 *  THEME DEFINES
 *********************/
#define MODE_DARK 1
#define RADIUS_DEFAULT (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 12) : lv_disp_dpx(theme.disp, 8))

/*SCREEN*/
#define LIGHT_COLOR_SCR        lv_palette_lighten(LV_PALETTE_GREY, 4)
#define LIGHT_COLOR_CARD       lv_color_white()
#define LIGHT_COLOR_TEXT       lv_palette_darken(LV_PALETTE_GREY, 4)
#define LIGHT_COLOR_GREY       lv_palette_lighten(LV_PALETTE_GREY, 2)
#define DARK_COLOR_SCR         lv_color_hex(0x15171A)
#define DARK_COLOR_CARD        lv_color_hex(0x282b30)
#define DARK_COLOR_TEXT        lv_palette_lighten(LV_PALETTE_GREY, 5)
#define DARK_COLOR_GREY        lv_color_hex(0x2f3237)

#define TRANSITION_TIME         LV_THEME_DEFAULT_TRANSITION_TIME
#define BORDER_WIDTH            lv_disp_dpx(theme.disp, 2)
#define OUTLINE_WIDTH           lv_disp_dpx(theme.disp, 3)

#define PAD_DEF     (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 24) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 20) : lv_disp_dpx(theme.disp, 16))
#define PAD_SMALL   (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 14) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 12) : lv_disp_dpx(theme.disp, 10))
#define PAD_TINY   (disp_size == DISP_LARGE ? lv_disp_dpx(theme.disp, 8) : disp_size == DISP_MEDIUM ? lv_disp_dpx(theme.disp, 6) : lv_disp_dpx(theme.disp, 2))


/*** Setup screen resolution for LVGL ***/
static const uint16_t screenWidth = TFT_WIDTH;
static const uint16_t screenHeight = TFT_HEIGHT;

#define BUFF_SIZE 40
#define LVGL_DOUBLE_BUFFER

static lv_disp_draw_buf_t draw_buf;

static lv_disp_t *disp;
static lv_theme_t *theme_current;
static lv_color_t bg_theme_color;

static LGFX lcd; // declare display variable

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
static SemaphoreHandle_t xGuiSemaphore = NULL;
static TaskHandle_t g_lvgl_task_handle;

static void gui_task(void *args);

/*** Function declaration ***/
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
static void lv_tick_task(void *arg);


esp_err_t lv_display_init()
{
    // Setting display to landscape
    // if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 2);

    lcd.setRotation(2);
    lcd.setColorDepth(16);
    lcd.setBrightness(128);
    //lcd.fillScreen(TFT_BLACK);

    /* LVGL : Setting up buffer to use for display */
#if defined(LVGL_DOUBLE_BUFFER)
    // EXT_RAM_BSS_ATTR lv_color_t * buf1 = (lv_color_t *)malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));
    // EXT_RAM_BSS_ATTR lv_color_t * buf2 = (lv_color_t *)malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));

    lv_color_t * buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t),MALLOC_CAP_DMA);
    lv_color_t * buf2 = (lv_color_t *)heap_caps_malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t),MALLOC_CAP_DMA);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * BUFF_SIZE);    
#else
    // EXT_RAM_BSS_ATTR lv_color_t * buf1 = (lv_color_t *)malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));
    lv_color_t * buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t),MALLOC_CAP_DMA);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * BUFF_SIZE);
#endif

    /*** LVGL : Setup & Initialize the display device driver ***/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = display_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.sw_rotate = 1;
    disp = lv_disp_drv_register(&disp_drv);

    //*** LVGL : Setup & Initialize the input device driver ***
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t lv_periodic_timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "periodic_gui",
        .skip_unhandled_events  = true
        };
    esp_timer_handle_t lv_periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&lv_periodic_timer_args, &lv_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lv_periodic_timer, LV_TICK_PERIOD_MS * 1000));

    // Setup theme
    theme_current = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                          lv_palette_main(LV_PALETTE_RED),
                                          LV_USE_THEME_DEFAULT, /*Light or dark mode*/
                                          &lv_font_montserrat_14);

    //lv_disp_set_theme(disp, theme_current); /*Assign the theme to the display*/
    //bg_theme_color = theme_current->flags & LV_USE_THEME_DEFAULT ? DARK_COLOR_CARD : LIGHT_COLOR_CARD;
    bg_theme_color = theme_current->flags & LV_USE_THEME_DEFAULT ? lv_palette_darken(LV_PALETTE_GREY, 5) : lv_color_hex(0xBFBFBD);

    xGuiSemaphore = xSemaphoreCreateMutex();
    if (!xGuiSemaphore)  
    {
        ESP_LOGE(TAG, "Create mutex for LVGL failed");
        if (lv_periodic_timer) esp_timer_delete(lv_periodic_timer);
        return ESP_FAIL;
    }

#if CONFIG_FREERTOS_UNICORE == 0
    int err = xTaskCreatePinnedToCore(gui_task, "lv gui", 1024 * 8, NULL, 3, &g_lvgl_task_handle, 1);
#else
    int err = xTaskCreatePinnedToCore(gui_task, "lv gui", 1024 * 8, NULL, 3, &g_lvgl_task_handle, 0);
#endif
    if (!err)
    {
        ESP_LOGE(TAG, "Create task for LVGL failed");
        if (lv_periodic_timer) esp_timer_delete(lv_periodic_timer);
        return ESP_FAIL;
    }

    return ESP_OK;
}

// Display callback to flush the buffer to screen
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    /* Without DMA */
    // lcd.startWrite();
    // lcd.setAddrWindow(area->x1, area->y1, w, h);
    // lcd.pushPixels((uint16_t *)&color_p->full, w * h, true);
    // lcd.endWrite();

    /* With DMA */
    // TODO: Yet to do performance test
    lcd.startWrite();
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    lcd.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
    lcd.endWrite();

    lv_disp_flush_ready(disp);
}

/* Setting up tick task for lvgl */
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void gui_task(void *args)
{
    ESP_LOGI(TAG, "Start to run LVGL");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            //lv_timer_handler_run_in_period(5); /* run lv_timer_handler() every 5ms */
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

void lvgl_acquire(void)
{
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    if (g_lvgl_task_handle != task) {
        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    }
}

void lvgl_release(void)
{
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    if (g_lvgl_task_handle != task) {
        xSemaphoreGive(xGuiSemaphore);
    }
}

// Touchpad callback to read the touchpad
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    bool touched = lcd.getTouch(&touchX, &touchY);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        // Set the coordinates
        data->point.x = touchX;
        data->point.y = touchY;
    }
}
