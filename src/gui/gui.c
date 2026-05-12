#include "login.h"
#include "main_view.h"

#define SPACING 8

static GtkWidget *create_stack(GtkApplication *app, gpointer gd)
{
    GtkWidget *stack = gtk_stack_new();
    GtkWidget *login = create_login(stack, gd);
    GtkWidget *main = create_main(app, stack, gd);

    gtk_stack_add_named(GTK_STACK(stack), login, "login");
    gtk_stack_add_named(GTK_STACK(stack), main, "main");

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "login");

    return stack;
}

static void activate(GtkApplication *app, gpointer gd)
{
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *stack = create_stack(app, gd);

    gtk_window_set_title(GTK_WINDOW(window), "iiko-office");
    gtk_window_set_child(GTK_WINDOW(window), stack);
    gtk_window_present(GTK_WINDOW(window));

    return;
}

int gui_start(int argc, char **argv)
{
    global_data *gdata = g_new(global_data, 1);

    const char *const *langs = g_get_language_names();

    if (langs && langs[0] && strncmp(langs[0], "ru", 2) == 0)
        gdata->current_lang = LANG_RU;

    else gdata->current_lang = LANG_EN;

    gdata->curl = curl_easy_init();
    gdata->address = NULL;
    gdata->token = NULL;

    GtkApplication *app = gtk_application_new("org.iiko.office", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), gdata);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    curl_easy_cleanup(gdata->curl);

    return status;
}
