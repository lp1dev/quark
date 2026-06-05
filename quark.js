// == Globals ==

document = {}
window = {
    intervals: [],
    _location: "index.html"
}
events = {
    listeners: {
    }
}
navigator = {}
keyboard = {}

//// In the C code, we have

// enum update_types {
//     RECT,
//     SURFACE,
//     RENDER_PROPERTIES,
//     ATTRIBUTES,
//     STYLE,
//     EL,
//     INTERNAL_ID,
//     TAG,
//     ID,
//     INNER_TEXT
// };

// == Style Class == 

function Attributes(attributes) {
    Object.defineProperty(this, "_id", {
    	enumerable: false,
	    value: attributes.id,
	    writable: true,
	    configurable: true
    })

    Object.defineProperty(this, "_onclick", {
    	enumerable: false,
	    value: attributes['onclick'],
	    writable: true,
	    configurable: true
    })

    Object.defineProperty(this, "id", {
        get: function() {
            return this._id;
        },
        set: function(id) {
            this._id = id;
            quark.c_updateElement(attributes.internalId, 8, "id", id)
        },
        configurable: true
    })

    Object.defineProperty(this, "onclick", {
        get: function() {
            return eval(this._onclick);
        },
        set: function(onclick) {
            this._onclick = String(onclick);
            // Functions cannot be converted to string with duktape
            // https://github.com/svaarala/duktape/issues/352
            // They will be passed as string for the moment
            // And I'll have to find a solution for this later, maybe store 
            // them somewhere with IDs I'll call later
            if (typeof(onclick) !== 'string') {
                throw new Error("OnClick attributes must be strings, not functions with quark!");
            }
            quark.c_updateElement(attributes['internalId'], 3, "onclick", onclick)
        },
        configurable: true
    })

}

//// In the C code, we have

// enum update_types {
//     RECT,
//     SURFACE,
//     RENDER_PROPERTIES,
//     ATTRIBUTES,
//     STYLE,
//     EL,
//     INTERNAL_ID,
//     TAG,
//     ID,
//     INNER_TEXT,
//     HTML
// };

// == Style Class == 

function Style(style_obj) {
    Object.defineProperty(this, "_color", {
	enumerable: false,
	value: style_obj.color,
	writable: true,
	configurable: true
    })

    Object.defineProperty(this, "_backgroundColor", {
	enumerable: false,
	value: style_obj['background-color'],
	writable: true,
	configurable: true
    })

    Object.defineProperty(this, "_display", {
        enumerable: false,
        value: style_obj['display'] ? style_obj['display'] : 'block',
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "color", {
        get: function() {
            return this._color;
        },
        set: function(color) {
            this._color = color;
            quark.c_updateElement(style_obj['internalId'], 4, "color", color)
        },
        configurable: true
    })

    Object.defineProperty(this, "backgroundColor", {
        get: function() {
            return this._backgroundColor;
        },
        set: function(backgroundColor) {
            this._backgroundColor = backgroundColor;
            quark.c_updateElement(style_obj['internalId'], 4, "background-color", backgroundColor)
        },
        configurable: true
    })

    Object.defineProperty(this, "display", {
        get: function() {
            return this._display;
        },
        set: function(displayValue) {
            this._display = displayValue;
            quark.c_updateElement(style_obj['internalId'], 4, "display", displayValue)
        },
        configurable: true
    })

}

//// == Element Class ==


function Element(element_obj) {
    Object.defineProperty(this, "_id", {
        enumerable: false,
        value: element_obj.id,
        writable: true,
        configurable: true
    })


    Object.defineProperty(this, "_internalId", {
        enumerable: false,
        configurable: true,
        value: element_obj.internalId,
        writable: false
    })

    Object.defineProperty(this, "_innerText", {
        enumerable: false,
        value: element_obj.innerText,
        writable: true
    })

    Object.defineProperty(this, "_innerHTML", {
        enumerable: false,
        value: element_obj.innerHTML,
        writable: true
    })

    Object.defineProperty(this, "_x", {
        enumerable: false,
        value: element_obj.x,
        writable: false
    })

    Object.defineProperty(this, "_y", {
        enumerable: false,
        value: element_obj.y,
        writable: false
    })

    Object.defineProperty(this, "_width", {
        enumerable: false,
        value: element_obj.width,
        writable: false
    })

    Object.defineProperty(this, "_height", {
        enumerable: false,
        value: element_obj.height,
        writable: false
    })


    Object.defineProperty(this, "id", {
        get: function() {
            return this._id;
        },
        set: function(id) {
            this._id = id;
            quark.c_updateElement(this._internalId, 8, "id", id);
        },
        configurable: true
    })

    Object.defineProperty(this, "innerText", {
        get: function() {
            return this._innerText;
        },
        set: function(innerText) {
            this._innerText = innerText;
            quark.c_updateElement(this._internalId, 9, "innerText", innerText);
        },
        configurable: true
    })

    Object.defineProperty(this, "innerHTML", {
        get: function() {
            return this._innerHTML;
        },
        set: function(innerHTML) {
            this._innerHTML = innerHTML;
            quark.c_updateElement(this._internalId, 10, "innerHTML", innerHTML);
        },
        configurable: true
    })

    Object.defineProperty(this, "style", {
	    value: new Style(c_getElementStyle(this._internalId)),
	    configurable: true,
	    writable: true
    })

    Object.defineProperty(this, "attributes", {
        value: new Attributes(c_getElementAttributes(this._internalId)),
        configurable: true,
        writable: true
    })
}

// // == Global Objects Methods Overrides ==
//  Done after global definition because we need the element class

document.getElementById = function(id) {
    element = quark.c_getElementById(id)
    e = new Element(element); // Here, we're not doing return new Element() because it seems to override the same object in duktape
    return e
}

window.setTimeout = function(callback, timeout_duration) {
  intervalID = parseInt(Math.random() * 1000000);
  window.intervals.push({id: intervalID, callback, timeout_duration})
  c_setInterval(intervalID, -123456789, timeout_duration);
  return intervalID
}

window.setInterval = function(callback, interval_duration) {
  intervalID = parseInt(Math.random() * 1000000);
  window.intervals.push({id: intervalID, callback, interval_duration})
  c_setInterval(intervalID, interval_duration, -123456789)
  return intervalID
}

window.clearInterval = function(intervalID) {
  c_clearInterval(intervalID)
}

window.clearTimeout = function(timeoutID) {
  c_clearInterval(timeoutID)
}

window.close = function() {
  c_exit()
}

quark_executeInterval = function(intervalID) {
  i = 0
  while (window.intervals[i]) {
    if (window.intervals[i].id == intervalID) {
      window.intervals[i].callback()
    }
    i++
  }
}

quark_onClick = function(element) {
  console.log("\tJS: Onclick()", element)
  console.log("\tJS: el.internal_id", element.internalId)

  el = new Element(element)
  if (el.attributes.onclick) {
    el.attributes.onclick()
  }
}

setTimeout = window.setTimeout
setInterval = window.setInterval
clearInterval = window.clearInterval
clearTimeout = window.clearTimeout
getElementById = document.getElementById

//

quark_onEvent = function(type, datacode) {
  if (events.listeners[type] == undefined) {
    events.listeners[type] = []
  }
  else {
    events.listeners[type].forEach(function (callback) {
        callback({type: type, code: datacode})
    })
  }
}

document.addEventListener = function (event, callback) {
  if (events.listeners[event] == undefined) {
    events.listeners[event] = []
  }
  events.listeners[event].push(callback)
}

navigator.getGamepads = function() {
  var gamepads = c_getGamepads()
  return gamepads
}


Object.defineProperty(window, "location", {
  get: function() {
      return window._location;
  },
  set: function(location) {
      window._location = location;
      c_setLocation(location.split('?')[0]);
  },
  configurable: true
})

//
// == Socket Class ==

function Socket(host, port) {
    Object.defineProperty(this, "_protocol", {
        enumerable: false,
        value: "tcp",
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "_fd", {
        enumerable: false,
        value: -1,
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "_host", {
        enumerable: false,
        value: host,
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "_port", {
        enumerable: false,
        value: port,
        writable: true,
        configurable: true
    })

    this.connect = function() {
        return TCPSocket_connect(this._fd, this._host, this._port)
    }

    this.close = function() {
        socket_close(this._fd)
    }

    this.setTimeout = function(timeout) {
        return socket_set_timeout(this._fd, timeout)
    }

    this.setNonBlocking = function(nonblocking) {
        return socket_set_nonblocking(this._fd, nonblocking)
    }

    this.ping = function() {
        return TCPSocket_ping(this._fd, this._host, this._port)
    }

    this._fd = TCPSocket_create(host+":"+port)
}// keyboard.js — JS-side keyboard helpers for Vipper Zero.
// Add to quark.js or load alongside it.
//
// Requires the following C bindings to be registered:
//   keyboard_init()                          -> int
//   keyboard_shutdown()                      -> int
//   keyboard_send_string(str)                -> int
//   keyboard_send_shortcut(modifier, char)   -> int   (optional)
//   keyboard_send_key(modifier, hid_code)    -> int


// ---------------------------------------------------------------
// Modifier bitmask (USB HID modifier byte)
// ---------------------------------------------------------------
keyboard.KEY_MOD = {
    LCTRL:  0x01,
    LSHIFT: 0x02,
    LALT:   0x04,
    LMETA:  0x08,   // Windows key on PC, Command on Mac
    RCTRL:  0x10,
    RSHIFT: 0x20,
    RALT:   0x40,   // AltGr on PC
    RMETA:  0x80,

    // Aliases
    CTRL:   0x01,
    SHIFT:  0x02,
    ALT:    0x04,
    WIN:    0x08,
    CMD:    0x08
};

// ---------------------------------------------------------------
// HID keycodes (USB HID Usage Page 0x07, Keyboard/Keypad)
// Use these with keyboard_send_key(modifier, code)
// ---------------------------------------------------------------
keyboard.HID = {
    // Letters
    A: 0x04, B: 0x05, C: 0x06, D: 0x07, E: 0x08, F: 0x09,
    G: 0x0A, H: 0x0B, I: 0x0C, J: 0x0D, K: 0x0E, L: 0x0F,
    M: 0x10, N: 0x11, O: 0x12, P: 0x13, Q: 0x14, R: 0x15,
    S: 0x16, T: 0x17, U: 0x18, V: 0x19, W: 0x1A, X: 0x1B,
    Y: 0x1C, Z: 0x1D,

    // Digits (top row)
    NUM_1: 0x1E, NUM_2: 0x1F, NUM_3: 0x20, NUM_4: 0x21, NUM_5: 0x22,
    NUM_6: 0x23, NUM_7: 0x24, NUM_8: 0x25, NUM_9: 0x26, NUM_0: 0x27,

    // Control / whitespace
    ENTER:     0x28,
    ESC:       0x29,
    BACKSPACE: 0x2A,
    TAB:       0x2B,
    SPACE:     0x2C,

    // Punctuation
    MINUS:        0x2D,
    EQUAL:        0x2E,
    LEFTBRACE:    0x2F,
    RIGHTBRACE:   0x30,
    BACKSLASH:    0x31,
    SEMICOLON:    0x33,
    APOSTROPHE:   0x34,
    GRAVE:        0x35,
    COMMA:        0x36,
    DOT:          0x37,
    SLASH:        0x38,
    CAPSLOCK:     0x39,

    // F-keys
    F1: 0x3A, F2: 0x3B, F3: 0x3C, F4: 0x3D, F5: 0x3E, F6: 0x3F,
    F7: 0x40, F8: 0x41, F9: 0x42, F10: 0x43, F11: 0x44, F12: 0x45,

    // Navigation
    PRINTSCREEN: 0x46,
    SCROLLLOCK:  0x47,
    PAUSE:       0x48,
    INSERT:      0x49,
    HOME:        0x4A,
    PAGEUP:      0x4B,
    DELETE:      0x4C,
    END:         0x4D,
    PAGEDOWN:    0x4E,

    // Arrow keys
    RIGHT: 0x4F,
    LEFT:  0x50,
    DOWN:  0x51,
    UP:    0x52,

    // Keypad
    NUMLOCK:     0x53,
    KP_SLASH:    0x54,
    KP_ASTERISK: 0x55,
    KP_MINUS:    0x56,
    KP_PLUS:     0x57,
    KP_ENTER:    0x58,
    KP_1: 0x59, KP_2: 0x5A, KP_3: 0x5B, KP_4: 0x5C, KP_5: 0x5D,
    KP_6: 0x5E, KP_7: 0x5F, KP_8: 0x60, KP_9: 0x61, KP_0: 0x62,
    KP_DOT: 0x63,

    // Misc
    APPLICATION: 0x65,   // Menu / context key
    POWER:       0x66,

    // Modifier-as-key (rarely needed; modifiers go in the mod byte)
    LCTRL_KEY:  0xE0,
    LSHIFT_KEY: 0xE1,
    LALT_KEY:   0xE2,
    LMETA_KEY:  0xE3,
    RCTRL_KEY:  0xE4,
    RSHIFT_KEY: 0xE5,
    RALT_KEY:   0xE6,
    RMETA_KEY:  0xE7
};

// ---------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------

// Send an arrow key by direction name.
keyboard.send_arrow = function (direction) {
    if (direction === "UP")    return keyboard_send_key(0, keyboard.HID.UP);
    if (direction === "DOWN")  return keyboard_send_key(0, keyboard.HID.DOWN);
    if (direction === "LEFT")  return keyboard_send_key(0, keyboard.HID.LEFT);
    if (direction === "RIGHT") return keyboard_send_key(0, keyboard.HID.RIGHT);
    return -1;
}

// Send a function key F1..F12 by number.
keyboard.send_fkey = function (n) {
    if (n < 1 || n > 12) return -1;
    return keyboard_send_key(0, keyboard.HID.F1 + (n - 1));
}

// Send Ctrl+key for the common copy/paste/etc. shortcuts.
//keyboardkey is an HID code (e.g. HID.C, HID.V, HID.Z).
keyboard.send_ctrl = function (hid_code) {
    return keyboard_send_key(keyboard.KEY_MOD.CTRL, hid_code);
}

// Send Alt+key.
keyboard.send_alt = function (hid_code) {
    return keyboard_send_key(keyboard.KEY_MOD.ALT, hid_code);
}

// Send Win+key (also Cmd+key on Mac — same HID modifier).
keyboard.send_win = function (hid_code) {
    return keyboard_send_key(keyboard.KEY_MOD.WIN, hid_code);
}

// Send a chord with any combination of modifiers.
// mods is an array of KEY_MOD values, e.g. [KEY_MOD.CTRL, KEY_MOD.SHIFT].
keyboard.send_chord = function (mods, hid_code) {
    var mask = 0;
    for (var i = 0; i < mods.length; i++) mask = mask | mods[i];
    return keyboard_send_key(mask, hid_code);
}