#include <SDL2/SDL.h>
#include <errno.h>
#include "js.h"
#include "../net/tcp_debugger.h"
#include "../net/net.h"
#include "../exploits/python.h"

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
      debug("ERROR EVALUATING JS FILE!", NULL);
      exit(0);
    }
    return ctx;
}

// Taken from https://docs.sheetjs.com/docs/demos/engines/duktape/#load-sheetjs-scripts
/* 
static duk_int_t eval_js_file(duk_context *ctx, const char *filename)

*/
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
  free(buf);
  return retval;
}

/* 

void trigger_js_event_int(duk_context *ctx, char *type, int value)

  Trigger a JS event using the quark_onEvent JS global function

*/
void trigger_js_event_int(duk_context *ctx, char *type, int value) {
    duk_get_global_string(ctx, "quark_onEvent");
    duk_push_string(ctx, type);
    duk_push_int(ctx, value);
    duk_call(ctx, 2);
    duk_pop(ctx);
}


/* 
static duk_ret_t quark_python_call(duk_context *ctx);

  Call a function inside a Python module.

*/
static duk_ret_t quark_python_call(duk_context *ctx) {
    const char *module;
    const char *function;

    module = duk_get_string(ctx, 0);
    function = duk_get_string(ctx, 1);
    python_call((char *)module, (char *)function);
    duk_pop_2(ctx);
    return (duk_ret_t) 1;
}

/* 
static duk_ret_t quark_python_init(duk_context *ctx)

  Initialize the Python environment

*/
static duk_ret_t quark_python_init(duk_context *ctx) {
    python_init();
    return (duk_ret_t) 0;
}


/* 
static duk_ret_t quark_exit(duk_context *ctx)

*/
static duk_ret_t quark_exit(duk_context *ctx) {
    SDL_Event event;

    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    return (duk_ret_t) 0;
}

void get_controller_buttons(SDL_GameController *controller, duk_context *ctx) {
    int max_buttons;
    int pressed;
    duk_idx_t array_index;



    array_index = duk_push_array(ctx);
    max_buttons = 22; // Only 22 buttons supported by SDL_GameControllerButton
    for (int i = 0; i < max_buttons; i++) {
        pressed = SDL_GameControllerGetButton(controller, i);
        duk_push_object(ctx);
        //
        duk_push_boolean(ctx, pressed);
        duk_put_prop_string(ctx, -2, "pressed");
        //
        duk_push_boolean(ctx, pressed);
        duk_put_prop_string(ctx, -2, "touched");
        //
        duk_push_number(ctx, (double) pressed);
        duk_put_prop_string(ctx, -2, "value");
        //
        duk_put_prop_index(ctx, array_index, i);
    }
}

/* 

static duk_ret_t quark_debug(duk_context *ctx)

  Prints (and sends to TCP debugger) a string

*/
static duk_ret_t quark_debug(duk_context *ctx) {
  char * message;

  message = (char *) duk_get_string(ctx, 0);
  debug(message, NULL);
  duk_pop(ctx);
  return (duk_ret_t) 0;
}

static duk_ret_t get_controllers(duk_context *ctx) {
    SDL_GameController *controller;
    duk_idx_t array_index;

    array_index = duk_push_array(ctx); // We create an empty JS array

    for (int i = 0;i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            duk_push_object(ctx); // We create an empty JS object
            //
            duk_push_boolean(ctx, (duk_bool_t)1);
            // In case something breaks here : this 1 was originally a "true" in renderer.c. But I don't know which imports "true" comes from.
            duk_put_prop_string(ctx, -2, "connected");
            //
            duk_push_string(ctx, SDL_GameControllerName(controller));
            duk_put_prop_string(ctx, -2, "id");
            //
            get_controller_buttons(controller, ctx);
            duk_put_prop_string(ctx, -2, "buttons");
            duk_put_prop_index(ctx, array_index, i); // Adding the (updated) object to the array
        }
    }
    return (duk_ret_t) 1;
}

/* 

static duk_ret_t tcp_socket(duk_context *ctx)

    Open a TCP socket

*/
static duk_ret_t tcp_socket_create(duk_context *ctx) {
  char *name;
  int socket;

  name = (char *) duk_get_string(ctx, 0);
  socket = socket_create_tcp(name);
  
  duk_pop(ctx);
  if (socket < 0) {
    duk_push_int(ctx, -1);    
  } else {
    duk_push_int(ctx, socket);
  }

  printf("C: Created socket %i\n", socket);
  return (duk_ret_t) 1;
}

static duk_ret_t tcp_socket_connect(duk_context *ctx) {
  int socket;
  char *host;
  int port;
  int ret;
  int error;

  socket = duk_get_int(ctx, 0);
  host = (char *)duk_get_string(ctx, 1);
  port = duk_get_int(ctx, 2);
  ret = socket_connect(socket, host, port);

  duk_pop_3(ctx);
  if (ret < 0) {
    error = errno;
    if (error == 111) { // Connection Refused (RST)
      duk_push_int(ctx, -1);
    } else if (error == 110) {
      duk_push_int(ctx, -2); // Connection Timed Out
    } else {
      duk_push_int(ctx, -3); // Other errors
    }  } else {
    duk_push_int(ctx, socket);
  }
  return (duk_ret_t) 1;
}

static duk_ret_t tcp_socket_close(duk_context *ctx) {
  int socket;

  socket = duk_get_int(ctx, 0);
  socket_close(socket);
  return (duk_ret_t) 0;
}

static duk_ret_t quark_socket_set_timeout(duk_context *ctx) {
  int socket;
  int timeout;
  int ret;


  socket = duk_get_int(ctx, 0);
  timeout = duk_get_int(ctx, 1);
	printf("Setting timeout to socket %i %i\n", socket, timeout);
  ret = socket_set_timeout(socket, timeout);
  duk_push_int(ctx, ret);
  return (duk_ret_t) 1;
}

static duk_ret_t quark_socket_set_nonblocking(duk_context *ctx) {
  int socket;
  int nonblocking;
  int ret;

  socket = duk_get_int(ctx, 0);
  nonblocking = duk_get_int(ctx, 1);
  ret = socket_set_nonblocking(socket, nonblocking);
  duk_push_int(ctx, ret);
  return (duk_ret_t) 1;
}


/* 

void init_js_globals(duk_context *ctx)

  Create js globals in the duk_context parameter

*/
void init_js_globals(duk_context *ctx) {
    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "quark"); // Creating a "quark" global

    duk_push_c_function(ctx, quark_python_init, 0);
    duk_put_global_string(ctx, "c_pythonInit");

    duk_push_c_function(ctx, quark_python_call, 2);
    duk_put_global_string(ctx, "c_pythonCall");

    duk_push_c_function(ctx, get_controllers, 0);
    duk_put_global_string(ctx, "c_getGamepads");

    duk_push_c_function(ctx, quark_debug, 1);
    duk_put_global_string(ctx, "quark_debug");

    duk_push_c_function(ctx, quark_socket_set_timeout, 2);
    duk_put_global_string(ctx, "socket_set_timeout");

    duk_push_c_function(ctx, quark_socket_set_nonblocking, 2);
    duk_put_global_string(ctx, "socket_set_nonblocking");

    duk_push_c_function(ctx, tcp_socket_create, 1);   
    duk_put_global_string(ctx, "TCPSocket_create");

    duk_push_c_function(ctx, tcp_socket_connect, 3);   
    duk_put_global_string(ctx, "TCPSocket_connect");


    duk_push_c_function(ctx, tcp_socket_close, 1);
    duk_put_global_string(ctx, "socket_close");

    duk_push_c_function(ctx, quark_exit, 0);
    duk_put_global_string(ctx, "c_exit");
}

