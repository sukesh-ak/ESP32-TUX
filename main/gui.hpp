/*
    Free PNG icons => https://www.flaticon.com/search?word=charte&shape=outline&order_by=4
*/

/* 
    Create Custom symbols

Download FontAwesome Free version or 
take from lvgl\scripts\built_in_font\FontAwesome5-Solid+Brands+Regular.woff
Automation done with this => https://github.com/lvgl/lv_font_conv

Goto conversion tool
https://lvgl.io/tools/fontconverter

Name eg. font_fa_14
Size : 14
Bpp : 4 bit-per-pixel

Select 'fa-brands-400.ttf' from download
Range: Provide the symbol hex values with comma separated

Use this site to convert Unicode to Hex UTF-8 bytes
https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=f294&mode=hex

then define constants for us like
#define FA_BLE_SYMBOL "\xEF\x8A\x94"
*/


LV_FONT_DECLARE(font_fa_14)
#define FA_SYMBOL_BLE "\xEF\x8A\x94"    // 0xf294
#define FA_SYMBOL_SETTINGS "\xEF\x80\x93" // 0xf0ad
/*********************
 *      DEFINES
 *********************/
#define HEADER_HEIGHT 30 
#define FOOTER_HEIGHT 30 


/******************
 *  LV DEFINES
 ******************/
static const lv_font_t *font_large;
static const lv_font_t *font_normal;
static const lv_font_t *font_symbol;
static const lv_font_t *font_fa;

static lv_obj_t *panel_header;
static lv_obj_t *panel_status; // Status icons in the header
static lv_obj_t *content_container;
static lv_obj_t *screen_container;

static lv_obj_t *label_title;
static lv_obj_t *label_message;

static lv_obj_t *icon_storage;
static lv_obj_t *icon_wifi;
static lv_obj_t *icon_ble;
static lv_obj_t *icon_battery;

static lv_coord_t screen_h;
static lv_coord_t screen_w;

/******************
 *  LVL STYLES
 ******************/
static lv_style_t style_message;
static lv_style_t style_title;
static lv_style_t style_storage;
static lv_style_t style_wifi;
static lv_style_t style_ble;
static lv_style_t style_battery;


/******************
 *  LVL ANIMATION
 ******************/
static lv_anim_t anim_labelscroll;


/******************
 *  LVL FUNCS & EVENTS
 ******************/
static void create_header(lv_obj_t *parent);
static void create_content(lv_obj_t *parent);
static void create_footer(lv_obj_t *parent);
static void display_message(const char * fmt, ...);
static void panel_status_eventhandler(lv_event_t* e);
static void counter_event_handler(lv_event_t * e);
static void rotate_event_handler(lv_event_t * e);
static void theme_switch_event_handler(lv_event_t * e);

void lv_setup_styles()
{
    font_symbol = &lv_font_montserrat_14;
    font_normal = &lv_font_montserrat_14;
    font_large = &lv_font_montserrat_16;
    font_fa = &font_fa_14;

    screen_h = lv_obj_get_height(lv_scr_act());
    screen_w = lv_obj_get_width(lv_scr_act());
    
    // DASHBOARD TITLE
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    lv_style_set_align(&style_title, LV_ALIGN_LEFT_MID);
    lv_style_set_pad_left(&style_title, 15);

    // BATTERY
    lv_style_init(&style_battery);
    lv_style_set_text_font(&style_battery, font_symbol);
    lv_style_set_align(&style_battery, LV_ALIGN_RIGHT_MID);
    lv_style_set_pad_right(&style_battery, 15);

    // SD CARD
    lv_style_init(&style_storage);
    lv_style_set_text_font(&style_storage, font_symbol);
    lv_style_set_align(&style_storage, LV_ALIGN_RIGHT_MID);
    lv_style_set_pad_right(&style_storage, 45);

    // WIFI
    lv_style_init(&style_wifi);
    lv_style_set_text_font(&style_wifi, font_symbol);
    lv_style_set_align(&style_wifi, LV_ALIGN_RIGHT_MID);
    lv_style_set_pad_right(&style_wifi, 65);

    // BLE
    lv_style_init(&style_ble);
    lv_style_set_text_font(&style_ble, font_fa);
    lv_style_set_align(&style_ble, LV_ALIGN_RIGHT_MID);
    lv_style_set_pad_right(&style_ble, 90); // Symbol is narrow

    // FOOTER MESSAGE & ANIMATION
    lv_anim_init(&anim_labelscroll);
    lv_anim_set_delay(&anim_labelscroll, 1000);          //Wait 1 second to start the first scroll
    lv_anim_set_repeat_delay(&anim_labelscroll,3000);    //Repeat the scroll 3 seconds after the label scrolls back to the initial position

    lv_style_init(&style_message);
    lv_style_set_anim(&style_message, &anim_labelscroll);   // Set animation for the style

    //lv_style_set_text_color(&style_message, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_opa(&style_message, LV_OPA_COVER);
    lv_style_set_text_font(&style_message, font_normal);
    lv_style_set_align(&style_message, LV_ALIGN_LEFT_MID);
    lv_style_set_pad_left(&style_message, 15);    
    lv_style_set_pad_right(&style_message, 15);    
}

static void create_header(lv_obj_t *parent)
{
    // HEADER PANEL
    panel_header = lv_obj_create(parent);
    lv_obj_set_size(panel_header,LV_PCT(100),HEADER_HEIGHT);
    //lv_obj_set_style_bg_color(panel_header, bg_theme_color, 0);
    lv_obj_set_style_pad_all(panel_header, 0, 0);
    lv_obj_set_style_radius(panel_header, 0, 0);
    lv_obj_set_align(panel_header, LV_ALIGN_TOP_MID);

    label_title = lv_label_create(panel_header);
    lv_label_set_text(label_title, LV_SYMBOL_HOME " DASHBOARD");
    lv_obj_add_style(label_title, &style_title, 0);

    // HEADER STATUS ICON PANEL
    panel_status = lv_obj_create(panel_header);
    lv_obj_set_style_bg_opa(panel_status,LV_OPA_TRANSP,0);
    lv_obj_set_size(panel_status,120,LV_PCT(100)-2);
    lv_obj_set_style_pad_all(panel_status, 0, 0);
    lv_obj_set_style_border_width(panel_status, 0, 0);
    lv_obj_set_style_radius(panel_status, 0, 0);
    lv_obj_set_style_align(panel_status,LV_ALIGN_RIGHT_MID,0);

    // WIFI
    icon_wifi = lv_label_create(panel_status);
    lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    lv_obj_add_style(icon_wifi, &style_wifi, 0);

    // BLE
    icon_ble = lv_label_create(panel_status);
    lv_label_set_text(icon_ble, FA_SYMBOL_BLE);
    lv_obj_add_style(icon_ble, &style_ble, 0);

    // SD CARD
    icon_storage = lv_label_create(panel_status);
    lv_label_set_text(icon_storage, LV_SYMBOL_SD_CARD);
    lv_obj_add_style(icon_storage, &style_storage, 0);  

    // BATTERY
    icon_battery = lv_label_create(panel_status);
    lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_add_style(icon_battery, &style_battery, 0);

    lv_obj_add_event_cb(panel_status, panel_status_eventhandler, LV_EVENT_CLICKED, NULL);
}

static void create_content(lv_obj_t *parent)
{
    // Convert Islands to style (or a widget) to use everywhere

    // **************** UI Islands #1
    lv_obj_t *contentPanel2 = lv_obj_create(parent);
    lv_obj_set_size(contentPanel2, LV_PCT(100), 100);
    lv_obj_set_style_bg_opa(contentPanel2,LV_OPA_50,0);

    // Rotate
    lv_obj_t *btn2 = lv_btn_create(contentPanel2);
    lv_obj_align(btn2,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(btn2, rotate_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_t *lbl2 = lv_label_create(btn2);
    lv_label_set_text(lbl2, "Set to Landscape");
    lv_obj_center(lbl2);

    // **************** UI Islands #2
    lv_obj_t *contentPanel1 = lv_obj_create(parent);
    lv_obj_set_size(contentPanel1, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(contentPanel1,LV_OPA_50,0);

    lv_obj_set_flex_flow(contentPanel1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(contentPanel1, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(contentPanel1);
    //lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Theme : Dark");

    lv_obj_t *sw = lv_switch_create(contentPanel1);
    lv_obj_add_event_cb(sw, theme_switch_event_handler, LV_EVENT_ALL, label);

    // **************** UI Islands #3
    lv_obj_t *contentPanel = lv_obj_create(parent);
    lv_obj_set_size(contentPanel, LV_PCT(100), 300);
    lv_obj_set_style_bg_opa(contentPanel,LV_OPA_50,0);

    // UI Island Title
    lv_obj_t *iTitle_Panel = lv_obj_create(contentPanel);
    lv_obj_align(iTitle_Panel, LV_ALIGN_TOP_LEFT, 0, -13);
    lv_obj_set_style_border_width(iTitle_Panel, 0, 0);
    lv_obj_set_style_pad_top(iTitle_Panel, 4, 0);
    lv_obj_set_style_pad_bottom(iTitle_Panel, 3, 0);
    lv_obj_set_style_radius(iTitle_Panel, 3, 0);
    lv_obj_set_style_bg_color(iTitle_Panel,lv_palette_main(LV_PALETTE_CYAN),0); 
    lv_obj_set_style_bg_opa(iTitle_Panel,LV_OPA_50,0);
    lv_obj_set_size(iTitle_Panel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    
    // UI Island Title Text
    lv_obj_t *lbl_iTitle = lv_label_create(iTitle_Panel);
    lv_label_set_text(lbl_iTitle, LV_SYMBOL_BELL " UI ISLAND TITLE");

    // Button with counter
    lv_obj_t *btn1 = lv_btn_create(contentPanel);
    lv_obj_align(btn1,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(btn1, counter_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_t *lbl = lv_label_create(btn1);
    lv_label_set_text(lbl, "Counter Button: 0");
    lv_obj_center(lbl);

}

static void create_footer(lv_obj_t *parent)
{
    lv_obj_t *panel_footer = lv_obj_create(parent);
    lv_obj_set_size(panel_footer,LV_PCT(100),FOOTER_HEIGHT);
    //lv_obj_set_style_bg_color(panel_footer, bg_theme_color, 0);
    lv_obj_set_style_pad_all(panel_footer, 0, 0);
    lv_obj_set_style_radius(panel_footer, 0, 0);
    lv_obj_set_align(panel_footer, LV_ALIGN_BOTTOM_MID);

    // Create Footer label and animate if longer
    label_message = lv_label_create(panel_footer);      // full screen as the parent
    lv_obj_set_width(label_message, LV_PCT(100));
    lv_label_set_long_mode(label_message, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_add_style(label_message, &style_message, LV_STATE_DEFAULT);

    // Show LVGL version
    display_message("LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
}

static void draw_ui()
{
    // screen_container is the root of the UX
    screen_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(screen_container,LV_PCT(100),LV_PCT(100));
    lv_obj_set_style_pad_all(screen_container, 0, 0);
    lv_obj_align(screen_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(screen_container,LV_OPA_TRANSP,0);
    lv_obj_set_style_border_width(screen_container, 0, 0);
    lv_obj_set_scrollbar_mode(screen_container, LV_SCROLLBAR_MODE_OFF);

    // STATUS / TITLE BAR
    create_header(screen_container);
    create_footer(screen_container);    

    content_container = lv_obj_create(screen_container);
    lv_obj_set_size(content_container,screen_w,screen_h - HEADER_HEIGHT - FOOTER_HEIGHT); 
    lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, HEADER_HEIGHT);
    lv_obj_set_style_border_width(content_container, 0, 0);   

    // Comment the line below which sets transparency and then either set a background color or image
    // Also set the island panels to 50% transparency
    lv_obj_set_style_bg_opa(content_container,LV_OPA_TRANSP,0);
    //lv_obj_set_style_bg_color(content_container,lv_palette_main(LV_PALETTE_RED),0);

    lv_obj_set_flex_flow(content_container, LV_FLEX_FLOW_ROW_WRAP);
    
    create_content(content_container);
}

  /* Counter button event handler */
  static void counter_event_handler(lv_event_t * e)
  {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
      static uint8_t cnt = 0;
      cnt++;

      /*Get the first child of the button which is the label and change its text*/
      lv_obj_t *label = lv_obj_get_child(btn, 0);
      lv_label_set_text_fmt(label, "Counter Button: %d", cnt);
      //LV_LOG_USER("Clicked");
    }
  }

static void rotate_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);

    if (code == LV_EVENT_CLICKED)
    {
        lvgl_acquire();

        if (lv_disp_get_rotation(disp) == LV_DISP_ROT_NONE)
        {
            lv_disp_set_rotation(disp, LV_DISP_ROT_90);
            lv_label_set_text(label, "Set to Portrait");
        }
        else
        {
            lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);
            lv_label_set_text(label, "Set to Landscape");
        }

        lvgl_release();

        // Update 
        screen_h = lv_obj_get_height(lv_scr_act());
        screen_w = lv_obj_get_width(lv_scr_act());
        lv_obj_set_size(content_container,screen_w,screen_h-HEADER_HEIGHT-FOOTER_HEIGHT); 
        
        //display_message("%d,%d",screen_h,screen_w);
    }
}

static void theme_switch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * udata = (lv_obj_t * )lv_event_get_user_data(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        if (lv_obj_has_state(obj, LV_STATE_CHECKED))
        {
            theme_current = lv_theme_default_init(disp, 
                                            lv_palette_main(LV_PALETTE_BLUE),
                                            lv_palette_main(LV_PALETTE_RED),
                                            0, /*Light or dark mode*/
                                            &lv_font_montserrat_14);      
            bg_theme_color = lv_palette_lighten(LV_PALETTE_GREY, 1);
            lv_disp_set_theme(disp,theme_current);
            lv_label_set_text(udata, "Theme : Light");
        }
        else
        {
            theme_current = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                lv_palette_main(LV_PALETTE_RED),
                                1, /*Light or dark mode*/
                                &lv_font_montserrat_14);      
            bg_theme_color = lv_palette_darken(LV_PALETTE_GREY, 4);
            lv_disp_set_theme(disp,theme_current);
            lv_label_set_text(udata, "Theme : Dark");
        }
    }
}

static void display_message(const char * fmt, ...)
{
    char buffer[200];
    va_list args;
    va_start(args,fmt);
    vsprintf(buffer,fmt,args);
    lv_label_set_text(label_message,buffer);
    va_end(args);
}

static void panel_status_eventhandler(lv_event_t* e)
{
    display_message("Status icons touched but this is a very long message to show scroll animation!");
}
