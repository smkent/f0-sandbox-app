#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <core/common_defines.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/gui_i.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include "app.h"
#include "sandbox_app_icons.h"

#define QUEUE_SIZE 8

const Icon* icons[] = {
    &I_icon_question_block,
    &I_icon_coin,
    &I_icon_goomba,
    &I_icon_mail_stamp,
    &I_icon_mcrn,
};
const unsigned icons_count = (sizeof(icons) / sizeof(icons[0]));

static uint32_t app_to_menu(void* ctx) {
    UNUSED(ctx);
    return ViewMenu;
}

static uint32_t app_main_back(void* ctx) {
    furi_assert(ctx);
    view_main_t* view_main = ctx;
    notification_message(view_main->app->notifications, &sequence_reset_rgb);
    return app_to_menu(ctx);
}

static uint32_t app_exit(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
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

static void submenu_callback(void* ctx, uint32_t index) {
    furi_assert(ctx);
    app_t* app = ctx;
    if(index == MenuMain) {
        app->view_id = ViewMain;
        notification_message(app->notifications, &sequence_led_color);
        view_dispatcher_switch_to_view(app->view_dispatcher, ViewMain);
    }
}

static void app_views_free(app_t* app) {
    furi_assert(app->view_main);
    view_dispatcher_remove_view(app->view_dispatcher, ViewMain);
    view_free(app->view_main->view);
    free(app->view_main);
}

static app_t* app_views_alloc(app_t* app) {
    app->view_main = malloc(sizeof(view_main_t));
    app->view_main->view = view_alloc();
    app->view_main->app = app;
    view_set_context(app->view_main->view, app->view_main);
    view_set_draw_callback(app->view_main->view, draw_callback);
    view_set_input_callback(app->view_main->view, event_callback);

    view_dispatcher_add_view(app->view_dispatcher, ViewMain, app->view_main->view);
    view_set_previous_callback(app->view_main->view, app_main_back);

    return app;
}

static void app_free(app_t* app) {
    furi_assert(app);
    app_views_free(app);
    if(app->notifications) {
        furi_record_close(RECORD_NOTIFICATION);
    }
    if(app->queue) {
        furi_message_queue_free(app->queue);
    }
    view_dispatcher_remove_view(app->view_dispatcher, ViewMenu);
    submenu_free(app->submenu);
    if(app->view_dispatcher) {
        view_dispatcher_free(app->view_dispatcher);
    }
    if(app->view_port) {
        gui_remove_view_port(app->gui, app->view_port);
    }
    if(app->view_port) {
        view_port_enabled_set(app->view_port, false);
        view_port_free(app->view_port);
    }
    if(app->gui) {
        furi_record_close(RECORD_GUI);
    }
    free(app);
}

static app_t* app_alloc() {
    app_t* app = NULL;
    if(!(app = malloc(sizeof(app_t)))) {
        return NULL;
    }
    if(!(app->gui = furi_record_open(RECORD_GUI))) {
        goto bail;
    }
    if(!(app->view_port = view_port_alloc())) {
        goto bail;
    }
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    if(!(app->view_dispatcher = view_dispatcher_alloc())) {
        goto bail;
    }
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    if(!(app->queue = furi_message_queue_alloc(QUEUE_SIZE, sizeof(event_t)))) {
        goto bail;
    }
    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Main", MenuMain, submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), app_exit);
    view_dispatcher_add_view(app->view_dispatcher, ViewMenu, submenu_get_view(app->submenu));
    app->view_id = ViewMenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);

    if(!(app->notifications = furi_record_open(RECORD_NOTIFICATION))) {
        goto bail;
    }
    return app_views_alloc(app);

bail:
    app_free(app);
    return NULL;
}

int32_t app_entry_point(void) {
    int32_t error = 255;
    app_t* app = NULL;
    srand(DWT->CYCCNT);
    if(!(app = app_alloc())) {
        return error;
    }
    view_dispatcher_run(app->view_dispatcher);
    error = 0;
    app_free(app);
    return error;
}
