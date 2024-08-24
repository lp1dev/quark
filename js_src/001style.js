// In the C code, we have

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

//