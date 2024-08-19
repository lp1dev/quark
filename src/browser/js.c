#include "js.h"

static void error_handler(void *udata, const char *msg) {
    (void) udata;  /* ignored in this case, silence warning */

    /* Note that 'msg' may be NULL. */
    fprintf(stderr, "*** FATAL ERROR: %s\n", (msg ? msg : "no message"));
    fflush(stderr);
    abort();
}

int js_init() {

    return 0;
}


duk_ret_t get_element_by_id(duk_context *ctx) {
    const char *id = duk_to_string(ctx, 0);
    //TODO implement
    return 0;
}

duk_ret_t eval(char *str) {
    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, error_handler);
    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "window"); // Pushing context to the "window" global
    duk_push_c_function(ctx, get_element_by_id, 1 /*nargs*/);
    duk_put_global_string(ctx, "get_element_by_id");
    duk_console_init(ctx, 0 /*flags*/);
    duk_eval_string_noresult(ctx, str);
    // exit(0);
}

