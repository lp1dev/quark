#include "js.h"

duk_ret_t eval(char *str) {
    duk_context *ctx = duk_create_heap_default();
    duk_push_c_function(ctx, hellolog, 1 /*nargs*/);
    duk_put_global_string(ctx, "log");
    duk_eval_string_noresult(ctx, str);
    // exit(0);
}

duk_ret_t hellolog(duk_context *ctx) {
    const char *output = duk_to_string(ctx, 0);
    printf("%s\n", output);
    return 0;
    // return strlen(output);
}

