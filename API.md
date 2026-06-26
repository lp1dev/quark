# Quark HTML/JS API Reference

Quark renders HTML/CSS and runs JavaScript on a Duktape engine, targeting PS Vita and GNU/Linux. This document covers the JavaScript API surface exposed to app code.

The runtime is `quark.js`, which must sit next to the `quark` binary (or be bundled into the VPK). It defines the DOM-like wrappers and is backed by native `c_*` / `quark_*` functions registered from the C side (`src/browser/js.c`, `src/renderer.c`).

> Quark implements a deliberately small subset of the web platform. Anything not listed here is unsupported. See [Limitations](#limitations).

---

## Loading scripts

Only **inline** `<script>` blocks execute. The `src` attribute is not supported, and external JSON/JS/CSS loading is not implemented.

```html
<script type="text/javascript">
  // your code here
</script>
```

Supported HTML tags: `body`, `head`, `div`, `p`, `h1`–`h4`, `img`, `button`, `script`.

---

## Globals

These objects exist at startup:

| Global | Purpose |
|---|---|
| `document` | Element lookup and event registration |
| `window` | Timers, navigation, lifecycle |
| `navigator` | Gamepad access |
| `keyboard` | USB HID keyboard injection helpers + keycode tables |
| `events` | Internal listener registry (`events.listeners`) |
| `gamepad` | Holds `gamepad.mapping` (button-index → name) |
| `platforms` | `{ LINUX: 0x00, VITA: 0x01 }` |
| `console` | `console.log` etc. (Duktape console module) |

---

## `document`

### `document.getElementById(id)` → `Element`

Returns an `Element` wrapper for the matching node. Each call constructs a fresh wrapper (Duktape reuses object identity, so don't cache and compare).

```js
var out = document.getElementById("outputarea");
out.innerText = "Hello from Quark!";
```

### `document.addEventListener(event, callback)`

Registers a handler. The callback receives `{ type, code }`. See [Events](#events).

---

## `Element`

Returned by `getElementById`. Setting a property immediately triggers a native re-render of that node.

| Property | Access | Notes |
|---|---|---|
| `id` | get/set | Updates the node's `id` attribute |
| `innerText` | get/set | Plain text content |
| `innerHTML` | get/set | Parsed as HTML |
| `style` | object | A `Style` instance (see below) |
| `attributes` | object | An `Attributes` instance (see below) |

```js
var el = document.getElementById("title");
el.innerText = "Updated";
el.style.color = "red";
```

Internal fields (`_internalId`, `_x`, `_y`, `_width`, `_height`) exist but are non-enumerable plumbing; treat them as private.

### `Style`

Only three properties are wired to the renderer from JS. Setting any of them re-renders the element. Other CSS properties only take effect when declared in the stylesheet / inline HTML at parse time, not when mutated from JS.

| Property | Maps to CSS |
|---|---|
| `style.color` | `color` |
| `style.backgroundColor` | `background-color` |
| `style.display` | `display` (defaults to `block`) |

### `Attributes`

| Property | Access | Notes |
|---|---|---|
| `attributes.id` | get/set | Same backing as `Element.id` |
| `attributes.onclick` | get/set | **Must be a string**, not a function (see quirk below) |

**`onclick` quirk:** Duktape cannot serialize functions to strings ([duktape#352](https://github.com/svaarala/duktape/issues/352)). When you set `onclick` from JS it must be a string of code; assigning a function throws. The getter `eval()`s the stored string. In HTML attributes this is transparent:

```html
<button id="b" onclick="counterup()">Click</button>
```

```js
// From JS — string, not a function:
document.getElementById("b").attributes.onclick = "counterup()";   // OK
document.getElementById("b").attributes.onclick = counterup;       // throws
```

---

## `window` and timers

| Member | Signature | Notes |
|---|---|---|
| `window.setTimeout` | `(callback, ms)` → `id` | Also global as `setTimeout` |
| `window.setInterval` | `(callback, ms)` → `id` | Also global as `setInterval` |
| `window.clearTimeout` | `(id)` | Also global as `clearTimeout` |
| `window.clearInterval` | `(id)` | Also global as `clearInterval` |
| `window.close` | `()` | Exits the app |
| `window.location` | set | Navigates; see below |

`getElementById` is also aliased to the global scope (`getElementById(...)`).

### `window.location`

Assigning navigates to another HTML page in the project. The query string is stripped before navigation.

```js
window.location = "hosts.html?foo=bar";   // navigates to hosts.html
```

---

## Events

Register with `document.addEventListener(type, cb)`. The callback receives `{ type, code }`.

| Event | `code` payload |
|---|---|
| `keydown` | SDL keysym of the pressed key |
| `button_press` | SDL controller button index — map via `gamepad.mapping[code]` |
| `gamepadconnected` | `0` (multi-controller not yet implemented) |

Element `onclick` fires on mouse click and on front-touchscreen tap (Vita `touchId 1`; the rear panel is not yet wired).

```js
document.addEventListener("keydown", function (event) {
  document.getElementById("keypressed").innerText = "Key " + event.code;
});

document.addEventListener("button_press", function (event) {
  document.getElementById("keypressed").innerText =
    gamepad.mapping[event.code] + " pressed";
});
```

---

## Gamepad

### `navigator.getGamepads()` → `Array`

Returns connected controllers. Each entry:

```js
{
  connected: true,
  id: "<controller name>",
  buttons: [ { pressed, touched, value }, ... ]  // up to 22 entries (SDL)
}
```

`gamepad.mapping` maps the first 15 button indices to names (`X`, `CIRCLE`, `SQUARE`, `TRIANGLE`, `SHARE`, `PS`, `OPTIONS`, `LEFTSTICK`, `RIGHTSTICK`, `LEFTSHOULDER`, `RIGHTSHOULDER`, `DPAD_UP`, `DPAD_DOWN`, `DPAD_LEFT`, `DPAD_RIGHT`).

---

## Networking

### `Socket`

TCP only.

```js
var s = new Socket(host, port);
```

| Method | Signature | Returns |
|---|---|---|
| `connect()` | `()` | `int` status (see codes below) |
| `close()` | `()` | — |
| `ping()` | `()` | `int` |
| `setTimeout(timeout)` | `(timeout)` | `int` |
| `setNonBlocking(nonblocking)` | `(0\|1)` | `int` |

Observed `connect()` return codes (from the port scanner example):

| Value | Meaning |
|---|---|
| `>= 0` | open / connected |
| `-6` | RST received (closed) |
| `-5` | no response (likely closed/filtered) |

```js
var s = new Socket(host, port);
var ret = s.connect();
if (ret >= 0) { /* open */ }
s.close();
```

### Network globals

| Function | Signature | Returns |
|---|---|---|
| `quark_ping(host)` | `(host)` | `int` |
| `net_get_device_info()` | `()` | object (below) |

```js
var info = net_get_device_info();
// { ipv4, mac, gateway, netmask, state }
// state === 3  -> CONNECTED
```

---

## USB HID keyboard injection

The `keyboard` object drives USB keyboard emulation (the Vipper Zero use case). Native bindings:

| Function | Signature | Returns |
|---|---|---|
| `keyboard_init()` | `()` | `int` |
| `keyboard_shutdown()` | `()` | `int` |
| `keyboard_send_string(str)` | `(str)` | `int` |
| `keyboard_send_key(modifier, hid_code)` | `(mod, code)` | `int` |

`modifier` is a USB HID modifier bitmask; `hid_code` is a HID Usage Page 0x07 keycode.

### Keycode tables

- `keyboard.KEY_MOD` — modifier bits: `LCTRL` `LSHIFT` `LALT` `LMETA` `RCTRL` `RSHIFT` `RALT` `RMETA`, plus aliases `CTRL` `SHIFT` `ALT` `WIN`/`CMD`.
- `keyboard.HID` — full keycode set: letters `A`–`Z`, digits `NUM_0`–`NUM_9`, `ENTER` `ESC` `BACKSPACE` `TAB` `SPACE`, punctuation, `F1`–`F12`, navigation/arrows, keypad `KP_*`, etc.

### Helpers

All build on `keyboard_send_key`:

| Helper | Signature | Notes |
|---|---|---|
| `keyboard.send_arrow(direction)` | `"UP"\|"DOWN"\|"LEFT"\|"RIGHT"` | Returns `-1` on bad direction |
| `keyboard.send_fkey(n)` | `1`–`12` | Returns `-1` out of range |
| `keyboard.send_ctrl(hid_code)` | HID code | Ctrl + key |
| `keyboard.send_alt(hid_code)` | HID code | Alt + key |
| `keyboard.send_win(hid_code)` | HID code | Win/Cmd + key |
| `keyboard.send_chord(mods, hid_code)` | `mods[]`, HID code | OR-combines modifier array |

```js
keyboard_init();
keyboard_send_string("hello");
keyboard.send_ctrl(keyboard.HID.C);                       // Ctrl+C
keyboard.send_chord([keyboard.KEY_MOD.CTRL,
                     keyboard.KEY_MOD.SHIFT],
                    keyboard.HID.ESC);                     // Ctrl+Shift+Esc
keyboard_shutdown();
```

---

## On-screen input (IME prompt)

### `prompt(title, initial, max_chars)` → `string | null`

Opens the system text-input dialog (Vita IME). Returns the entered string, or `null` if cancelled. Defaults: `title=""`, `initial=""`, `max_chars=64`.

```js
var name = prompt("Enter your name", "", 32);
if (name !== null) { /* ... */ }
```

---

## Filesystem

| Function | Signature | Returns |
|---|---|---|
| `quark_listdir(path)` | `(path)` | array of filename strings |
| `quark_readfile(path)` | `(path)` | file contents as string |
| `quark_writefile(data, path)` | `(data, path)` | `int` status |

> **Arg order:** `quark_writefile` takes **data first, then path**.

```js
var files = quark_listdir("ux0:/data/");
var text  = quark_readfile("ux0:/data/config.json");
quark_writefile("contents", "ux0:/data/out.txt");
```

---

## Audio

Tracker-style synth interface (channel / note / instrument / effect).

| Function | Signature |
|---|---|
| `quark_audio_note_on(channel, note, instrument, effect)` | `effect` is a hex value, e.g. `0xC20` |
| `quark_audio_note_off(channel)` | — |

```js
quark_audio_note_on(0, 60, 5, 0xC20);  // ch 0, note, instrument, effect
quark_audio_note_off(0);
```

---

## Platform & debug

| Function | Signature | Returns |
|---|---|---|
| `quark_get_platform()` | `()` | `int` — compare with `platforms.LINUX` (0) / `platforms.VITA` (1) |
| `quark_debug(message)` | `(message)` | prints and forwards to the TCP debugger |

```js
if (quark_get_platform() === platforms.VITA) { /* Vita-only path */ }
quark_debug("Scan done");
```

---

## Low-level C bridge (internal)

`quark.js` wraps these native globals; application code should use the high-level APIs above rather than call them directly:

`c_getElementById`, `c_updateElement`, `c_getElementStyle`, `c_getElementAttributes`, `c_setInterval`, `c_clearInterval`, `c_setLocation`, `c_exit`, `c_getGamepads`, `TCPSocket_create`, `TCPSocket_connect`, `TCPSocket_ping`, `socket_set_timeout`, `socket_set_nonblocking`, `socket_close`.

---

## Limitations

- **Scripts:** inline only; no `src`, no external JS/CSS/JSON loading.
- **`onclick` from JS:** must be a string, not a function reference.
- **Style from JS:** only `color`, `backgroundColor`, `display` re-render live; other CSS applies at parse time only.
- **CSS subset:** `display: inline|block`, `font-size`, `color`, `background-color`, `padding`, `margin`, `height`, `width`, `text-align`, `vertical-align`, `position`.
- **Negative `padding` (and likely `margin`) crashes the renderer.**
- **Gamepad:** only the first 15 buttons are mapped; multi-controller is not implemented (`gamepadconnected` always reports index `0`).
- **Touch:** front screen only; rear panel not wired.
- Early-stage project — expect breakage.