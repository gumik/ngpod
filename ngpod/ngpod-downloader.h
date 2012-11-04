#ifndef __NGPOD_DOWNLOADER_H__
#define __NGPOD_DOWNLOADER_H__

#include <glibmm/date.h>
#include <glibmm/ustring.h>
#include <glibmm/signalproxy.h>

#include <libsoup/soup.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

namespace ngpod
{

class Downloader
{
public:
    enum Status
    {
        SUCCESS,
        SUCCESS_NO_IMAGE,
        FAILED,
        FAILED_GET_IMAGE
    };

    Downloader();
    virtual ~Downloader();

    void Start(const Glib::ustring& url);
    const Glib::Date& GetDate() const { return date; }
    const Glib::ustring& GetLink() const { return link; }
    const char* GetData() const { return data; }
    int GetDataLength() const { return data_length; }
    const Glib::ustring& GetTitle() const { return title; }
    const Glib::ustring& GetDescription() const { return description; }
    Status GetStatus() const { return status; }
    bool IsSuccess() const;

    Glib::SignalProxy0<void> signal_DownloadFinished();

    static Glib::ustring DEFAULT_URL;

private:
    void DownloadImage();
    void EmitDownloadFinished();
    void ImageDownloadCallback(SoupSession *session, SoupMessage *msg);
    bool SetLink(const char* data, int length);
    void SetTitle(const char* data, int length);
    void SetDate(const char* data, int length);
    void SetDescription(const char* data, int length);
    void SiteDownloadCallback(SoupSession* session, SoupMessage* msg);
    static Glib::ustring GetXPathValue(const char *data, guint length, const gchar *xpath);
    static Glib::ustring GetXpathAttributeValue(const char *data, guint length, const gchar *xpath, const gchar *attribute);
    static void SiteDownloadCallbackStatic(SoupSession* session, SoupMessage* msg, gpointer user_data);
    static void ImageDownloadCallbackStatic(SoupSession *session, SoupMessage *msg, gpointer user_data);

    SoupSession *session;
    Glib::Date date;
    Glib::ustring link;
    Glib::ustring title;
    Glib::ustring description;
    SoupMessage *image_response_message;
    const char *data;
    int data_length;
    Status status;
};

}

#endif /* __NGPOD_DOWNLOADER_H__ */
