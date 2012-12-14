#ifndef __NGPOD_APPLICATION_H__
#define __NGPOD_APPLICATION_H__

#include "Logger.h"
#include "ngpod-timer.h"
#include "ngpod-watcher.h"
#include "XfceWallpaper.h"
#include "ngpod-settings.h"

namespace ngpod
{

class Application
{
public:
    Application(int& argc, char**& argv);
    int Run();

protected:
    virtual void InitGui() = 0;
    bool Init();
    virtual int StartGui() = 0;
    virtual IPresenter* CreatePresenter() = 0;

    Logger logger;
    int& argc;
    char**& argv;

private:
    std::auto_ptr<Downloader> downloader;
    std::auto_ptr<Watcher> watcher;
    std::auto_ptr<IPresenter> presenter;
    std::auto_ptr<XfceWallpaper> wallpaper;
    std::auto_ptr<Timer> timer;
    std::auto_ptr<Settings> settings;
};

}

#endif /* __NGPOD_APPLICATION_H__ */