#pragma once

#include <gui/gui.h>
#include <gui/view_dispatcher.h>

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    ViewPort* view_port;
    FuriMessageQueue* queue;
    NotificationApp* notifications;
} app_t;

typedef enum {
    KEY,
} event_type_t;

typedef struct {
    event_type_t type;
    InputEvent event;
} event_t;
