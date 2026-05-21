#include "common.h"
#include "../api/api.h"

typedef struct
{
    global_data *gdata;

    GtkWidget *stack;

    GtkWidget *url_label;
    GtkWidget *url;

    GtkWidget *login_label;
    GtkWidget *login;

    GtkWidget *password_label;
    GtkWidget *password;

    GtkWidget *button;
} login_box;

typedef struct
{
    global_data *gdata;

    GtkWidget *stack;
    GtkWidget *button;

    GtkWidget *url;
    GtkWidget *login;
    GtkWidget *password;

    auth_args args;
    curl_get_result *result;
} auth_callback_args;

static gboolean auth_thread_done(gpointer udata)
{
    auth_callback_args *callback_args = udata;
    curl_get_result *result = callback_args->result;

    if (result->status == CURLE_OK && result->text[0] != 0 && (unsigned char) result->text[0] != 0xd0)
    {
        g_mutex_lock(&callback_args->gdata->lock);

        g_free(callback_args->gdata->address);
        g_free(callback_args->gdata->login);
        g_free(callback_args->gdata->password);
        g_free(callback_args->gdata->token);

        callback_args->gdata->address = callback_args->args.address;
        callback_args->gdata->login = callback_args->args.login;
        callback_args->gdata->password = callback_args->args.pass;
        callback_args->gdata->token = g_strdup(result->text);

        g_mutex_unlock(&callback_args->gdata->lock);

        gtk_stack_set_visible_child_name(GTK_STACK(callback_args->stack), "main");
    }

    else
    {
        g_free(callback_args->args.address);
        g_free(callback_args->args.login);
        g_free(callback_args->args.pass);
    }

    curl_get_cleanup(result);
    callback_args->result = NULL;

    gtk_widget_set_sensitive(GTK_WIDGET(callback_args->button), TRUE);

    return G_SOURCE_REMOVE;
}

static gpointer auth_thread(gpointer udata)
{
    auth_callback_args *callback_args = udata;

    callback_args->result = auth(callback_args->args);
    g_idle_add(auth_thread_done, callback_args);

    return NULL;
}

static void auth_callback(GtkButton *button, gpointer udata)
{
    auth_callback_args *callback_args = udata;

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    char *url = g_strdup(gtk_editable_get_text(GTK_EDITABLE(callback_args->url)));
    char *login = g_strdup(gtk_editable_get_text(GTK_EDITABLE(callback_args->login)));
    char *password = g_strdup(gtk_editable_get_text(GTK_EDITABLE(callback_args->password)));

    if (url[0] && login[0] && password[0])
    {
        callback_args->args = (auth_args) {.address = url, .login = login, .pass = password};

        g_thread_unref(g_thread_new("auth", auth_thread, callback_args));
    }

    else
    {
        g_free(url);
        g_free(login);
        g_free(password);
        gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
    }

    return;
}

GtkWidget *create_login(GtkWidget *stack, global_data *gdata)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);

    GtkWidget *url_label = gtk_label_new(_(gdata->current_lang, STR_URL));
    GtkWidget *url = gtk_entry_new();

    GtkWidget *login_label = gtk_label_new(_(gdata->current_lang, STR_USERNAME));
    GtkWidget *login = gtk_entry_new();

    GtkWidget *password_label = gtk_label_new(_(gdata->current_lang, STR_PASSWORD));
    GtkWidget *password = gtk_password_entry_new();

    GtkWidget *button = gtk_button_new_with_label(_(gdata->current_lang, STR_LOGIN));
    gtk_widget_set_margin_top(button, SPACING * 3);

    gtk_box_append(GTK_BOX(box), url_label);
    gtk_box_append(GTK_BOX(box), url);

    gtk_box_append(GTK_BOX(box), login_label);
    gtk_box_append(GTK_BOX(box), login);

    gtk_box_append(GTK_BOX(box), password_label);
    gtk_box_append(GTK_BOX(box), password);

    gtk_box_append(GTK_BOX(box), button);

    gtk_widget_set_size_request(url, 480, -1);
    gtk_widget_set_size_request(login, 480, -1);
    gtk_widget_set_size_request(password, 480, -1);
    gtk_widget_set_size_request(button, 480, -1);

    auth_callback_args *callback_args = g_new(auth_callback_args, 1);
    *callback_args = (auth_callback_args)
    {
        .gdata = gdata,
        .stack = stack,
        .button = button,
        .url = url,
        .login = login,
        .password = password,
    };

    g_signal_connect(callback_args->button, "clicked", G_CALLBACK(auth_callback), callback_args);

    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);

    gtk_widget_set_margin_start(box, SPACING);
    gtk_widget_set_margin_end(box, SPACING);
    gtk_widget_set_margin_top(box, SPACING);
    gtk_widget_set_margin_bottom(box, SPACING);

    return box;
}
