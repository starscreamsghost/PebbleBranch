#include <pebble.h>
  //creates the static pointer for the window
  static Window *s_main_window;
 //create the static pointer for the time layer 
  static TextLayer *s_time_layer;
  static TextLayer *s_minute_layer;
  static TextLayer *s_colon_layer;
  static BitmapLayer *s_background_layer;
  static GBitmap *s_background_bitmap;
  static GFont s_time_font;

static void update_time(){
  //Get tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  struct tm *tick_min = localtime(&temp);
  
  //Create the long-lived buffer
  static char buffer[] = "00";
  static char min_buffer[] = "00";
  static char colon_buffer[] = ":";
  
  //Write the current time hrs 
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
  text_layer_set_text(s_colon_layer, colon_buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}
static void min_handler(struct tm *tick_min, TimeUnits units_changed){
  update_time();
}

//instance of the loading of the window
static void main_window_load(Window *window){
  //Create the GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_AUTOBOT_IMAGE);
  s_background_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  //Create the hour text layer
  s_time_layer = text_layer_create(GRect(0,75,82,72)); //0,78,144,50
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  //Colon
  s_colon_layer = text_layer_create(GRect(0,75,145,72));
  text_layer_set_background_color(s_colon_layer, GColorClear);
  text_layer_set_text_color(s_colon_layer, GColorWhite);  
  //Create the minute text layer
  s_minute_layer = text_layer_create(GRect(0,75,200,72));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorBlack);
  
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_FONT_BOLD_18));
  
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  //Apply to ColonLayer
  text_layer_set_font(s_colon_layer, s_time_font);
  text_layer_set_text_alignment(s_colon_layer, GTextAlignmentCenter);
  //Apply to Minutelayer
  text_layer_set_font(s_minute_layer, s_time_font);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
  //Add it as a child layer to Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_colon_layer));
  //Display time from the start 
  update_time();
}
//instance of the unloading of the window
static void main_window_unload(Window *window){
  fonts_unload_custom_font(s_time_font);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  //Destroy the text layer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_minute_layer);
  text_layer_destroy(s_colon_layer);
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