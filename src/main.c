#include <pebble.h>

static Window* s_window;
static Layer* s_layer;

static int timedata[3];

static void update_proc(Layer* layer, GContext* ctx) {
    GPoint center = (GPoint) {
        .x = 72, .y = 84
    };
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    for (int i = 0; i < 60; i++) {
        // y start = 147
        // x start = 0
        // tic0 = inner
        // tic1 = outer
        GPoint tic0, tic1;
        if (i % 5 == 0) {
            tic0 = (GPoint) {
                .x = (center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 55 / TRIG_MAX_RATIO), .y = (center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 55 / TRIG_MAX_RATIO)
            }; //outer
            tic1 = (GPoint) {
                .x = (tic0.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 16 / TRIG_MAX_RATIO), .y = (tic0.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 16 / TRIG_MAX_RATIO)
            };
        } else {
            tic0 = (GPoint) {
                .x = (center.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 63 / TRIG_MAX_RATIO), .y = (center.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 63 / TRIG_MAX_RATIO)
            }; //outer
            tic1 = (GPoint) {
                .x = (tic0.x + sin_lookup(TRIG_MAX_ANGLE * i / 60) * 8 / TRIG_MAX_RATIO), .y = (tic0.y + -cos_lookup(TRIG_MAX_ANGLE * i / 60) * 8 / TRIG_MAX_RATIO)
            };
        }
        graphics_draw_line(ctx, tic0, tic1);
    }
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_circle(ctx, center, 71);
    GPoint hrs = (GPoint) {
        .x = (center.x + sin_lookup(TRIG_MAX_ANGLE * timedata[0] / 12) * 32 / TRIG_MAX_RATIO), .y = (center.y + -cos_lookup(TRIG_MAX_ANGLE * timedata[0] / 12) * 32 / TRIG_MAX_RATIO)
    };
    GPoint min = (GPoint) {
        .x = (center.x + sin_lookup(TRIG_MAX_ANGLE * timedata[1] / 60) * 64 / TRIG_MAX_RATIO), .y = (center.y + -cos_lookup(TRIG_MAX_ANGLE * timedata[1] / 60) * 64 / TRIG_MAX_RATIO)
    };
    GPoint sec0 = (GPoint) {
        .x = (center.x + -sin_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 16 / TRIG_MAX_RATIO), .y = (center.y + cos_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 16 / TRIG_MAX_RATIO)
    };
    GPoint sec1 = (GPoint) {
        .x = (center.x + sin_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 64 / TRIG_MAX_RATIO), .y = (center.y + -cos_lookup(TRIG_MAX_ANGLE * timedata[2] / 60) * 64 / TRIG_MAX_RATIO)
    };
    graphics_draw_line(ctx, center, hrs);
    graphics_draw_line(ctx, center, min);
    graphics_context_set_stroke_color(ctx, GColorYellow );
    graphics_draw_line(ctx, center, sec0);
    graphics_draw_line(ctx, center, sec1);
    graphics_context_set_fill_color(ctx, GColorYellow );
    graphics_fill_circle(ctx, center, 3);
}

static void main_window_load(Window* window) {
    window_set_background_color(window, GColorBlack );
    s_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(s_layer, update_proc);
    layer_add_child(window_get_root_layer(window), s_layer);
}

static void main_window_unload(Window* window) {
    layer_destroy(s_layer);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
    timedata[0] = tick_time->tm_hour % 12;
    timedata[1] = tick_time->tm_min;
    timedata[2] = tick_time->tm_sec;
    layer_mark_dirty(s_layer);
}

static void init() {
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = main_window_load,
         .unload = main_window_unload
    });
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    window_stack_push(s_window, true);
    layer_mark_dirty(s_layer);
}


static void deinit() {
    window_destroy(s_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
