#include "common.h"

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

typedef struct
{
    GtkWidget *stack;
    GtkWidget *button;

    auth_args args;
    curl_get_result *result;
    global_data *gdata;
} auth_thread_args;

static gboolean auth_thread_done(gpointer udata)
{
    auth_thread_args *thread_args = udata;
    curl_get_result *result = thread_args->result;

    if (result->status == CURLE_OK && result->text[0] != 0 && (unsigned char) result->text[0] != 0xd0)
    {
        thread_args->gdata->address = thread_args->args.address;
        thread_args->gdata->token = result->text;

        gtk_stack_set_visible_child_name(GTK_STACK(thread_args->stack), "main");
    }

    gtk_widget_set_sensitive(GTK_WIDGET(thread_args->button), TRUE);

    g_free(thread_args->args.login);
    g_free(thread_args->args.pass);

    g_free(thread_args);

    return G_SOURCE_REMOVE;
}

static gpointer auth_thread(gpointer udata)
{
    auth_thread_args *thread_args = udata;

    thread_args->result = auth(thread_args->args);
    g_idle_add(auth_thread_done, thread_args);

    return NULL;
}

static void auth_callback(GtkButton *button, gpointer udata)
{
    login_box *l = udata;

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    char *url = g_strdup(gtk_editable_get_text(GTK_EDITABLE(l->url)));
    char *login = g_strdup(gtk_editable_get_text(GTK_EDITABLE(l->login)));
    char *password = g_strdup(gtk_editable_get_text(GTK_EDITABLE(l->password)));

    if (url[0] && login[0] && password[0])
    {
        auth_thread_args *thread_args = g_new(auth_thread_args, 1);

        thread_args->stack = l->stack;
        thread_args->button = l->button;
        thread_args->args = (auth_args) {.address = url, .login = login, .pass = password};
        thread_args->gdata = l->gdata;

        g_thread_unref(g_thread_new("auth", auth_thread, thread_args));
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
    login_box *lb = g_new(login_box, 1);

    lb->stack = stack;

    lb->url_label = gtk_label_new(_(gdata->current_lang, STR_URL));
    lb->url = gtk_entry_new();

    lb->login_label = gtk_label_new(_(gdata->current_lang, STR_USERNAME));
    lb->login = gtk_entry_new();

    lb->password_label = gtk_label_new(_(gdata->current_lang, STR_PASSWORD));
    lb->password = gtk_password_entry_new();

    lb->button = gtk_button_new_with_label(_(gdata->current_lang, STR_LOGIN));
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

    g_signal_connect(lb->button, "clicked", G_CALLBACK(auth_callback), lb);

    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);

    gtk_widget_set_margin_start(box, SPACING);
    gtk_widget_set_margin_end(box, SPACING);
    gtk_widget_set_margin_top(box, SPACING);
    gtk_widget_set_margin_bottom(box, SPACING);

    lb->gdata = gdata;

    return box;
}
