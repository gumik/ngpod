#ifndef __NGPOD_DOWNLOADER_H__
#define __NGPOD_DOWNLOADER_H__

#include <boost/date_time/date.hpp>
#include <libsoup/soup.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <sigc++/sigc++.h>
#include <string>

#include "Logger.h"

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

    void Start(const std::string& url);
    const std::string& GetLink() const { return link; }
    const char* GetData() const { return data; }
    int GetDataLength() const { return data_length; }
    const std::string& GetTitle() const { return title; }
    const std::string& GetDescription() const { return description; }
    Status GetStatus() const { return status; }
    bool IsSuccess() const;

    sigc::signal<void> signal_DownloadFinished;

    static std::string DEFAULT_URL;

private:
    void DownloadImage();
    void EmitDownloadFinished();
    void ImageDownloadCallback(SoupSession *session, SoupMessage *msg);
    bool SetLink(const char* data, int length);
    void SetTitle(const char* data, int length);
    void SetDescription(const char* data, int length);
    void SiteDownloadCallback(SoupSession* session, SoupMessage* msg);
    static std::string GetXPathValue(const char *data, guint length, const gchar *xpath);
    static std::string GetXpathAttributeValue(const char *data, guint length, const gchar *xpath, const gchar *attribute);
    static void SiteDownloadCallbackStatic(SoupSession* session, SoupMessage* msg, gpointer user_data);
    static void ImageDownloadCallbackStatic(SoupSession *session, SoupMessage *msg, gpointer user_data);

    SoupSession *session;
    std::string link;
    std::string title;
    std::string description;
    SoupMessage *image_response_message;
    const char *data;
    int data_length;
    Status status;
    Logger log;
};

}

#endif /* __NGPOD_DOWNLOADER_H__ */
