NGPOD
=====

Downloads National Geographic Photo of the Day and eventually
sets it as desktop background. Actually it supports only
[MATE](http://mate-desktop.org) desktop.

Dependencies
-------------
* glib >= 2.32
* gtk+ 2
* libsoup 2
* mateconf 2

Installation
------------
    cd ngpod/ngpod
    cmake .
    make

Configuration
-------------
All options are set in `~/.config/gumik/ngpod-gtk.ini` file.
This is standard .ini like file. It should contain one section `[main]`.
Possible options are:

* dir - path to directory where to put downloaded pictures; *required*
* time_span - time from midnight when new picture is expected; *optional*
* log_file - path to log file; *optional*
* last_date - date when the last picture was downloaded;
    it is set by the program every time new picture appears;
    normally you don't want to use it

### Example ###
    [main]
    dir=/home/gumik/temp/ngpod
    log_file=/home/gumik/temp/ngpod/ngpod.log
    last_date=2012;7;29;
    time_span=9;01

Usage
-----
When you run `ngpod` executable it will print some unuseful log messages
to the standard output. If new picture appears, a nice yellow rectangle icon is
shown on the system tray. Clicking on it shows dialog window with picture which
you may accept or reject.

Known bugs
----------
* GUI is in Polish language only