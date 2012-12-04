#include "GtkApplication.h"

int main (int argc, char **argv)
{
    ngpod::GtkApplication app(argc, argv);
    return app.Run();
}