#include <pebble.h>
  //creates the static pointer for the window
  static Window *s_main_window;
 //create the static pointer for the time layer 
  static TextLayer *s_time_layer;
  static TextLayer *s_minute_layer;
  static TextLayer *s_battery_layer;
  static TextLayer *s_date_layer;
  static BitmapLayer *s_background_layer, *s_bt_icon_layer;
  
  //static GBitmap *s_background_layer;
  static GBitmap *s_background_bitmap, *s_bt_icon_bitmap;
  //static GBitmap *s_background_bitmap;

  

static void update_time(){
  //Get tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  struct tm *tick_min = localtime(&temp);
  
  //Create the long-lived buffer
  static char buffer[] = "00";
  static char min_buffer[] = "00";
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%d %b", tick_time);
  
  //Write the current time hrs and min
   if(clock_is_24h_style()== true){
    //Use 24 format
    strftime(buffer, sizeof("00"), "%H", tick_time);
  } else{
    //Use 12hr format
    strftime(buffer, sizeof("00"), "%I", tick_time);
  }
  //Write the current time in minutes
  if(clock_is_24h_style()== true){
    strftime(min_buffer, sizeof("00"), "%M", tick_min);
  } else{
    //Use 12hr
    strftime(min_buffer, sizeof("00"), "%M", tick_min);
  }
  
  //Display the time on the textlayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_minute_layer, min_buffer);
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

static void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);
  
  if(!connected) {
    vibes_double_pulse();
  }
}

//instance of the loading of the window
static void main_window_load(Window *window){
  //Create the GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STORMTROOPER);
  s_background_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  //Create the hour layer
  s_time_layer = text_layer_create(GRect(0,70,70,50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  //Create the min Layer
  s_minute_layer = text_layer_create(GRect(0,110,70,50));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorWhite);
  //Create battery layer
  s_battery_layer = text_layer_create(GRect(120,10,20,20));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  //Create line layer
  s_date_layer = text_layer_create(GRect(0,65,70,50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  //Create BT Layer
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BTicon);
  s_bt_icon_layer = bitmap_layer_create(GRect(0,0,30,30)); //59,12,30,30 <- example coordinates
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));
  
  //Improve the layer to more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_minute_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
  
  //Add it as a child layer to Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  
  //Get the current battery level
  battery_handler(battery_state_service_peek());
  //Check the bluetooth connection
 // bluetooth_callback(bluetooth_connection_service_peek());
  //Display time from the start 
    update_time();
  //Show the correct state of the BT connection
 #ifdef PBL_SDK_2
  bluetooth_callback(bluetooth_connection_service_peek());
#elif PBL_SDK_3
 bluetooth_callback(connection_service_peek_pebble_app_connection());
#endif
}
//instance of the unloading of the window
static void main_window_unload(Window *window){
  //Destroy the text layer
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_bt_icon_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_minute_layer);
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
    
#ifdef PBL_SDK_2
    bluetooth_connection_service_subscribe(bluetooth_callback);
#elif PBL_SDK_3
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
#endif
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