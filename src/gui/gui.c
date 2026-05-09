#include <curl/curl.h>
#include <curl/easy.h>
#include <gtk/gtk.h>
#include <pthread.h>

#include "../../include/api.h"

#define SPACING 8

///

typedef struct
{
    CURL *curl;
    const char *address;
    const char *token;
} global_data;

///

typedef struct
{
    GtkWidget *stack;

    GtkWidget *url_label;
    GtkWidget *url;

    GtkWidget *login_label;
    GtkWidget *login;

    GtkWidget *password_label;
    GtkWidget *password;

    GtkWidget *button;

    global_data *gdata;
} login_box;

static void get_login(GtkButton *button, gpointer user_data)
{
    login_box *l = user_data;

    char *url = g_strdup(gtk_editable_get_text(GTK_EDITABLE(l->url)));
    char *login = g_strdup(gtk_editable_get_text(GTK_EDITABLE(l->login)));
    char *password = g_strdup(gtk_editable_get_text(GTK_EDITABLE(l->password)));

    g_print("url: %s\nlogin: %s\npassword: %s\n", url, login, password);

    if (url[0] && login[0] && password[0])
    {
        pthread_t curl_thread;
        auth_args *args = g_new(auth_args, 1);

        args->curl = l->gdata->curl;
        args->address = url;
        args->login = login;
        args->pass = password;

        curl_get_result *result = NULL;
        pthread_create(&curl_thread, NULL, auth, (void*) args);
        pthread_join(curl_thread, (void**) &result);

        if (result->status == CURLE_OK && (unsigned char) result->text[0] != 0xd0)
        {
            l->gdata->token = result->text;
            l->gdata->address = g_strdup(url);

            gtk_stack_set_visible_child_name(GTK_STACK(l->stack), "main");
        }
    }

    g_free(url);
    g_free(login);
    g_free(password);

    return;
}

static GtkWidget *create_login(GtkWidget *stack, global_data *gdata)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);
    login_box *lb = g_new(login_box, 1);

    lb->stack = stack;

    lb->url_label = gtk_label_new("Server address");
    lb->url = gtk_entry_new();

    lb->login_label = gtk_label_new("Username");
    lb->login = gtk_entry_new();

    lb->password_label = gtk_label_new("Password");
    lb->password = gtk_password_entry_new();

    lb->button = gtk_button_new_with_label("Login");
    gtk_widget_set_margin_top(lb->button, SPACING * 3);

    gtk_box_append(GTK_BOX(box), lb->url_label);
    gtk_box_append(GTK_BOX(box), lb->url);

    gtk_box_append(GTK_BOX(box), lb->login_label);
    gtk_box_append(GTK_BOX(box), lb->login);

    gtk_box_append(GTK_BOX(box), lb->password_label);
    gtk_box_append(GTK_BOX(box), lb->password);

    gtk_box_append(GTK_BOX(box), lb->button);

    gtk_widget_set_size_request(lb->url, 480, -1);
    gtk_widget_set_size_request(lb->login, 480, -1);
    gtk_widget_set_size_request(lb->password, 480, -1);
    gtk_widget_set_size_request(lb->button, 480, -1);

    g_signal_connect(lb->button, "clicked", G_CALLBACK(get_login), lb);

    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);

    gtk_widget_set_margin_start(box, SPACING);
    gtk_widget_set_margin_end(box, SPACING);
    gtk_widget_set_margin_top(box, SPACING);
    gtk_widget_set_margin_bottom(box, SPACING);

    lb->gdata = gdata;

    return box;
}

///

typedef struct
{
    GtkWidget *entry;
    GtkWidget *popup;
}
calendar_popup;

static void set_date(GtkCalendar *calendar, gpointer udata)
{
    calendar_popup *cp = udata;

    g_autoptr(GDateTime) dt = gtk_calendar_get_date(calendar);
    if (!dt) return;

    char *text = g_date_time_format(dt, "%F");

    gtk_editable_set_text(GTK_EDITABLE(cp->entry), text);
    g_free(text);

    gtk_popover_popdown(GTK_POPOVER(cp->popup));

    return;
}

typedef struct
{
    GtkWidget *text;
    GtkWidget *entry;
    GtkWidget *button;
} datepicker_row;

static datepicker_row create_datepicker(const char *label)
{
    GtkWidget *text = gtk_label_new(label);

    calendar_popup *cp = g_new(calendar_popup, 1);
    cp->entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(cp->entry), "YYYY-MM-DD");

    GtkWidget *button = gtk_menu_button_new();
    gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(button), "view-calendar-symbolic");

    cp->popup = gtk_popover_new();
    GtkWidget *calendar = gtk_calendar_new();
    gtk_popover_set_child(GTK_POPOVER(cp->popup), calendar);

    gtk_menu_button_set_popover(GTK_MENU_BUTTON(button), cp->popup);

    g_signal_connect(calendar, "day-selected", G_CALLBACK(set_date), cp);

    datepicker_row row = {.text = text, .entry = cp->entry, .button = button};

    return row;
}

static GtkWidget *create_shifts()
{
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(grid), SPACING);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);

    datepicker_row date_from = create_datepicker("Date from");
    datepicker_row date_to = create_datepicker("Date to");

    gtk_label_set_xalign(GTK_LABEL(date_from.text), 0.0);
    gtk_label_set_xalign(GTK_LABEL(date_to.text), 0.0);

    GtkWidget *button = gtk_button_new_with_label("Refresh");

    gtk_grid_attach(GTK_GRID(grid), date_from.text, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_from.entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_from.button, 2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), date_to.text, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_to.entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_to.button, 2, 1, 1, 1);

    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), button);

    return box;
}

//

static GtkWidget *create_sidebar_stack()
{
    GtkWidget *stack = gtk_stack_new();
    gtk_stack_add_titled(GTK_STACK(stack), create_shifts(), "shifts", "Cash shifts");
    return stack;
}

static GtkWidget *create_sidebar(GtkWidget *stack)
{
    GtkWidget *sidebar = gtk_stack_sidebar_new();
    gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(sidebar), GTK_STACK(stack));
    return sidebar;
}

//

typedef struct
{
    GtkWidget *stack;
    global_data *gdata;
} quit_main_data;

static void quit_main(GSimpleAction *action, GVariant *parameter, gpointer ptr)
{
    g_print("Quitting!\n");

    quit_main_data *qmd = ptr;

    logout_args *args = g_new(logout_args, 1);

    args->curl = qmd->gdata->curl;
    args->address = qmd->gdata->address;
    args->token = qmd->gdata->token;

    curl_get_result *result = NULL;
    pthread_t logout_thread;
    pthread_create(&logout_thread, NULL, logout, args);
    pthread_join(logout_thread, (void**) &result);

    if (result->status == CURLE_OK)
    {
        gtk_stack_set_visible_child_name(GTK_STACK(qmd->stack), "login");
    }

    else
    {
        g_print("Something strange is about to happen...\n");
    }

    return;
}

static GtkWidget *create_menubar(GtkApplication *app, GtkWidget *stack, global_data *gdata)
{
    GMenu *menu = g_menu_new();
    GMenu *menu_file = g_menu_new();

    GSimpleAction *quit_action = g_simple_action_new("quit", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(quit_action));

    quit_main_data *qmd = g_new(quit_main_data, 1);
    qmd->stack = stack;
    qmd->gdata = gdata;

    g_signal_connect(quit_action, "activate", G_CALLBACK(quit_main), qmd);

    g_menu_append(menu_file, "Quit", "app.quit");
    g_menu_append_submenu(menu, "File", G_MENU_MODEL(menu_file));

    return gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu));
}

//

typedef struct
{
    const char *token;

    GtkWidget *stack;
    GtkWidget *sidebar;
} main_view;

static GtkWidget *create_main(GtkApplication *app, GtkWidget *stack, global_data *gdata)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, SPACING);
    main_view *mv = g_new(main_view, 1);

    mv->stack = create_sidebar_stack();
    mv->sidebar = create_sidebar(mv->stack);
    gtk_box_append(GTK_BOX(box), mv->sidebar);
    gtk_box_append(GTK_BOX(box), mv->stack);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);
    GtkWidget *menubar = create_menubar(app, stack, gdata);
    gtk_box_append(GTK_BOX(vbox), menubar);
    gtk_box_append(GTK_BOX(vbox), box);

    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);

    return vbox;
}

//

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

//

int gui_start(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    global_data *gdata = g_new(global_data, 1);
    gdata->curl = curl_easy_init();
    gdata->address = NULL;
    gdata->token = NULL;

    app = gtk_application_new("org.iiko.office", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), gdata);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    curl_easy_cleanup(gdata->curl);

    return status;
}

///
