#ifndef JS_H_
#define JS_H_

#include "../../duktape/src/duktape.h"

duk_ret_t hellolog(duk_context *ctx);
duk_ret_t eval(char *str);

#endif