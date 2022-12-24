#include "view_two.h"

static const NotificationSequence led_color = {
    &message_red_255,
    &message_green_255,
    &message_blue_0,
    &message_do_not_reset,
    NULL,
};

static void handle_enter(void* ctx) {
    furi_assert(ctx);
    struct AppView* view = ctx;
    notification_message(view->app->notifications, &led_color);
}

static void handle_exit(void* ctx) {
    furi_assert(ctx);
    struct AppView* view = ctx;
    notification_message(view->app->notifications, &sequence_reset_rgb);
}

static uint32_t handle_back(void* ctx) {
    UNUSED(ctx);
    return ViewMenu;
}

static bool handle_input(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    struct AppView* view = ctx;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        view_dispatcher_switch_to_view(view->app->view_dispatcher, ViewMenu);
        return true;
    }
    return false;
}

static void handle_draw(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "2!");
}

struct ViewConfig view_two_config = {
    .id = ViewTwo,
    .handle_enter = handle_enter,
    .handle_exit = handle_exit,
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
