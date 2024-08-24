// == Globals ==

document = {}
window = {
    intervals: []
}
quark = {}

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
            console.log('Setting onclick to', onclick)
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
//     INNER_TEXT
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
    console.log(element)
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

quark_onClick = function(element) {
  el = new Element(element)
  if (el.attributes.onclick) {
    el.attributes.onclick()
  }
}

setTimeout = window.setTimeout
setInterval = window.setInterval
getElementById = document.getElementById

//