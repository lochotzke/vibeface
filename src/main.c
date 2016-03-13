#include <pebble.h>

#define KEY_VIBRATE 0

static Window *s_main_window;
static TextLayer *s_time_layer;
static AppTimer *s_vibe_timer;

static void vibe_timer_callback(void *data)
{
    // multiple vibes
//     vibes_long_pulse(); // 500 ms
//     s_vibe_timer = app_timer_register(750, vibe_timer_callback, NULL);

    // one continious vibe
    vibes_enqueue_custom_pattern((VibePattern) {
        .durations = (uint32_t []) { 10000 },
        .num_segments = 1
    });
    s_vibe_timer = app_timer_register(10000, vibe_timer_callback, NULL);
}

static void inbox_received_callback(DictionaryIterator *iter, void *ctx)
{
    Tuple *vibrate_tuple = dict_find(iter, KEY_VIBRATE);

    if (vibrate_tuple) {
        if (s_vibe_timer) {
            app_timer_cancel(s_vibe_timer);
            s_vibe_timer = NULL;
        }
        vibes_cancel();

        if (vibrate_tuple->value->int16) {
            vibe_timer_callback(NULL);
        }
    }
}

static void update_time()
{
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    update_time();
}

static void main_window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window)
{
    text_layer_destroy(s_time_layer);
}

static void init()
{
    if (!s_main_window) {
        s_main_window = window_create();
        window_set_background_color(s_main_window, GColorBlack);
        window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
        });
    }
    window_stack_push(s_main_window, true);

    update_time();
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    app_message_register_inbox_received(inbox_received_callback);
    app_message_open(8, 8);
}

static void deinit()
{
    window_destroy(s_main_window);
    s_main_window = NULL;
}

int main()
{
    init();
    app_event_loop();
    deinit();
}

