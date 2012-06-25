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