#include "common.h"

const char *translations[LANG__COUNT][STR__COUNT] =
{
    [LANG_EN] =
    {
        [STR_EMPTY] = "Empty",
        [STR_URL] = "Server address",
        [STR_USERNAME] = "Username",
        [STR_PASSWORD] = "Password",
        [STR_LOGIN] = "Login",
        [STR_FILE] = "File",
        [STR_QUIT] = "Quit",
        [STR_CASHSHIFTS] = "Cash Shifts",
        [STR_DATE_FROM] = "Date from",
        [STR_DATE_TO] = "Date to",
        [STR_CLOSE] = "Close",
        [STR_REFRESH] = "Refresh",
        [STR_OPENDATE] = "Open Date",
        [STR_CLOSEDATE] = "Close Date",
        [STR_ACCEPTDATE] = "Accept Date",
        [STR_SESSIONSTATUS] = "Session Status",
        [STR_SALESSUM] = "Sales Sum",
        [STR_SALESCASH] = "Sales Cash",
        [STR_SALESCARD] = "Sales Card",
        [STR_SALESCREDIT] = "Sales Credit",
        [STR_SESSIONNUMBER] = "Session Number",
    },

    [LANG_RU] =
    {
        [STR_EMPTY] = "Пусто",
        [STR_URL] = "Адрес сервера",
        [STR_USERNAME] = "Имя пользователя",
        [STR_PASSWORD] = "Пароль",
        [STR_LOGIN] = "Войти",
        [STR_FILE] = "Файл",
        [STR_QUIT] = "Выход",
        [STR_CASHSHIFTS] = "Кассовые смены",
        [STR_DATE_FROM] = "Дата с",
        [STR_DATE_TO] = "Дата по",
        [STR_CLOSE] = "Закрыть",
        [STR_REFRESH] = "Обновить",
        [STR_OPENDATE] = "Дата Открытия",
        [STR_CLOSEDATE] = "Дата Закрытия",
        [STR_ACCEPTDATE] = "Дата принятия",
        [STR_SESSIONSTATUS] = "Статус Смены",
        [STR_SALESSUM] = "Сумма Продаж",
        [STR_SALESCASH] = "Оплачено Наличкой",
        [STR_SALESCARD] = "Оплачено Картой",
        [STR_SALESCREDIT] = "Оплачено В Кредит",
        [STR_SESSIONNUMBER] = "Номер Смены",
    }
};

static void set_date(GtkCalendar *calendar, gpointer udata)
{
    calendar_popup *cp = udata;

    g_autoptr(GDateTime) dt = gtk_calendar_get_date(calendar);
    if (!dt) return;

    char *text = g_date_time_format(dt, "%F");

    gtk_editable_set_text(GTK_EDITABLE(cp->entry), text);
    g_free(text);

    return;
}

static void close_calendar(GtkButton *button, gpointer udata)
{
    GtkWidget *popup = udata;
    gtk_popover_popdown(GTK_POPOVER(popup));

    return;
}

datepicker_row create_datepicker(const char *label)
{
    GtkWidget *text = gtk_label_new(label);

    calendar_popup *cp = g_new(calendar_popup, 1);
    cp->entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(cp->entry), "YYYY-MM-DD");

    GtkWidget *button = gtk_menu_button_new();
    gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(button), "x-office-calendar");

    cp->popup = gtk_popover_new();

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACING);

    GtkWidget *calendar = gtk_calendar_new();
    GtkWidget *close_btn = gtk_button_new_with_label("Close");


    gtk_box_append(GTK_BOX(box), calendar);
    gtk_box_append(GTK_BOX(box), close_btn);

    gtk_popover_set_child(GTK_POPOVER(cp->popup), box);

    gtk_menu_button_set_popover(GTK_MENU_BUTTON(button), cp->popup);

    g_signal_connect(calendar, "day-selected", G_CALLBACK(set_date), cp);
    g_signal_connect(calendar, "next-year", G_CALLBACK(set_date), cp);
    g_signal_connect(calendar, "next-month", G_CALLBACK(set_date), cp);
    g_signal_connect(calendar, "prev-year", G_CALLBACK(set_date), cp);
    g_signal_connect(calendar, "prev-month", G_CALLBACK(set_date), cp);

    g_signal_connect(close_btn, "clicked", G_CALLBACK(close_calendar), cp->popup);

    datepicker_row row = {.text = text, .entry = cp->entry, .button = button};

    return row;
}

///

const char *reformat_date(const char *str, lang_id current_lang)
{
    if (str)
    {
        g_autoptr(GTimeZone) timezone = g_time_zone_new_local();
        g_autoptr(GDateTime) time = g_date_time_new_from_iso8601(str, timezone);
        const char *result = g_date_time_format(time, "%d.%m.%Y %H:%M");
        return result;
    }

    else
    {
        return _(current_lang, STR_EMPTY);
    }
}

///

void init_css(GdkDisplay *display)
{
    static const char css[] =
    ".table-cell {"
    "    border: 1px solid @theme_fg_color;"
    "    padding: 8px;"
    "}";

    g_autoptr(GtkCssProvider) provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);

    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
