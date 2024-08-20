// Quark Internals
//
// In the C code, we have
// enum update_types {
//     RECT,
//     SURFACE,
//     RENDER_PROPERTIES,
//     STYLE,
//     EL,
//     INTERNAL_ID,
//     STYLE_SIZE,
//     TAG,
//     ID,
//     INNER_TEXT
// };

// == Globals ==
document = {}
window = {}

// == Types ==
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
        writable: true
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
    // return this

}

// Functions 
document.getElementById = function(id) {
  element = quark.c_getElementById(id)
  e = new Element(element); // Here, we're not doing return new Element() because it seems to override the same object in duktape
  return e
}

// end of custom JS code for internals