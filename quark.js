// Quark Internals
//
// In the C code, we have
// enum update_types {
//     RECT, 0
//     SURFACE, 1
//     RENDER_PROPERTIES, 2
//     STYLE, 3
//     EL, 4
//     INTERNAL_ID, 5
//     STYLE_SIZE, 6
//     TAG, 7
//     ID, 8
//     INNER_TEXT, 9
//     COLOR, 10
//     BACKGROUND_COLOR 11
// };

// == Globals ==
document = {}
window = {}

// == Types ==

function Style(style_obj) {
    Object.defineProperty(this, "_color", {
	enumerable: false,
	value: style_obj.color,
	writable: true,
	configurable: true
    })

    Object.defineProperty(this, "_backgroundColor", {
	enumerable: false,
	value: style_obj.backgroundColor,
	writable: true,
	configurable: true
    })

    Object.defineProperty(this, "color", {
        get: function() {
            return this._color;
        },
        set: function(color) {
            this._color = color;
            quark.c_updateElement(this._internalId, 10, color)
        },
        configurable: true
    })

    Object.defineProperty(this, "backgroundColor", {
        get: function() {
            return this._backgroundColor;
        },
        set: function(color) {
            this._backgroundColor = backgroundColor;
            quark.c_updateElement(this._internalId, 11, backgroundColor)
        },
        configurable: true
    })

}

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
            quark.c_updateElement(this._internalId, 8, id)
        },
        configurable: true
    })

    Object.defineProperty(this, "innerText", {
        get: function() {
            return this._innerText;
        },
        set: function(innerText) {
            this._innerText = innerText;
            quark.c_updateElement(this._internalId, 9, innerText)
        },
        configurable: true
    })

    Object.defineProperty(this, "style", {
	value: new Style({
	    color: element_obj.color,
	    backgroundColor: element_obj.backgroundColor
	}),
	configurable: true,
	writable: true
    })
}

// Functions 
document.getElementById = function(id) {
  element = quark.c_getElementById(id)
  console.log(element)
  e = new Element(element); // Here, we're not doing return new Element() because it seems to override the same object in duktape
  return e
}

// end of custom JS code for internals
