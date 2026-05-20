#include <gtk/gtk.h>
#include "common.h"
#include "../api/api.h"

typedef struct
{
    GtkWidget *date_from;
    GtkWidget *date_to;
    GtkWidget *cashshifts_table;
    global_data *gdata;
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

static void refresh_cashshifts_table(GtkButton *button, gpointer udata)
{
    cashshifts_entries *entries = udata;

    char *date_from = g_strdup(gtk_editable_get_text(GTK_EDITABLE(entries->date_from)));
    char *date_to = g_strdup(gtk_editable_get_text(GTK_EDITABLE(entries->date_to)));

    if (date_from && date_to)
    {
        cashshifts_list prompt =
        {
            .openDateFrom = date_from,
            .openDateTo = date_to,
            .departmentId = NULL,
            .groupId = NULL,
            .revision_from = -1,
            .status = "ANY",
        };

        cashshifts_list_args args =
        {
            .token = entries->gdata->token,
            .address = entries->gdata->address,
            .prompt = prompt,
        };

        cashshifts_list_answer *result = cashshifts_list_get(args);

        if (result)
        {
            GtkWidget *child = gtk_widget_get_first_child(entries->cashshifts_table);
            while (child != NULL)
            {
                GtkWidget *next = gtk_widget_get_next_sibling(child);
                gtk_grid_remove(GTK_GRID(entries->cashshifts_table), child);
                child = next;
            }

            size_t row_pos = 0;

            row header =
            {
                .OpenDate = gtk_label_new(_(entries->gdata->current_lang, STR_OPENDATE)),
                .CloseDate = gtk_label_new(_(entries->gdata->current_lang, STR_CLOSEDATE)),
                .AcceptDate = gtk_label_new(_(entries->gdata->current_lang, STR_ACCEPTDATE)),
                .SessionStatus = gtk_label_new(_(entries->gdata->current_lang, STR_SESSIONSTATUS)),
                .SalesSum = gtk_label_new(_(entries->gdata->current_lang, STR_SALESSUM)),
                .SalesCash = gtk_label_new(_(entries->gdata->current_lang, STR_SALESCASH)),
                .SalesCard = gtk_label_new(_(entries->gdata->current_lang, STR_SALESCARD)),
                .SalesCredit = gtk_label_new(_(entries->gdata->current_lang, STR_SALESCREDIT)),
                .SessionNumber = gtk_label_new(_(entries->gdata->current_lang, STR_SESSIONNUMBER)),
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

            add_row(row_pos, header, GTK_GRID(entries->cashshifts_table));
            row_pos++;

            for (size_t i = 0; i < result->size; i++)
            {
                row element;

                cashshifts_list_answer_element e = result->elements[i];

                element.OpenDate = gtk_label_new(reformat_date(e.openDate, entries->gdata->current_lang));
                element.CloseDate = gtk_label_new(reformat_date(e.closeDate, entries->gdata->current_lang));
                element.AcceptDate = gtk_label_new(reformat_date(e.acceptDate, entries->gdata->current_lang));
                element.SessionStatus = gtk_label_new(reformat_date(e.acceptDate, entries->gdata->current_lang));
                element.SalesSum = gtk_label_new(g_strdup_printf("%i", e.salesCard + e.salesCash + e.salesCredit));
                element.SalesCash = gtk_label_new(g_strdup_printf("%i", e.salesCash));
                element.SalesCard = gtk_label_new(g_strdup_printf("%i", e.salesCard));
                element.SalesCredit = gtk_label_new(g_strdup_printf("%i", e.salesCredit));
                element.SessionNumber = gtk_label_new(g_strdup_printf("%i", e.sessionNumber));

                gtk_label_set_xalign(GTK_LABEL(element.OpenDate), 0.0);
                gtk_label_set_xalign(GTK_LABEL(element.CloseDate), 0.0);
                gtk_label_set_xalign(GTK_LABEL(element.AcceptDate), 0.0);
                gtk_label_set_xalign(GTK_LABEL(element.SessionStatus), 0.0);
                gtk_label_set_xalign(GTK_LABEL(element.SalesSum), 1.0);
                gtk_label_set_xalign(GTK_LABEL(element.SalesCash), 1.0);
                gtk_label_set_xalign(GTK_LABEL(element.SalesCard), 1.0);
                gtk_label_set_xalign(GTK_LABEL(element.SalesCredit), 1.0);
                gtk_label_set_xalign(GTK_LABEL(element.SessionNumber), 1.0);

                gtk_label_set_selectable(GTK_LABEL(element.OpenDate), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.CloseDate), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.AcceptDate), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.SessionStatus), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.SalesSum), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.SalesCash), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.SalesCard), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.SalesCredit), TRUE);
                gtk_label_set_selectable(GTK_LABEL(element.SessionNumber), TRUE);

                gtk_widget_add_css_class(element.OpenDate, "table-cell");
                gtk_widget_add_css_class(element.CloseDate, "table-cell");
                gtk_widget_add_css_class(element.AcceptDate, "table-cell");
                gtk_widget_add_css_class(element.SessionStatus, "table-cell");
                gtk_widget_add_css_class(element.SalesSum, "table-cell");
                gtk_widget_add_css_class(element.SalesCash, "table-cell");
                gtk_widget_add_css_class(element.SalesCard, "table-cell");
                gtk_widget_add_css_class(element.SalesCredit, "table-cell");
                gtk_widget_add_css_class(element.SessionNumber, "table-cell");

                add_row(row_pos, element, GTK_GRID(entries->cashshifts_table));
                row_pos++;
            }

            cashshifts_list_destroy(result);
        }
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

GtkWidget *create_shifts(global_data *gdata)
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
