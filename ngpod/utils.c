#include "utils.h"

guint
create_void_void_signal (const gchar *signal_name)
{
    return g_signal_newv (
        signal_name,
        G_TYPE_OBJECT,
        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        NULL /* closure */,
        NULL /* accumulator */,
        NULL /* accumulator data */,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE /* return_type */,
        0     /* n_params */,
        NULL  /* param_types */);
}

void
log_message (const gchar *class_name, const gchar *format, ...)
{
    va_list list;
    va_start (list, format);
    g_logv (class_name, G_LOG_LEVEL_MESSAGE, format, list);
    va_end (list);
}
