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
