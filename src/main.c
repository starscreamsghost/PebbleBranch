#include <pebble.h>
  //creates the static pointer for the window
  static Window *s_main_window;
 //create the static pointer for the time layer 
  static TextLayer *s_hour_1_layer;
  static TextLayer *s_hour_2_layer;
  static TextLayer *s_minute_1_layer;
  static TextLayer *s_minute_2_layer;
  static TextLayer *s_battery_layer;
  static TextLayer *s_date_layer;
  static BitmapLayer *s_background_layer;
  
  static GBitmap *s_background_bitmap;
 

static void update_time(){
  //Get tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //Create the long-lived buffer
  static char time [16];
  static char hours_digit_1 [2]; // changing this from "buffer"
  static char hours_digit_2 [2]; //changing this from "min_buffer"
  static char min_digit_1 [2];
  static char min_digit_2 [2];
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%d %b", tick_time);
  strftime(time, sizeof(time), "d", tick_time);
  
  //Write the current time hrs and min
   if(clock_is_24h_style()== true){
    //Use 24 format
    snprintf(hours_digit_1, sizeof("0"), "%c" , time[0]);
    snprintf(hours_digit_2, sizeof("0") ,"%c", time[1]);
  } else{
    //Use 12hr format
    snprintf(hours_digit_1, sizeof("0"), "%c" , time[0]);
    snprintf(hours_digit_2, sizeof("0") ,"%c", time[1]);
  }
  //Write the current time in minutes
  if(clock_is_24h_style()== true){
    snprintf(min_digit_1, sizeof("0"), "%c", time[2]);    
    snprintf(min_digit_2, sizeof("0"), "%c", time[3]);
  } else{
    //Use 12hr
    snprintf(min_digit_1, sizeof("0"), "%c", time[2]);
    snprintf(min_digit_2, sizeof("0"), "%c", time[3]);
  }
  
  //Display the time on the textlayer
  text_layer_set_text(s_hour_1_layer, hours_digit_1);
  text_layer_set_text(s_hour_2_layer, hours_digit_2);
  text_layer_set_text(s_minute_1_layer, min_digit_1);
  text_layer_set_text(s_minute_2_layer, min_digit_2);  
  text_layer_set_text(s_date_layer,date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}
 
static void min_handler(struct tm *tick_min, TimeUnits units_changed){
  update_time();
}

static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d", new_state.charge_percent);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

//instance of the loading of the window
static void main_window_load(Window *window){
  //Create the GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_Vader);
  s_background_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  //Create the 1st hour layer
  s_hour_1_layer = text_layer_create(GRect(0,70,70,50));
  text_layer_set_background_color(s_hour_1_layer, GColorClear);
  text_layer_set_text_color(s_hour_1_layer, GColorBlack);
  //Create the 2nd hour layer
  s_hour_2_layer = text_layer_create(GRect(0,80,70,50));
  text_layer_set_background_color(s_hour_2_layer, GColorClear);
  text_layer_set_text_color(s_hour_2_layer, GColorBlack);
  //Create the 1st min Layer
  s_minute_1_layer = text_layer_create(GRect(0,110,70,50));
  text_layer_set_background_color(s_minute_1_layer, GColorClear);
  text_layer_set_text_color(s_minute_1_layer, GColorBlack);
  //Create the 2nd min Layer
  s_minute_2_layer = text_layer_create(GRect(0,120,70,50));
  text_layer_set_background_color(s_minute_2_layer, GColorClear);
  text_layer_set_text_color(s_minute_2_layer, GColorBlack);
  //Create battery layer
  s_battery_layer = text_layer_create(GRect(120,10,20,20));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  
  
  //Improve the layer to more like a watchface
  text_layer_set_font(s_hour_1_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_hour_1_layer, GTextAlignmentCenter);
  text_layer_set_font(s_hour_1_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_hour_2_layer, GTextAlignmentCenter);
  text_layer_set_font(s_minute_1_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_minute_1_layer, GTextAlignmentCenter);
  text_layer_set_font(s_minute_1_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_minute_2_layer, GTextAlignmentCenter);
  
  //Add it as a child layer to Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_1_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_2_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_1_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_2_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  
  //Get the current battery level
  battery_handler(battery_state_service_peek());
 
  //Display time from the start 
    update_time();
}
//instance of the unloading of the window
static void main_window_unload(Window *window){
  //Destroy the text layer
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  text_layer_destroy(s_hour_1_layer);
  text_layer_destroy(s_hour_2_layer);
  text_layer_destroy(s_minute_1_layer);
  text_layer_destroy(s_minute_2_layer);
  text_layer_destroy(s_battery_layer);
}

  static void init(){
  //Create the main Window and assigned to pointer
    s_main_window = window_create();
    
    //Set handlers to manage the elements inside the window
    window_set_window_handlers(s_main_window,(WindowHandlers){
      .load = main_window_load,
      .unload = main_window_unload
    });
    
    //Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);
    
    //Registers with TickTimer
    tick_timer_service_subscribe(HOUR_UNIT, tick_handler);
    tick_timer_service_subscribe(MINUTE_UNIT, min_handler);
    
}

static void deinit(){
  //Destroy Window
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}