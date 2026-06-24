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

platforms = {
    LINUX: 0x00,
    VITA: 0x01
}

gamepad = {}

gamepad.mapping = [
      "X",
      "CIRCLE",
      "SQUARE",
      "TRIANGLE",
      "SHARE",
      "PS",
      "OPTIONS",
      "LEFTSTICK",
      "RIGHTSTICK",
      "LEFTSHOULDER",
      "RIGHTSHOULDER",
      "DPAD_UP",
      "DPAD_DOWN",
      "DPAD_LEFT",
      "DPAD_RIGHT",
] // Only implemented the first 15 buttons
