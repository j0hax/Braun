#include <pebble.h>
#include "paths.h"

static Window* s_window;
static Layer* s_face_layer;
static Layer* s_hands_layer;
static GPoint center;

static GPath* hour_ptr;
static GPath* minute_ptr;
static GPath* second_ptr;
static GPath* chevron;

static int timedata[4];


// Draws hands onto the given layer
static void draw_hands(Layer* layer, GContext* ctx) {
  gpath_rotate_to(hour_ptr, (TRIG_MAX_ANGLE * timedata[0] / 12) + (TRIG_MAX_ANGLE * timedata[1] / 720.0f));
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, hour_ptr);
  // minute hand
  gpath_rotate_to(minute_ptr, (TRIG_MAX_ANGLE * timedata[1] / 60) + (TRIG_MAX_ANGLE * timedata[2] / 3600.0f));
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, minute_ptr);
  
  // seconds get some extra attention here-- the same as above, but with an extra little counterweight type thing on the opposite site
  GPoint sec0 = (GPoint) {
    .x = (int16_t)(center.x + -sin_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 18 / TRIG_MAX_RATIO),
     .y = (int16_t)(center.y + cos_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 18 / TRIG_MAX_RATIO)
  };
  GPoint sec1 = (GPoint) {
    .x = (int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 72 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 72 / TRIG_MAX_RATIO)
  };
  graphics_context_set_stroke_color(ctx, GColorYellow );
  graphics_context_set_stroke_width(ctx, 3);
  graphics_draw_line(ctx, center, sec1);
  graphics_context_set_stroke_width(ctx, 6);
  graphics_draw_line(ctx, center, sec0);
  graphics_context_set_fill_color(ctx, GColorYellow );
  graphics_fill_circle(ctx, center, 5);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_pixel(ctx, center);
}

// draws face onto the given layer
static void draw_face(Layer* layer, GContext* ctx) {
  
  static char buf[] = "00000000000";
  
  graphics_context_set_stroke_color(ctx, GColorLightGray);

  // draw 60 minute ticks around clock
  for (int i = 0; i < 60; i++) {
    // tic0 = start of inner point
    // tic1 = end of outer point
    GPoint tic0, tic1;

    // tic0 is the starting point from center, tic1 is the outer point of the tic
    if (i % 5 == 0) {
      // hour
      tic0 = (GPoint) {
        .x = (int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 69 / TRIG_MAX_RATIO),
         .y = (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 69 / TRIG_MAX_RATIO)
      };
      tic1 = (GPoint) {
        .x = (int16_t)(tic0.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 15 / TRIG_MAX_RATIO),
         .y = (int16_t)(tic0.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 15 / TRIG_MAX_RATIO)
      };
      graphics_context_set_stroke_width(ctx, 2);
      
      /*if (i != 30) {
        GRect HourText = GRect((int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 54 / TRIG_MAX_RATIO)-10,
                               (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 54 / TRIG_MAX_RATIO)-10,
                               20, 20);
        
        snprintf(buf, sizeof(buf), "%d", i/5);
        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                       HourText, GTextOverflowModeWordWrap,
                       GTextAlignmentCenter, NULL);
        
      }*/

    } else {
      // minute marks
      tic0 = (GPoint) {
        .x = (int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 74 / TRIG_MAX_RATIO),
         .y = (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 74 / TRIG_MAX_RATIO)
      };
      tic1 = (GPoint) {
        .x = (int16_t)(tic0.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 10 / TRIG_MAX_RATIO),
         .y = (int16_t)(tic0.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 10 / TRIG_MAX_RATIO)
      };
      graphics_context_set_stroke_width(ctx, 1);
    }

    graphics_draw_line(ctx, tic0, tic1);
  }

  // draw the date (new and complex how scary)
  snprintf(buf, sizeof(buf), "%d", timedata[3]);
  int yDate = center.y + layer_get_frame(s_face_layer).size.h / 4;
  int xDate = center.x - 6;
  GRect date_window = GRect(xDate, yDate, 12, 18);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, date_window, 0, 0);
  graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     date_window, GTextOverflowModeWordWrap,
                     GTextAlignmentCenter, NULL);
  graphics_context_set_fill_color(ctx, GColorRed);
  gpath_move_to(chevron, GPoint(xDate + 6, yDate - 3));
  gpath_rotate_to(chevron, TRIG_MAX_ANGLE * 0.5f);
  gpath_draw_filled(ctx, chevron);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  // just store the time, don't bother with making a new one in the update_proc or calling update_time
  timedata[0] = tick_time->tm_hour % 12;
  timedata[1] = tick_time->tm_min;
  timedata[2] = tick_time->tm_sec;
  timedata[3] = tick_time->tm_mday;
  layer_mark_dirty(s_hands_layer);
}

static void main_window_load(Window* window) {
  // self-explanatory: make the main window black with one full-screen layer to display the time
  window_set_background_color(window, GColorBlack);
  s_face_layer = layer_create(layer_get_bounds(window_get_root_layer(s_window)));
  s_hands_layer = layer_create(layer_get_bounds(window_get_root_layer(s_window)));
  layer_set_update_proc(s_face_layer, draw_face);
  layer_set_update_proc(s_hands_layer, draw_hands);
  layer_add_child(window_get_root_layer(window), s_face_layer);
  layer_add_child(window_get_root_layer(window), s_hands_layer);
}

static void main_window_unload(Window* window) {
  layer_destroy(s_face_layer);
  layer_destroy(s_hands_layer);
  gpath_destroy(hour_ptr);
  gpath_destroy(minute_ptr);
  gpath_destroy(second_ptr);
}

static void init() {
  // initialize stuff you know the drill
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
     .unload = main_window_unload
  });
  GRect bounds = layer_get_bounds(window_get_root_layer(s_window));
  center = grect_center_point(&bounds);
  hour_ptr = gpath_create(&HOUR_HAND_PATHINFO);
  minute_ptr = gpath_create(&MINUTE_HAND_PATHINFO);
  chevron = gpath_create(&CHEVRON_PATHINFO);
  gpath_move_to(hour_ptr, center);
  gpath_move_to(minute_ptr, center);
  time_t t = time(NULL);
  struct tm* time_now = localtime(&t);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  window_stack_push(s_window, true);
  tick_handler(time_now, SECOND_UNIT);
}


static void deinit() {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}