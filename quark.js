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

// Globals
document = {}
//
window = {}
//

document.getElementById = function(id) {

  element = quark.c_getElementById(id)
  output = {}
  console.log('C element is', element)
  Object.defineProperty(output, "_id", {
    enumerable: false,
    value: element.id,
    writable: true
  })
  Object.defineProperty(output, "_internalId", {
    enumerable: false,
    value: element.internalId,
    writable: false
  })
  Object.defineProperty(output, "_innerText", {
    enumerable: false,
    value: element.innerText,
    writable: true
  })


  Object.defineProperty(output, "id", {
    get: function() {
      return this._id
    },
    set: function(id) {
      this._id = id;
      quark.c_updateElement(this._internalId, 8, id)
    }
  })
  Object.defineProperty(output, "innerText", {
    get: function() {
      return this._innerText
    },
    set: function(innerText) {
      this._innerText = innerText;
      quark.c_updateElement(this._internalId, 9, innerText)
    }
  })

  return output
}
// end of custom JS code for internals