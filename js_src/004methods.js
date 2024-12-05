// == Global Objects Methods Overrides ==
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
      c_setLocation(location);
  },
  configurable: true
})

//