#include "ngpod-downloader.h"
#include "utils.h"
#include <stdio.h>
#include <glib.h>

using namespace Glib;

namespace ngpod
{

ustring Downloader::DEFAULT_URL = "http://photography.nationalgeographic.com/photography/photo-of-the-day";

Downloader::~Downloader()
{
    g_object_unref (session);

    if (image_response_message != NULL)
        g_object_unref (image_response_message);
}

Downloader::Downloader()
{
    session = soup_session_async_new ();
    data = NULL;
    data_length = 0;
    status = FAILED;
    image_response_message = NULL;
}

void Downloader::Start(const ustring& url)
{
    SoupMessage *msg = soup_message_new("GET", url.c_str());

    log_message ("Downloader", "start: %s", url.c_str());

    soup_session_queue_message (session, msg, Downloader::SiteDownloadCallbackStatic, this);
}

bool Downloader::IsSuccess() const
{
    return status == SUCCESS || status == SUCCESS_NO_IMAGE;
}

void Downloader::SiteDownloadCallbackStatic(SoupSession *session, SoupMessage *msg, gpointer user_data)
{
    static_cast<Downloader*>(user_data)->SiteDownloadCallback(session, msg);
}

void Downloader::SiteDownloadCallback(SoupSession *session, SoupMessage* msg)
{
    SoupMessageBody *body;
    g_object_get (msg, "response-body", &body, NULL);

    if (body->data == NULL)
    {
        status = FAILED;
        EmitDownloadFinished();
        return;
    }

    if (!SetLink (body->data, body->length))
    {
        status = SUCCESS_NO_IMAGE;
        EmitDownloadFinished();
        return;
    }

    SetDate (body->data, body->length);
    SetTitle (body->data, body->length);
    SetDescription (body->data, body->length);

    DownloadImage();
}

void Downloader::SetDate (const char *data, int length)
{
    date.clear();

    ustring date_str = GetXPathValue (data, length, "//*[@id=\"caption\"]/p[1]");
    date = date_from_string (date_str);

    if (date.valid())
    {
        log_message ("Downloader", "Date: %s", date_to_string (date).c_str());
    }
}

bool Downloader::SetLink(const char *data, int length)
{
    static const gchar *xpath = "//*[@id=\"content_mainA\"]/div[1]/div/div[1]/div[2]/a";
    link = GetXpathAttributeValue (data, length, xpath, "href");
    return !link.empty();
}

void Downloader::SetTitle (const char *data, int length)
{
    title = GetXPathValue(data, length, "//*[@id=\"caption\"]/h2");
    if (!title.empty()) log_message ("Downloader", "title: %s", title.c_str());
}

void Downloader::SetDescription (const char *data, int length)
{
    description = GetXPathValue(data, length, "//*[@id=\"caption\"]/p[4]");
    if (!description.empty()) log_message ("Downloader", "description: %s", description.c_str());
}

ustring Downloader::GetXPathValue (const char *data, guint length, const gchar *xpath)
{
    htmlDocPtr doc = htmlReadMemory (data, length, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    xmlXPathContextPtr context = xmlXPathNewContext (doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar *) xpath, context);
    ustring xpath_value;

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

        xpath_value.assign(reinterpret_cast<const char*>(nodeBuffer->content), nodeBuffer->use);
        xmlBufferFree (nodeBuffer);
    }

    xmlXPathFreeObject (result);
    xmlXPathFreeContext (context);
    xmlFreeDoc (doc);

    return xpath_value;
}

ustring Downloader::GetXpathAttributeValue (const char *data, guint length, const gchar *xpath, const gchar *attribute)
{
    htmlDocPtr doc = htmlReadMemory (data, length, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    xmlXPathContextPtr context = xmlXPathNewContext (doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar *) xpath, context);
    ustring xpath_value;

    if (!xmlXPathNodeSetIsEmpty (result->nodesetval))
    {
        xmlChar *attr_value = xmlGetProp (result->nodesetval->nodeTab[0], (xmlChar *) attribute);

        if (attr_value != NULL)
        {
            xpath_value.assign(reinterpret_cast<const char*>(attr_value));
            xmlFree (attr_value);
        }
    }

    xmlXPathFreeObject (result);
    xmlXPathFreeContext (context);
    xmlFreeDoc (doc);

    return xpath_value;
}

void Downloader::DownloadImage()
{
    SoupMessage *msg = soup_message_new("GET", link.c_str());
    log_message ("Downloader", "download_image: %s", link.c_str());
    soup_session_queue_message (session, msg, Downloader::ImageDownloadCallbackStatic, this);
}

void Downloader::ImageDownloadCallbackStatic (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
    static_cast<Downloader*>(user_data)->ImageDownloadCallback(session, msg);
}

void Downloader::ImageDownloadCallback (SoupSession *session, SoupMessage *msg)
{
    image_response_message = msg;
    g_object_ref (msg);

    guint status_code;
    g_object_get (msg, "status-code", &status_code, NULL);

    if (status_code != 200)
    {
        status = FAILED_GET_IMAGE;
        signal_DownloadFinished();
        return;
    }

    SoupMessageBody *body;
    g_object_get (msg, "response-body", &body, NULL);
    data = body->data;
    data_length = body->length;
    status = SUCCESS;
    EmitDownloadFinished();
}

void Downloader::EmitDownloadFinished ()
{
    log_message ("Downloader", "download_finished: %d", status);
    signal_DownloadFinished();
}

}