#ifndef JS_H_
#define JS_H_

#include "../../duktape/src/duktape.h"
#include "../../duktape/extras/console/duk_console.h"


duk_context *js_init();
// void serialize_future_render(duk_context *ctx, void *item);

#endif