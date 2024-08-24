// == Global Objects Methods Overrides ==
//  Done after global definition because we need the element class

document.getElementById = function(id) {
    element = quark.c_getElementById(id)
    console.log(element)
    e = new Element(element); // Here, we're not doing return new Element() because it seems to override the same object in duktape
    return e
}

window.setTimeout = function(callback, timeout_duration) {
  intervalID = parseInt(Math.random() * 1000000);
  window.intervals.push({id: intervalID, callback, timeout_duration});
  c_setInterval(intervalID, -123456789, timeout_duration);
  return intervalID
}

quark_executeInterval = function(intervalID) {
  i = 0
  while (window.intervals[i]) {
    if (window.intervals[i].id == intervalID) {
      console.log(window.intervals[i])
      window.intervals[i].callback()
    }
    i++
  }
}

setTimeout = window.setTimeout
getElementById = document.getElementById

//