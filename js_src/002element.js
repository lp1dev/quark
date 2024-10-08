// == Element Class ==


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

    Object.defineProperty(this, "_innerHTML", {
        enumerable: false,
        value: element_obj.innerHTML,
        writable: true
    })

    Object.defineProperty(this, "_x", {
        enumerable: false,
        value: element_obj.x,
        writable: false
    })

    Object.defineProperty(this, "_y", {
        enumerable: false,
        value: element_obj.y,
        writable: false
    })

    Object.defineProperty(this, "_width", {
        enumerable: false,
        value: element_obj.width,
        writable: false
    })

    Object.defineProperty(this, "_height", {
        enumerable: false,
        value: element_obj.height,
        writable: false
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

    Object.defineProperty(this, "innerHTML", {
        get: function() {
            return this._innerHTML;
        },
        set: function(innerHTML) {
            this._innerHTML = innerHTML;
            quark.c_updateElement(this._internalId, 10, "innerHTML", innerHTML);
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

// 