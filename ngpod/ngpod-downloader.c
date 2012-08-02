#include "ngpod-downloader.h"
#include "utils.h"
#include <libsoup/soup.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <stdio.h>

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NGPOD_TYPE_DOWNLOADER, NgpodDownloaderPrivate))

struct _NgpodDownloaderPrivate
{
    SoupSession *session;
    GDate *date;
    gchar *link;
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
static void set_date (NgpodDownloader *self, const char *data, guint length);
static gboolean set_link (NgpodDownloader *self, const char *data, guint length);
static void set_title (NgpodDownloader *self, const char *data, guint length);
static void set_description (NgpodDownloader *self, const char *data, guint length);
static gchar *get_xpath_value (const char *data, guint length, const gchar *xpath);
static gchar *get_xpath_attribute_value (const char *data, guint length, const gchar *xpath, const gchar *attribute);
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

    if (!set_link (self, body->data, body->length))
    {
        priv->status = NGPOD_DOWNLOADER_STATUS_SUCCESS_NO_IMAGE;
        emit_download_finished (self);
        return;
    }

    set_date (self, body->data, body->length);
    set_title (self, body->data, body->length);
    set_description (self, body->data, body->length);

    download_image (self);
}

static void
set_date (NgpodDownloader *self, const char *data, guint length)
{
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    if (priv->date != NULL)
    {
        g_date_free (priv->date);
        priv->date = NULL;
    }

    gchar *date_str = get_xpath_value (data, length, "//*[@id=\"caption\"]/p[1]");
    priv->date = date_from_string (date_str);
    g_free (date_str);

    if (priv->date != NULL)
    {
        gchar *str = date_to_string (priv->date);
        log_message ("Downloader", "Date: %s", str);
        g_free (str);
    }
}

static gboolean
set_link (NgpodDownloader *self, const char *data, guint length)
{
    static const gchar *xpath = "//*[@id=\"content_mainA\"]/div[1]/div/div[1]/div[2]/a";
    NgpodDownloaderPrivate *priv = GET_PRIVATE (self);

    g_free (priv->link);
    priv->link = get_xpath_attribute_value (data, length, xpath, "href");

    return priv->link != NULL;
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
    if (priv->description != NULL) log_message ("Downloader", "description: %s", priv->description);
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

static gchar *
get_xpath_attribute_value (const char *data, guint length, const gchar *xpath, const gchar *attribute)
{
    htmlDocPtr doc = htmlReadMemory (data, length, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    xmlXPathContextPtr context = xmlXPathNewContext (doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar *) xpath, context);
    gchar *xpath_value = NULL;

    if (!xmlXPathNodeSetIsEmpty (result->nodesetval))
    {
        xmlChar *attr_value = xmlGetProp (result->nodesetval->nodeTab[0], (xmlChar *) attribute);

        if (attr_value != NULL)
        {
            xpath_value = g_strdup ((gchar *) attr_value);
            xmlFree (attr_value);
        }
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