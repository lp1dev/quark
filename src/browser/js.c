#include "js.h"

static void error_handler(void *udata, const char *msg) {
    (void) udata;  /* ignored in this case, silence warning */

    /* Note that 'msg' may be NULL. */
    fprintf(stderr, "*** FATAL ERROR: %s\n", (msg ? msg : "no message"));
    fflush(stderr);
    abort();
}

duk_context *js_init() {
    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, error_handler);
    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "window"); // Pushing context to the "window" global
    duk_console_init(ctx, 0 /*flags*/);
    return ctx;
}


void eval(duk_context *ctx, char *str) {
    // duk_push_c_function(ctx, get_element_by_id, 1 /*nargs*/);
    // duk_put_global_string(ctx, "get_element_by_id");
    duk_eval_string(ctx, str);
    // exit(0);
}

