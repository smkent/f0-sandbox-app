#include "view_main.h"

static const Icon* icons[] = {
    &I_icon_question_block,
    &I_icon_coin,
    &I_icon_goomba,
    &I_icon_mail_stamp,
    &I_icon_mcrn,
};

static const unsigned icons_count = (sizeof(icons) / sizeof(icons[0]));

static uint32_t app_to_menu(void* ctx) {
    UNUSED(ctx);
    return ViewMenu;
}

static uint32_t app_main_back(void* ctx) {
    furi_assert(ctx);
    struct AppView* view_main = ctx;
    notification_message(view_main->app->notifications, &sequence_reset_rgb);
    return app_to_menu(ctx);
}

static bool event_callback(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* queue = ctx;
    UNUSED(queue);
    UNUSED(event);
    return false;
}

static void draw_callback(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "*-*");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "Sandbox App");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 64, AlignCenter, AlignBottom, "Bonus text");
    const unsigned y_start = (64 - (11 * icons_count)) / 2;
    for(unsigned i = 0; i < icons_count; i++) {
        canvas_draw_icon(canvas, 0, y_start + 11 * i, icons[i]);
    }
}

struct ViewConfig view_main_config = {
    .id = ViewMain,
    .handle_back = app_main_back,
    .handle_input = event_callback,
    .handle_draw = draw_callback,
};
