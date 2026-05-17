#if !defined(COMMON_H)
#define COMMON_H

#include <curl/curl.h>
#include <gtk/gtk.h>

#include "../api/api.h"

typedef enum
{
    STR_EMPTY,
    STR_URL,
    STR_USERNAME,
    STR_PASSWORD,
    STR_LOGIN,
    STR_FILE,
    STR_QUIT,
    STR_CASHSHIFTS,
    STR_DATE_FROM,
    STR_DATE_TO,
    STR_CLOSE,
    STR_REFRESH,
    STR_OPENDATE,
    STR_CLOSEDATE,
    STR_ACCEPTDATE,
    STR_SESSIONSTATUS,
    STR_SALESSUM,
    STR_SALESCASH,
    STR_SALESCARD,
    STR_SALESCREDIT,
    STR_SESSIONNUMBER,
    STR__COUNT,
} str_id;

typedef enum
{
    LANG_EN,
    LANG_RU,
    LANG__COUNT,
} lang_id;

extern const char *translations[LANG__COUNT][STR__COUNT];

#define SPACING 8

typedef struct
{
    CURL *curl;
    const char *address;
    const char *token;
    lang_id current_lang;
} global_data;

const char *translate(lang_id lang, str_id str);

#define _(current_lang, string) translate(current_lang, string)

typedef struct
{
    GtkWidget *entry;
    GtkWidget *popup;
}
calendar_popup;

typedef struct
{
    GtkWidget *text;
    GtkWidget *entry;
    GtkWidget *button;
} datepicker_row;

datepicker_row create_datepicker(const char *label);

const char *reformat_date(const char *str, lang_id current_lang);

void init_css(GdkDisplay *display);

#endif
