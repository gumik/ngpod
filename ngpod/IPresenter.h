#ifndef __NGPOD_IPRESENTER_H__
#define __NGPOD_IPRESENTER_H__

#include <sigc++/sigc++.h>
#include <string>

namespace ngpod
{

class IPresenter
{
public:
    virtual ~IPresenter() { }

    virtual void Notify(const char* data, int data_length, const std::string& title,
                const std::string& description) = 0;
    virtual void Hide() = 0;
    virtual void ShowError(const std::string& msg) = 0;
    virtual bool IsAccepted() const = 0;

    sigc::signal<void> signal_MadeChoice;
};

}

#endif /* __NGPOD_IPRESENTER_H__ */