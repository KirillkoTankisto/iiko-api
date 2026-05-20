#include "cashshifts.h"

GtkWidget *create_sidebar_stack(global_data *gdata)
{
    GtkWidget *stack = gtk_stack_new();
    gtk_stack_add_titled(GTK_STACK(stack), create_shifts(gdata), "shifts", _(gdata->current_lang, STR_CASHSHIFTS));
    return stack;
}

GtkWidget *create_sidebar(GtkWidget *stack)
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

    logout_args args =
    {
        .address = qmd->gdata->address,
        .token = qmd->gdata->token,
    };

    curl_get_result *result = logout(args);

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

    GSimpleAction *quit_action = g_simple_action_new("to_login", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(quit_action));

    quit_main_data *qmd = g_new(quit_main_data, 1);
    qmd->stack = stack;
    qmd->gdata = gdata;

    g_signal_connect(quit_action, "activate", G_CALLBACK(quit_main), qmd);

    g_menu_append(menu_file, _(gdata->current_lang, STR_QUIT), "app.to_login");
    g_menu_append_submenu(menu, _(gdata->current_lang, STR_FILE), G_MENU_MODEL(menu_file));

    return gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu));
}

//

typedef struct
{
    const char *token;

    GtkWidget *stack;
    GtkWidget *sidebar;
} main_view;

GtkWidget *create_main(GtkApplication *app, GtkWidget *stack, global_data *gdata)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, SPACING);
    main_view *mv = g_new(main_view, 1);

    mv->stack = create_sidebar_stack(gdata);
    mv->sidebar = create_sidebar(mv->stack);
    gtk_box_append(GTK_BOX(box), mv->sidebar);
    gtk_box_append(GTK_BOX(box), mv->stack);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);
    GtkWidget *menubar = create_menubar(app, stack, gdata);
    gtk_box_append(GTK_BOX(vbox), menubar);
    gtk_box_append(GTK_BOX(vbox), box);

    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_widget_set_hexpand(mv->stack, TRUE);
    gtk_widget_set_vexpand(mv->stack, TRUE);

    return vbox;
}

//
