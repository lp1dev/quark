#ifndef JS_H_
#define JS_H_

#include "../../duktape/src/duktape.h"
#include "../../duktape/extras/console/duk_console.h"

int js_init();
duk_ret_t eval(char *str);
duk_ret_t get_element_by_id(duk_context *ctx);

#endif