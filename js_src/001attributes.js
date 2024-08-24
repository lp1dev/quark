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

//