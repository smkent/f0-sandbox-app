#include "app.h"
#include "view_main.h"

static struct AppViewState views[] = {
    {
        .config = &view_main_config,
    },
};

static const unsigned views_count = COUNT_OF(views);

static const uint32_t queue_size = 8;

static uint32_t app_exit(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}

static void submenu_callback(void* ctx, uint32_t index) {
    furi_assert(ctx);
    app_t* app = ctx;
    if(index == MenuMain) {
        app->view_id = ViewMain;
        view_dispatcher_switch_to_view(app->view_dispatcher, ViewMain);
    }
}

static void app_views_free(app_t* app) {
    for(unsigned i = 0; i < views_count; i++) {
        furi_assert(views[i].context);
        view_dispatcher_remove_view(app->view_dispatcher, views[i].config->id);
        view_free(views[i].context->view);
        free(views[i].context);
    }
    view_dispatcher_remove_view(app->view_dispatcher, ViewMenu);
    submenu_free(app->submenu);
}

static app_t* app_views_alloc(app_t* app) {
    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Main", MenuMain, submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), app_exit);
    view_dispatcher_add_view(app->view_dispatcher, ViewMenu, submenu_get_view(app->submenu));
    app->view_id = ViewMenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);
    for(unsigned i = 0; i < views_count; i++) {
        views[i].context = malloc(sizeof(struct AppView));
        views[i].context->view = view_alloc();
        views[i].context->app = app;
        view_set_context(views[i].context->view, views[i].context);
        view_set_enter_callback(views[i].context->view, views[i].config->handle_enter);
        view_set_exit_callback(views[i].context->view, views[i].config->handle_exit);
        view_set_draw_callback(views[i].context->view, views[i].config->handle_draw);
        view_set_input_callback(views[i].context->view, views[i].config->handle_input);
        view_dispatcher_add_view(
            app->view_dispatcher, views[i].config->id, views[i].context->view);
        view_set_previous_callback(views[i].context->view, views[i].config->handle_back);
    }
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
    if(!(app->queue = furi_message_queue_alloc(queue_size, sizeof(event_t)))) {
        goto bail;
    }
    if(!(app->notifications = furi_record_open(RECORD_NOTIFICATION))) {
        goto bail;
    }
    return app_views_alloc(app);
bail:
    app_free(app);
    return NULL;
}

int32_t app_entry_point(void) {
    app_t* app = NULL;
    srand(DWT->CYCCNT);
    if(!(app = app_alloc())) {
        return 255;
    }
    view_dispatcher_run(app->view_dispatcher);
    app_free(app);
    return 0;
}
