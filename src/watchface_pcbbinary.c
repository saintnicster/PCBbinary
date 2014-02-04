#include "pebble.h"

Window *window;

static GBitmap *background_image;
static BitmapLayer *background_layer;

Layer *led_layer;

GRect getRect(int row, int led) {
  if (row == 0)
    return GRect(18 + led * 33, 56, 6, 7);
  else
    return GRect(18 + led * 20, 98, 6, 7);
}

void color_led(GContext* ctx, int row, int led, bool on) {
  graphics_context_set_fill_color(ctx, on ? GColorWhite : GColorBlack);
  graphics_fill_rect(ctx, getRect(row, led), 1, 0);
}

void led_layer_update_callback(Layer *me, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned short hour = t->tm_hour % 12;
  hour = hour ? hour : 12; // 0 as 12
  unsigned short min = t->tm_min;

  color_led(ctx, 0, 0, hour & 8);
  color_led(ctx, 0, 1, hour & 4);
  color_led(ctx, 0, 2, hour & 2);
  color_led(ctx, 0, 3, hour & 1);

  color_led(ctx, 1, 0, min & 32);
  color_led(ctx, 1, 1, min & 16);
  color_led(ctx, 1, 2, min & 8);
  color_led(ctx, 1, 3, min & 4);
  color_led(ctx, 1, 4, min & 2);
  color_led(ctx, 1, 5, min & 1);
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(led_layer);
}

static void do_init(void) {
		
  window = window_create();
  window_stack_push(window, true);

  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
	
  GRect frame = layer_get_frame(window_layer);	

  // Background image
  background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  background_layer = bitmap_layer_create(layer_get_frame(window_layer));
  bitmap_layer_set_bitmap(background_layer, background_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));		

  // Init the layer for the display
  led_layer = layer_create(frame);
  layer_set_update_proc(led_layer, &led_layer_update_callback);
  layer_add_child(window_layer, led_layer);

  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void do_deinit(void) {
  bitmap_layer_destroy(background_layer);
  gbitmap_destroy(background_image);
  layer_destroy(led_layer);
  window_destroy(window);	 
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}