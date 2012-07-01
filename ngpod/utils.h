#ifndef __NGPOD_UTILS_H
#define __NGPOD_UTILS_H

#include <glib-object.h>
#include <glib.h>

guint create_void_void_signal (const gchar *signal_name);
void log_message (const gchar *class_name, const gchar *format, ...);

#endif
