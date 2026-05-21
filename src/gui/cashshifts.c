#include <gtk/gtk.h>
#include "common.h"
#include "../api/api.h"

typedef struct
{
    global_data *gdata;

    GtkWidget *date_from;
    GtkWidget *date_to;
    GtkWidget *cashshifts_table;

    cashshifts_list_args args;
} cashshifts_entries;

typedef struct
{
    GtkWidget *OpenDate;
    GtkWidget *CloseDate;
    GtkWidget *AcceptDate;
    GtkWidget *SessionStatus;
    GtkWidget *SalesSum;
    GtkWidget *SalesCash;
    GtkWidget *SalesCard;
    GtkWidget *SalesCredit;
    GtkWidget *SessionNumber;
} row;

static void add_row(size_t rnum, row r, GtkGrid *g)
{
    gtk_grid_attach(g, r.OpenDate, 0, rnum, 1, 1);
    gtk_grid_attach(g, r.CloseDate, 1, rnum, 1, 1);
    gtk_grid_attach(g, r.AcceptDate, 2, rnum, 1, 1);
    gtk_grid_attach(g, r.SessionStatus, 3, rnum, 1, 1);
    gtk_grid_attach(g, r.SalesSum, 4, rnum, 1, 1);
    gtk_grid_attach(g, r.SalesCash, 5, rnum, 1, 1);
    gtk_grid_attach(g, r.SalesCard, 6, rnum, 1, 1);
    gtk_grid_attach(g, r.SalesCredit, 7, rnum, 1, 1);
    gtk_grid_attach(g, r.SessionNumber, 8, rnum, 1, 1);

    return;
}

typedef struct
{
    global_data *gdata;

    GtkWidget *button;
    GtkWidget *cashshifts_table;

    cashshifts_list_args args;
    cashshifts_list_answer *result;
} refresh_cashshifts_table_args;

static gboolean refresh_cashshifts_table_thread_done(gpointer ptr)
{
    refresh_cashshifts_table_args *args = ptr;

    if (args->result)
    {
        GtkWidget *child = gtk_widget_get_first_child(args->cashshifts_table);
        while (child != NULL)
        {
            GtkWidget *next = gtk_widget_get_next_sibling(child);
            gtk_grid_remove(GTK_GRID(args->cashshifts_table), child);
            child = next;
        }

        size_t row_pos = 0;

        row header =
        {
            .OpenDate = gtk_label_new(_(args->gdata->current_lang, STR_OPENDATE)),
            .CloseDate = gtk_label_new(_(args->gdata->current_lang, STR_CLOSEDATE)),
            .AcceptDate = gtk_label_new(_(args->gdata->current_lang, STR_ACCEPTDATE)),
            .SessionStatus = gtk_label_new(_(args->gdata->current_lang, STR_SESSIONSTATUS)),
            .SalesSum = gtk_label_new(_(args->gdata->current_lang, STR_SALESSUM)),
            .SalesCash = gtk_label_new(_(args->gdata->current_lang, STR_SALESCASH)),
            .SalesCard = gtk_label_new(_(args->gdata->current_lang, STR_SALESCARD)),
            .SalesCredit = gtk_label_new(_(args->gdata->current_lang, STR_SALESCREDIT)),
            .SessionNumber = gtk_label_new(_(args->gdata->current_lang, STR_SESSIONNUMBER)),
        };

        gtk_widget_add_css_class(header.OpenDate, "table-cell");
        gtk_widget_add_css_class(header.CloseDate, "table-cell");
        gtk_widget_add_css_class(header.AcceptDate, "table-cell");
        gtk_widget_add_css_class(header.SessionStatus, "table-cell");
        gtk_widget_add_css_class(header.SalesSum, "table-cell");
        gtk_widget_add_css_class(header.SalesCash, "table-cell");
        gtk_widget_add_css_class(header.SalesCard, "table-cell");
        gtk_widget_add_css_class(header.SalesCredit, "table-cell");
        gtk_widget_add_css_class(header.SessionNumber, "table-cell");

        add_row(row_pos, header, GTK_GRID(args->cashshifts_table));
        row_pos++;

        for (size_t i = 0; i < args->result->size; i++)
        {
            row one_row;

            cashshifts_list_answer_element element = args->result->elements[i];

            one_row.OpenDate = gtk_label_new(reformat_date(element.openDate, args->gdata->current_lang));
            one_row.CloseDate = gtk_label_new(reformat_date(element.closeDate, args->gdata->current_lang));
            one_row.AcceptDate = gtk_label_new(reformat_date(element.acceptDate, args->gdata->current_lang));
            one_row.SessionStatus = gtk_label_new(reformat_date(element.acceptDate, args->gdata->current_lang));
            one_row.SalesSum = gtk_label_new(g_strdup_printf("%i", element.salesCard + element.salesCash + element.salesCredit));
            one_row.SalesCash = gtk_label_new(g_strdup_printf("%i", element.salesCash));
            one_row.SalesCard = gtk_label_new(g_strdup_printf("%i", element.salesCard));
            one_row.SalesCredit = gtk_label_new(g_strdup_printf("%i", element.salesCredit));
            one_row.SessionNumber = gtk_label_new(g_strdup_printf("%i", element.sessionNumber));

            gtk_label_set_xalign(GTK_LABEL(one_row.OpenDate), 0.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.CloseDate), 0.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.AcceptDate), 0.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.SessionStatus), 0.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.SalesSum), 1.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.SalesCash), 1.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.SalesCard), 1.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.SalesCredit), 1.0);
            gtk_label_set_xalign(GTK_LABEL(one_row.SessionNumber), 1.0);

            gtk_label_set_selectable(GTK_LABEL(one_row.OpenDate), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.CloseDate), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.AcceptDate), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.SessionStatus), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.SalesSum), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.SalesCash), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.SalesCard), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.SalesCredit), TRUE);
            gtk_label_set_selectable(GTK_LABEL(one_row.SessionNumber), TRUE);

            gtk_widget_add_css_class(one_row.OpenDate, "table-cell");
            gtk_widget_add_css_class(one_row.CloseDate, "table-cell");
            gtk_widget_add_css_class(one_row.AcceptDate, "table-cell");
            gtk_widget_add_css_class(one_row.SessionStatus, "table-cell");
            gtk_widget_add_css_class(one_row.SalesSum, "table-cell");
            gtk_widget_add_css_class(one_row.SalesCash, "table-cell");
            gtk_widget_add_css_class(one_row.SalesCard, "table-cell");
            gtk_widget_add_css_class(one_row.SalesCredit, "table-cell");
            gtk_widget_add_css_class(one_row.SessionNumber, "table-cell");

            add_row(row_pos, one_row, GTK_GRID(args->cashshifts_table));
            row_pos++;
        }

        cashshifts_list_destroy(args->result);
    }

    gtk_widget_set_sensitive(args->button, TRUE);

    return G_SOURCE_REMOVE;
}

static gpointer refresh_cashshifts_table_thread(gpointer ptr)
{
    refresh_cashshifts_table_args *args = ptr;

    args->result = cashshifts_list_get(args->args);

    g_idle_add(refresh_cashshifts_table_thread_done, args);

    return NULL;
}

static void refresh_cashshifts_table(GtkButton *button, gpointer udata)
{
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    cashshifts_entries *entries = udata;

    char *date_from = g_strdup(gtk_editable_get_text(GTK_EDITABLE(entries->date_from)));
    char *date_to = g_strdup(gtk_editable_get_text(GTK_EDITABLE(entries->date_to)));

    if (date_from && date_to)
    {
        refresh_cashshifts_table_args *args = g_new(refresh_cashshifts_table_args, 1);
        *args = (refresh_cashshifts_table_args)
        {
            .gdata = entries->gdata,
            .button = GTK_WIDGET(button),
            .cashshifts_table = entries->cashshifts_table,
            .args.address = entries->gdata->address,
            .args.token = entries->gdata->token,
            .args.prompt = (cashshifts_list_prompt)
            {
                .openDateFrom = date_from,
                .openDateTo = date_to,
                .departmentId = NULL,
                .groupId = NULL,
                .revision_from = -1,
                .status = "ANY",
            },
        };

        g_thread_unref(g_thread_new(NULL, refresh_cashshifts_table_thread, args));
    }

    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
    }

    return;
}

static GtkWidget *create_cashshifts_table()
{
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_halign(grid, GTK_ALIGN_FILL);

    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    return grid;
}

GtkWidget *create_cashshifts(global_data *gdata)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(grid), SPACING);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);
    gtk_widget_set_hexpand(box, TRUE);

    datepicker_row date_from = create_datepicker(_(gdata->current_lang, STR_DATE_FROM));
    datepicker_row date_to = create_datepicker(_(gdata->current_lang, STR_DATE_TO));

    gtk_label_set_xalign(GTK_LABEL(date_from.text), 0.0);
    gtk_label_set_xalign(GTK_LABEL(date_to.text), 0.0);

    GtkWidget *button = gtk_button_new_with_label(_(gdata->current_lang, STR_REFRESH));

    gtk_grid_attach(GTK_GRID(grid), date_from.text, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_from.entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_from.button, 2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), date_to.text, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_to.entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), date_to.button, 2, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), button, 1, 2, 1, 1);

    gtk_box_append(GTK_BOX(box), grid);

    GtkWidget *table_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(table_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(table_window, TRUE);
    gtk_widget_set_vexpand(table_window, TRUE);

    GtkWidget *cashshifts_table = create_cashshifts_table();

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(table_window), cashshifts_table);
    gtk_box_append(GTK_BOX(box), table_window);

    cashshifts_entries *entries = g_new(cashshifts_entries, 1);
    entries->cashshifts_table = cashshifts_table;
    entries->date_from = date_from.entry;
    entries->date_to = date_to.entry;
    entries->gdata = gdata;

    g_signal_connect(button, "clicked", G_CALLBACK(refresh_cashshifts_table), entries);

    return box;
}
