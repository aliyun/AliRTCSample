const zlib = require('zlib');

function compress(data) {
    return zlib.deflateSync(data);
}

function decompress(data) {
    return zlib.inflateSync(data);
}


module.exports = {
    compress: compress,
    decompress: decompress
}