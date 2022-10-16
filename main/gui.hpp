
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

static lv_obj_t *panel_header;
static lv_obj_t *panel_status;
static lv_obj_t *panel_container;

static lv_obj_t *label_title;
static lv_obj_t *label_message;

static lv_obj_t *icon_storage;
static lv_obj_t *icon_wifi;
static lv_obj_t *icon_battery;


/******************
 *  LVL STYLES
 ******************/
static lv_style_t style_message;
static lv_style_t style_title;
static lv_style_t style_storage;
static lv_style_t style_wifi;
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
static void theme_switch_event_handler(lv_event_t * e);

void lv_setup_styles()
{
    font_symbol = &lv_font_montserrat_14;
    font_normal = &lv_font_montserrat_14;
    font_large = &lv_font_montserrat_16;
    
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
    lv_obj_set_style_border_width(panel_header, 0, 0);
    lv_obj_set_style_radius(panel_header, 0, 0);
    lv_obj_set_align(panel_header, LV_ALIGN_TOP_MID);

    label_title = lv_label_create(panel_header);
    lv_label_set_text(label_title, "DASHBOARD");
    lv_obj_add_style(label_title, &style_title, 0);

    // HEADER STATUS ICON PANEL
    panel_status = lv_obj_create(panel_header);
    lv_obj_set_style_bg_opa(panel_status,LV_OPA_TRANSP,0);
    lv_obj_set_size(panel_status,120,LV_PCT(100));
    lv_obj_set_style_pad_all(panel_status, 0, 0);
    lv_obj_set_style_border_width(panel_status, 0, 0);
    lv_obj_set_style_radius(panel_status, 0, 0);
    lv_obj_set_style_align(panel_status,LV_ALIGN_RIGHT_MID,0);

    // WIFI
    icon_wifi = lv_label_create(panel_status);
    lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    lv_obj_add_style(icon_wifi, &style_wifi, 0);

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
    // CONTENT PANEL CONTAINER
    panel_container = lv_obj_create(parent);
    lv_obj_set_size(panel_container, LV_HOR_RES -20, LV_VER_RES - (HEADER_HEIGHT + FOOTER_HEIGHT + 20));
    lv_obj_set_style_bg_opa(panel_container,LV_OPA_TRANSP,0);
    lv_obj_set_style_border_width(panel_container, 0, 0);
    lv_obj_align(panel_container, LV_ALIGN_TOP_MID, 0, HEADER_HEIGHT+10);
    lv_obj_set_style_pad_all(panel_container,0,0);

    //lv_obj_set_scrollbar_mode(panel_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(panel_container, LV_FLEX_FLOW_ROW_WRAP);

    // Islands #1
    lv_obj_t *contentPanel = lv_obj_create(panel_container);
    lv_obj_set_size(contentPanel, LV_PCT(100), 100);
    //lv_obj_set_style_bg_color(contentPanel, bg_theme_color, 0);
    lv_obj_set_style_border_width(contentPanel, 0, 0);

    // Button with counter
    lv_obj_t *btn1 = lv_btn_create(contentPanel);
    lv_obj_align(btn1,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(btn1, counter_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_t *lbl = lv_label_create(btn1);
    lv_label_set_text(lbl, "Counter Button: 0");
    lv_obj_center(lbl);

    // Islands #2
    lv_obj_t *contentPanel1 = lv_obj_create(panel_container);
    lv_obj_set_size(contentPanel1, LV_PCT(100), 100);
    //lv_obj_set_style_bg_color(contentPanel1, bg_theme_color, 0);
    lv_obj_set_style_border_width(contentPanel1, 0, 0);

    lv_obj_set_flex_flow(contentPanel1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(contentPanel1, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // SHOW LVGL VERSION IN THE CENTER
    lv_obj_t *label = lv_label_create(contentPanel1);
    //lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Theme : Dark");

    lv_obj_t *sw = lv_switch_create(contentPanel1);
    lv_obj_add_event_cb(sw, theme_switch_event_handler, LV_EVENT_ALL, label);

}

static void create_footer(lv_obj_t *parent)
{
    lv_obj_t *panel_footer = lv_obj_create(parent);
    lv_obj_set_size(panel_footer,LV_PCT(100),FOOTER_HEIGHT);
    lv_obj_set_align(panel_footer, LV_ALIGN_BOTTOM_MID);
    //lv_obj_set_style_bg_color(panel_footer, bg_theme_color, 0);
    lv_obj_set_style_pad_all(panel_footer, 0, 0);
    lv_obj_set_style_border_width(panel_footer, 0, 0);
    lv_obj_set_style_radius(panel_footer, 0, 0);

    // Create Footer label and animate if longer
    label_message = lv_label_create(panel_footer);      // full screen as the parent
    lv_obj_set_width(label_message, LV_PCT(100));
    lv_label_set_long_mode(label_message, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_add_style(label_message, &style_message, LV_STATE_DEFAULT);

    // Show LVGL version
    display_message("LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
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

static void theme_switch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * udata = (lv_obj_t * )lv_event_get_user_data(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        if (lv_obj_has_state(obj, LV_STATE_CHECKED))
        {
            theme_current = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
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
