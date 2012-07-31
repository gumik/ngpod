#include "ngpod-downloader.h"
#include "utils.h"
#include <libsoup/soup.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <stdio.h>
#include <stdlib.h>

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_DOWNLOADER, NgpodDownloaderPrivate))

struct _NgpodDownloaderPrivate
{
    SoupSession *session;
    GDate *date;
    gchar *link;
    gchar *resolution;
    gchar *title;
    gchar *description;
    SoupMessage *image_response_message;
    const char *data;
    gsize data_length;
    NgpodDownloaderStatus status;
};

enum
{
    DOWNLOAD_FINISHED,
    SIGNALS_NUM
};

static guint signals[SIGNALS_NUM];

G_DEFINE_TYPE (NgpodDownloader, ngpod_downloader, G_TYPE_OBJECT);

static void
ngpod_downloader_dispose (GObject *gobject)
{
    NgpodDownloader *self = NGPOD_DOWNLOADER (gobject);

    /*
    * In dispose, you are supposed to free all types referenced from this
    * object which might themselves hold a reference to self. Generally,
    * the most simple solution is to unref all members on which you own a
    * reference.
    */

    /* dispose might be called multiple times, so we must guard against
    * calling g_object_unref() on an invalid GObject.
    */
    /*if (self->priv->an_object)
    {
        g_object_unref (self->priv->an_object);
        self->priv->an_object = NULL;
    }*/

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_downloader_parent_class)->dispose (gobject);
}

static void
ngpod_downloader_finalize (GObject *gobject)
{
    NgpodDownloader *self = NGPOD_DOWNLOADER (gobject);

    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    g_object_unref (priv->session);
    g_clear_object (&priv->image_response_message);
    g_free (priv->title);
    g_free (priv->description);
    g_free (priv->link);
    g_free (priv->resolution);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (ngpod_downloader_parent_class)->finalize (gobject);
}

static void
ngpod_downloader_class_init (NgpodDownloaderClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = ngpod_downloader_dispose;
    gobject_class->finalize = ngpod_downloader_finalize;

    g_type_class_add_private (klass, sizeof (NgpodDownloaderPrivate));

    /* signals */
    signals[DOWNLOAD_FINISHED] = g_signal_newv ("download-finished",
                 G_TYPE_OBJECT,
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* closure */,
                 NULL /* accumulator */,
                 NULL /* accumulator data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);
}

static void
ngpod_downloader_init (NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv;
    self->priv = priv = GET_PRIVATE (self);

    priv->session = soup_session_async_new ();
    priv->date = NULL;
    priv->link = NULL;
    priv->resolution = NULL;
    priv->title = NULL;
    priv->description = NULL;
    priv->data = NULL;
    priv->data_length = 0;
    priv->status = NGPOD_DOWNLOADER_STATUS_FAILED;
    priv->image_response_message = NULL;
}

NgpodDownloader *
ngpod_downloader_new (void)
{
    GObject *object = g_object_new (NGPOD_TYPE_DOWNLOADER, NULL);
    return NGPOD_DOWNLOADER (object);
}

/*
 * constant fields
 */
const gchar *NGPOD_DOWNLOADER_DEFAULT_URL = "http://photography.nationalgeographic.com/photography/photo-of-the-day";

/*
 * public functions implementation
 */

static void site_download_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);
static void set_date (NgpodDownloader *self, const char *data);
static gboolean set_link_and_resolution (NgpodDownloader *self, const char *data);
static void set_title (NgpodDownloader *self, const char *data, guint length);
static void set_description (NgpodDownloader *self, const char *data, guint length);
static gchar *get_xpath_value (const char *data, guint length, const gchar *xpath);
static gint regex_substr (const gchar *text, gchar *regex_text, gchar ***result);
static void regex_substr_free (gchar ***result, gint count);
static GDate* date_from_strings (gchar ***strs);
static void emit_download_finished (NgpodDownloader *self);
static void download_image (NgpodDownloader *self);
static void image_download_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

void
ngpod_downloader_start (NgpodDownloader *self, const gchar *url)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    SoupSession *soup_session = priv->session;
    SoupMessage *msg = soup_message_new("GET", url);

    log_message ("Downloader", "start: %s", url);

    soup_session_queue_message (soup_session, msg, site_download_callback, self);
}

const GDate*
ngpod_downloader_get_date (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->date;
}

const gchar*
ngpod_downloader_get_link (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->link;
}

const gchar*
ngpod_downloader_get_resolution (NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->resolution;
}

const char*
ngpod_downloader_get_data (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->data;
}

gsize
ngpod_downloader_get_data_length (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->data_length;
}

const gchar *
ngpod_downloader_get_title (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->title;
}

const gchar *ngpod_downloader_get_description (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->description;
}

gboolean
ngpod_downloader_is_success (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->status == NGPOD_DOWNLOADER_STATUS_SUCCESS ||
        priv->status == NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE;
}

NgpodDownloaderStatus
ngpod_downloader_get_status (const NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    return priv->status;
}

/*
 * private functions
 */
static void
site_download_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
    NgpodDownloaderPrivate *priv;
    NgpodDownloader *self = NGPOD_DOWNLOADER (user_data);
    priv = GET_PRIVATE (self);

    SoupMessageBody *body;
    g_object_get (msg, "response-body", &body, NULL);

    if (body->data == NULL)
    {
        priv->status = NGPOD_DOWNLOADER_STATUS_FAILED;
        emit_download_finished (self);
        return;
    }

    set_date (self, body->data);

    if (!set_link_and_resolution (self, body->data))
    {
        priv->status = NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE;
        emit_download_finished (self);
        return;
    }

    set_title (self, body->data, body->length);
    set_description (self, body->data, body->length);

    download_image (self);
}

static void
set_date (NgpodDownloader *self, const char *data)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    gchar **substrs;
    guint count = regex_substr (data, "<p class=\"publication_time\">([^<]{3})[^<]* ([^<]*), ([^<]*)</p>", &substrs);

    g_free (priv->date);
    priv->date = NULL;

    if (count == 3)
    {
        priv->date = date_from_strings (&substrs);
    }

    regex_substr_free (&substrs, count);
}

static gboolean
set_link_and_resolution (NgpodDownloader *self, const char *data)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);
    gchar **substrs;
    guint count = regex_substr (data, "<div class=\"download_link\">[^<]*<a href=\"([^>]*)\">Download Wallpaper \\((.*) pixels\\)</a>[^<]*</div>", &substrs);
    gboolean ret = FALSE;

    g_free (priv->resolution);
    priv->resolution = NULL;

    g_free (priv->link);
    priv->link = NULL;

    if (count >= 2)
    {
        priv->resolution = substrs[1];
        priv->link = substrs[0];
        ret = TRUE;
        g_free (substrs);
    }
    else if (count > 0)
    {
        regex_substr_free (&substrs, count);
    }

    return ret;
}

static void
set_title (NgpodDownloader *self, const char *data, guint length)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    g_free (priv->title);
    priv->title = NULL;

    priv->title = get_xpath_value(data, length, "//*[@id=\"caption\"]/h2");
    if (priv->title != NULL) log_message ("Downloader", "title: %s", priv->title);
}

static void
set_description (NgpodDownloader *self, const char *data, guint length)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    g_free (priv->description);
    priv->description = NULL;

    priv->description = get_xpath_value(data, length, "//*[@id=\"caption\"]/p[4]");
    if (priv->description != NULL) log_message ("Downloader", "desctiption: %s", priv->description);
}

static gchar *
get_xpath_value (const char *data, guint length, const gchar *xpath)
{
    htmlDocPtr doc = htmlReadMemory (data, length, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    xmlXPathContextPtr context = xmlXPathNewContext (doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar *) xpath, context);
    gchar *xpath_value = NULL;

    if (!xmlXPathNodeSetIsEmpty (result->nodesetval))
    {
        xmlBufferPtr nodeBuffer = xmlBufferCreate();
        xmlNodePtr node = result->nodesetval->nodeTab[0]->children;

        while (node != NULL)
        {
            xmlBufferPtr tempBuffer = xmlBufferCreate();
            htmlNodeDump (tempBuffer, doc, node);
            xmlBufferAdd (nodeBuffer, tempBuffer->content, tempBuffer->use);
            xmlBufferFree (tempBuffer);
            node = node->next;
        }

        xpath_value = g_strndup ((gchar *) nodeBuffer->content, nodeBuffer->use);
        xmlBufferFree (nodeBuffer);
    }

    xmlXPathFreeObject (result);
    xmlXPathFreeContext (context);
    xmlFreeDoc (doc);

    return xpath_value;
}

static void
download_image (NgpodDownloader *self)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    SoupSession *soup_session = priv->session;
    SoupMessage *msg = soup_message_new("GET", priv->link);

    log_message ("Downloader", "download_image: %s", priv->link);

    soup_session_queue_message (soup_session, msg, image_download_callback, self);
}

static void
image_download_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
    NgpodDownloader *self = NGPOD_DOWNLOADER (user_data);
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    priv->image_response_message = msg;
    g_object_ref (msg);

    guint status_code;
    g_object_get (msg, "status-code", &status_code, NULL);

    if (status_code != 200)
    {
        priv->status = NGPOD_DOWNLOADER_STATUS_FAILED_GET_IMAGE;
        emit_download_finished (self);
        return;
    }

    SoupMessageBody *body;
    g_object_get (msg, "response-body", &body, NULL);
    priv->data = body->data;
    priv->data_length = body->length;
    priv->status = NGPOD_DOWNLOADER_STATUS_SUCCESS;
    emit_download_finished (self);
}

static void
emit_download_finished (NgpodDownloader *self)
{
    log_message ("Downloader", "download_finished: %d", self->priv->status);
    g_signal_emit (self, signals[DOWNLOAD_FINISHED], 0);
}

static gint
regex_substr (const gchar *text, gchar *regex_text, gchar ***result)
{
    GError *error = NULL;
    GRegex *regex = g_regex_new (regex_text, 0, 0, &error);
    g_return_val_if_fail (!error, 0);

    GMatchInfo *match_info;
    if (!g_regex_match (regex, text, 0, &match_info))
    {
        return 0;
    }

    int match_count = g_match_info_get_match_count (match_info);
    if (match_count < 2)
    {
        return 0;
    }

    *result = g_new (gchar*, match_count - 1);

    int i;
    for (i = 0; i < match_count - 1; ++i)
    {
        gint start_pos;
        gint end_pos;
        g_match_info_fetch_pos (match_info, i + 1, &start_pos, &end_pos);
        (*result)[i] = g_strndup (text + start_pos, end_pos - start_pos);
    }

    g_regex_unref (regex);
    g_match_info_free (match_info);

    return match_count - 1;
}

static void
regex_substr_free (gchar ***result, gint count)
{
    int i;
    for (i = 0; i < count; ++i)
    {
        g_free ((*result)[i]);
    }

    if (count)
    {
        g_free (*result);
    }
}

static const gchar *months[] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

static GDate*
date_from_strings (gchar ***strs)
{
    GDate *date = g_date_new ();

    gint i;
    gboolean found_month = FALSE;
    for (i = 0; i < 12; ++i)
    {
        if (g_strcmp0 (months[i], (*strs)[0]) == 0)
        {
            g_date_set_month (date, i + 1);
            found_month = TRUE;
            break;
        }
    }

    if (!found_month) return NULL;

    g_date_set_day (date, atoi ((*strs)[1]));
    g_date_set_year (date, atoi ((*strs)[2]));

    return date;
}