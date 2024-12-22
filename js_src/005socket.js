// == Socket Class ==

function Socket(host, port) {
    Object.defineProperty(this, "_protocol", {
        enumerable: false,
        value: "tcp",
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "_fd", {
        enumerable: false,
        value: -1,
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "_host", {
        enumerable: false,
        value: host,
        writable: true,
        configurable: true
    })

    Object.defineProperty(this, "_port", {
        enumerable: false,
        value: port,
        writable: true,
        configurable: true
    })

    this.connect = function() {
        return TCPSocket_connect(this._fd, this._host, this._port)
    }

    this.close = function() {
        socket_close(this._fd)
    }

    this.setTimeout = function(timeout) {
        return socket_set_timeout(this._fd, timeout)
    }

    this.setNonBlocking = function(nonblocking) {
        return socket_set_nonblocking(this._fd, nonblocking)
    }

    this.ping = function() {
        return TCPSocket_ping(this._fd, this._host, this._port)
    }

    this._fd = TCPSocket_create(host+":"+port)
}