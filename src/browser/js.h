#ifndef JS_H_
#define JS_H_

#include "../../duktape/src/duktape.h"
#include "../../duktape/extras/console/duk_console.h"

void eval(duk_context *ctx, char *str);
duk_context *js_init();
duk_ret_t get_element_by_id(duk_context *ctx);

#endif