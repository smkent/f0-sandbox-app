#include "view_main.h"

static const Icon* icons[] = {
    &I_icon_question_block,
    &I_icon_coin,
    &I_icon_goomba,
    &I_icon_mail_stamp,
    &I_icon_mcrn,
};

static const unsigned icons_count = COUNT_OF(icons);

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
    .handle_back = handle_back,
    .handle_input = handle_input,
    .handle_draw = handle_draw,
};
