#include <pebble.h>

static Window* s_window;
static Layer* s_face_layer;
static Layer* s_hands_layer;
static GPoint center;

static int timedata[3];

static void draw_hands(Layer* layer, GContext* ctx) {
    // define center
    graphics_context_set_stroke_color(ctx, GColorWhite);
    // basically, take the center coordinate, run the current time divided by the highest through sin() and cos(), set the length to 64 for minutes, 32 for hours
    GPoint hrs = (GPoint) {
        .x = (int16_t)(center.x + sin_lookup((TRIG_MAX_ANGLE * timedata[0] / 12) + (TRIG_MAX_ANGLE * timedata[1] / 720.0f)) * 40 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + -cos_lookup((TRIG_MAX_ANGLE * timedata[0] / 12) + (TRIG_MAX_ANGLE * timedata[1] / 720.0f)) * 40 / TRIG_MAX_RATIO)
    };
    GPoint min = (GPoint) {
        .x = (int16_t)(center.x + sin_lookup((TRIG_MAX_ANGLE * timedata[1] / 60) + (TRIG_MAX_ANGLE * timedata[2] / 3600.0f)) * 61 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + -cos_lookup((TRIG_MAX_ANGLE * timedata[1] / 60) + (TRIG_MAX_ANGLE * timedata[2] / 3600.0f)) * 61 / TRIG_MAX_RATIO)
    };
    // seconds get some extra attention here-- the same as above, but with an extra little counterweight type thing on the opposite site
    GPoint sec0 = (GPoint) {
        .x = (int16_t)(center.x + -sin_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 9 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + cos_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 9 / TRIG_MAX_RATIO)
    };
    GPoint sec1 = (GPoint) {
        .x = (int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 61 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 61 / TRIG_MAX_RATIO)
    };
    // set the width/color for everything and draw
    graphics_context_set_stroke_width(ctx, 5);
    graphics_draw_line(ctx, center, hrs);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx, center, min);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_stroke_color(ctx, GColorYellow );
    graphics_draw_line(ctx, center, sec1);
    graphics_context_set_stroke_width(ctx, 5);
    graphics_draw_line(ctx, center, sec0);
    graphics_context_set_fill_color(ctx, GColorYellow );
    graphics_fill_circle(ctx, center, 5);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_pixel(ctx, center);
}

static void draw_face(Layer* layer, GContext* ctx) {
    graphics_context_set_stroke_color(ctx, GColorLightGray);
    // draw 60 minute ticks around clock
    for (int i = 0; i < 60; i++) {
        // tic0 = start of inner point
        // tic1 = end of outer point
        GPoint tic0, tic1;
        // draw the hour ones longer
        if (i % 5 == 0) {
            tic0 = (GPoint) {
                .x = (int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 55 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 55 / TRIG_MAX_RATIO)
            };
            tic1 = (GPoint) {
                .x = (int16_t)(tic0.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 16 / TRIG_MAX_RATIO), .y = (int16_t)(tic0.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 16 / TRIG_MAX_RATIO)
            };
        } else {
            tic0 = (GPoint) {
                .x = (int16_t)(center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 63 / TRIG_MAX_RATIO), .y = (int16_t)(center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 63 / TRIG_MAX_RATIO)
            };
            tic1 = (GPoint) {
                .x = (int16_t)(tic0.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 8 / TRIG_MAX_RATIO), .y = (int16_t)(tic0.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 8 / TRIG_MAX_RATIO)
            };
        }
        graphics_draw_line(ctx, tic0, tic1);
    }
    // draw outline and hands
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_circle(ctx, center, 71);
}

static void main_window_load(Window* window) {
    // self-explanatory: make the main window black with one full-screen layer to display the time
    window_set_background_color(window, GColorBlack);
    s_face_layer = layer_create(GRect(0, 0, 144, 168));
    s_hands_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(s_face_layer, draw_face);
    layer_set_update_proc(s_hands_layer, draw_hands);
    layer_add_child(window_get_root_layer(window), s_face_layer);
    layer_add_child(window_get_root_layer(window), s_hands_layer);
    layer_mark_dirty(s_face_layer);
    layer_mark_dirty(s_hands_layer);
}

static void main_window_unload(Window* window) {
    layer_destroy(s_face_layer);
    layer_destroy(s_hands_layer);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
    // just store the time, don't bother with making a new one in the update_proc or calling update_time
    timedata[0] = tick_time->tm_hour % 12;
    timedata[1] = tick_time->tm_min;
    timedata[2] = tick_time->tm_sec;
    layer_mark_dirty(s_hands_layer);
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
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    window_stack_push(s_window, true);
}


static void deinit() {
    window_destroy(s_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
