#include "js.h"
// #include "css.h"

static void error_handler(void *udata, const char *msg) {
    (void) udata;  /* ignored in this case, silence warning */

    /* Note that 'msg' may be NULL. */
    fprintf(stderr, "*** FATAL ERROR: %s\n", (msg ? msg : "no message"));
    fflush(stderr);
    abort();
}

duk_context *js_init() {
    duk_int_t res = 0;

    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, error_handler);
    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "window"); // Pushing context to the "window" global
    duk_console_init(ctx, 0 /*flags*/);

    if((res = eval_js_file(ctx, "quark.js")) != 0) {
      printf("ERROR EVALUATING JS FILE\n");
      exit(0);
      /* error handler */ 
    }
    return ctx;
}

// Taken from https://docs.sheetjs.com/docs/demos/engines/duktape/#load-sheetjs-scripts
static duk_int_t eval_js_file(duk_context *ctx, const char *filename) {
  size_t len;
  /* read script from filesystem */
  FILE *f = fopen(filename, "rb");
  if(!f) { duk_push_undefined(ctx); perror("fopen"); return 1; }
  long fsize; { fseek(f, 0, SEEK_END); fsize = ftell(f); fseek(f, 0, SEEK_SET); }
  char *buf = (char *)malloc(fsize * sizeof(char));
  len = fread((void *) buf, 1, fsize, f);
  fclose(f);
  if(!buf) { duk_push_undefined(ctx); perror("fread"); return 1; }

  /* load script into the context */
  duk_push_lstring(ctx, (const char *)buf, (duk_size_t)len);
  /* eval script */
  duk_int_t retval = duk_peval(ctx);
  /* cleanup */
  duk_pop(ctx);
  return retval;
}