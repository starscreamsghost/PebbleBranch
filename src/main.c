#include <pebble.h>
  //creates the static pointer for the window
  static Window *s_main_window;
 //create the static pointer for the time layer 
  static TextLayer *s_time_layer;
  

static void update_time(){
  //Get tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //Create the long-lived buffer
  static char buffer[] = "00:00";
  
  //Write the current time hrs and min
  if(clock_is_24h_style()== true){
    //Use 24 format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else{
    //Use 12hr format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  //Display the time on the textlayer
  text_layer_set_text(s_time_layer, buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}

//instance of the loading of the window
static void main_window_load(Window *window){
  //Create the text layer
  s_time_layer = text_layer_create(GRect(0,55,144,50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  //text_layer_set_text(s_time_layer, "00:00"); Remove later just a place holder
  
  //Improve the layer to more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //Add it as a child layer to Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}
//instance of the unloading of the window
static void main_window_unload(Window *window){
  //Destroy the text layer
  text_layer_destroy(s_time_layer);
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
    //Display time from the start 
    update_time();
    //Registers with TickTimer
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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