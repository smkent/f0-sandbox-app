#pragma once

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>

typedef struct app_t app_t;

typedef struct {
    View* view;
    app_t* app;
} view_main_t;

struct app_t {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewPort* view_port;
    Submenu* submenu;
    FuriMessageQueue* queue;
    NotificationApp* notifications;
    uint32_t view_id;
    view_main_t* view_main;
};

typedef enum {
    KEY,
} event_type_t;

typedef struct {
    event_type_t type;
    InputEvent event;
} event_t;

typedef enum {
    ViewMenu,
    ViewMain,
} views_t;

typedef enum {
    MenuMain,
} menu_t;

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
