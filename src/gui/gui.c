#include "login.h"
#include "main_view.h"

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

    init_css(gtk_widget_get_display(window));

    gtk_window_set_title(GTK_WINDOW(window), "iiko Office");
    gtk_window_set_child(GTK_WINDOW(window), stack);
    gtk_window_present(GTK_WINDOW(window));

    return;
}

static global_data * global_data_init(void)
{
    global_data *gdata = g_new(global_data, 1);

    const char *const *langs = g_get_language_names();

    lang_id lang = (langs && langs[0] && strncmp(langs[0], "ru", 2) == 0) ? LANG_RU : LANG_EN;

    GMutex lock;
    g_mutex_init(&lock);

    memcpy(gdata, &(global_data) {.lock = lock, .address = NULL, .login = NULL, .password = NULL, .token = NULL, .current_lang = lang}, sizeof(global_data));

    return gdata;
}

int gui_start(int argc, char **argv)
{
    global_data *gdata = global_data_init();

    g_autoptr(GtkApplication) app = gtk_application_new("org.iiko.office", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), gdata);

    return g_application_run(G_APPLICATION(app), argc, argv);
}
