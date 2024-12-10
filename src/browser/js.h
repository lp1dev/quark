#ifndef JS_H_
#define JS_H_

#include "../../duktape/src/duktape.h"
#include "../../duktape/extras/console/duk_console.h"

duk_context *js_init();
static duk_int_t eval_js_file(duk_context *ctx, const char *filename);
void trigger_js_event_int(duk_context *ctx, char *type, int value);
void init_js_globals(duk_context *ctx);
#endif