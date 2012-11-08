#include <gio/gio.h>
#include <mateconf/mateconf.h>
#include "ngpod-wallpaper.h"

using namespace Glib;

namespace ngpod
{

Wallpaper::Wallpaper(const ustring& dir)
    : dir(dir)
{
}

bool Wallpaper::SetFromData(const char *data, gsize data_length, GError **error)
{
    // Prepare path
    DateTime now = DateTime::create_now_local();
    ustring path = GetPathFromDate (now, dir);

    // Write data to file
    GFile *file = g_file_new_for_path (path.c_str());

    GFileOutputStream *stream = g_file_create (file, G_FILE_CREATE_NONE, NULL, error);

    if (stream == NULL)
    {
        g_object_unref (file);
        return false;
    }

    gsize bytes_written = 0;
    if (!g_output_stream_write_all (G_OUTPUT_STREAM (stream), data, data_length, &bytes_written, NULL, error))
    {
        g_object_unref (stream);
        return false;
    }

    if (!g_output_stream_close (G_OUTPUT_STREAM (stream), NULL, error))
    {
        g_object_unref (stream);
        return false;
    }

    g_object_unref (stream);

    // Change desktop background
    MateConfEngine *conf = mateconf_engine_get_default ();
    gchar *value = mateconf_engine_get_string (conf, "/desktop/mate/background/picture_filename", NULL);
    if (!mateconf_engine_set_string (conf, "/desktop/mate/background/picture_filename", path.c_str(), error))
    {
        mateconf_engine_unref (conf);
        return false;
    }

    mateconf_engine_unref (conf);

    return true;
}

/* private methods */
ustring Wallpaper::GetPathFromDate(const DateTime& now, const ustring& dir)
{
    ustring filename = now.format("pod-%F.png");
    return build_filename(dir, filename);
}

}