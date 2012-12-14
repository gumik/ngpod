#include "ngpod-downloader.h"

#include <cstdio>

#include "utils.h"
#include "Logger.h"

using namespace std;

namespace ngpod
{

string Downloader::DEFAULT_URL = "http://photography.nationalgeographic.com/photography/photo-of-the-day";

Downloader::~Downloader()
{
    g_object_unref (session);

    if (image_response_message != NULL)
        g_object_unref (image_response_message);
}

Downloader::Downloader()
    : log("Downloader")
{
    session = soup_session_async_new ();
    data = NULL;
    data_length = 0;
    status = FAILED;
    image_response_message = NULL;
}

void Downloader::Start(const string& url)
{
    SoupMessage *msg = soup_message_new("GET", url.c_str());

    log << "start: " << url;

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

    SetTitle (body->data, body->length);
    SetDescription (body->data, body->length);

    DownloadImage();
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
    if (!title.empty()) log << "title: " << title;
}

void Downloader::SetDescription (const char *data, int length)
{
    description = GetXPathValue(data, length, "//*[@id=\"caption\"]/p[4]");
    if (!description.empty()) log << "description: " << description;
}

string Downloader::GetXPathValue (const char *data, guint length, const gchar *xpath)
{
    htmlDocPtr doc = htmlReadMemory (data, length, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    xmlXPathContextPtr context = xmlXPathNewContext (doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar *) xpath, context);
    string xpath_value;

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

        xpath_value.assign(reinterpret_cast<char*>(nodeBuffer->content));
        xmlBufferFree (nodeBuffer);
    }

    xmlXPathFreeObject (result);
    xmlXPathFreeContext (context);
    xmlFreeDoc (doc);

    return xpath_value;
}

string Downloader::GetXpathAttributeValue (const char *data, guint length, const gchar *xpath, const gchar *attribute)
{
    htmlDocPtr doc = htmlReadMemory (data, length, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    xmlXPathContextPtr context = xmlXPathNewContext (doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar *) xpath, context);
    string xpath_value;

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
    log << "download_image: " << link;
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
    log << "download_finished: " << status;
    signal_DownloadFinished();
}

}