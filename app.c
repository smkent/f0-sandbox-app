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

static void event_callback(InputEvent* event, FuriMessageQueue* queue) {
    event_t message = {
        .type = KEY,
        .event = *event,
    };
    furi_message_queue_put(queue, &message, FuriWaitForever);
}

static inline bool handle_key_press(event_t* msg) {
    switch(msg->event.type) {
    case InputTypeRelease:
        if(msg->event.key == InputKeyBack) {
            return false;
        }
        break;
    default:
        break;
    }
    return true;
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

static void app_free(app_t* app) {
    furi_assert(app);
    if(app->notifications) {
        furi_record_close(RECORD_NOTIFICATION);
    }
    if(app->queue) {
        furi_message_queue_free(app->queue);
    }
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
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(app->view_dispatcher);
    if(!(app->queue = furi_message_queue_alloc(QUEUE_SIZE, sizeof(event_t)))) {
        goto bail;
    }
    view_port_input_callback_set(app->view_port, event_callback, app->queue);
    view_port_draw_callback_set(app->view_port, draw_callback, NULL);
    if(!(app->notifications = furi_record_open(RECORD_NOTIFICATION))) {
        goto bail;
    }
    return app;

bail:
    app_free(app);
    return NULL;
}

static bool event_handler(event_t msg) {
    switch(msg.type) {
    case KEY:
        if(!handle_key_press(&msg)) {
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

static void run_event_loop(app_t* app) {
    event_t msg = {0};
    while(furi_message_queue_get(app->queue, &msg, FuriWaitForever) == FuriStatusOk) {
        if(!event_handler(msg)) {
            break;
        }
        view_port_update(app->view_port);
    }
}

static const NotificationMessage led_green_88 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 0x88,
};

static const NotificationSequence sequence_led_color = {
    &message_red_0,
    &led_green_88,
    &message_blue_255,
    &message_do_not_reset,
    NULL,
};

int32_t app_entry_point(void) {
    int32_t error = 255;
    app_t* app = NULL;
    srand(DWT->CYCCNT);
    if(!(app = app_alloc())) {
        return error;
    }
    notification_message(app->notifications, &sequence_led_color);
    run_event_loop(app);
    notification_message(app->notifications, &sequence_reset_rgb);
    error = 0;
    app_free(app);
    return error;
}
