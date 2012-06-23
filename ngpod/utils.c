#include "utils.h"

void
unref_if_not_null (GObject *object)
{
    if (object)
    {
        g_object_unref (object);
    }
}