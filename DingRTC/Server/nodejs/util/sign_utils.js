const crypto = require('crypto');

function generateSign(key, timestamp, salt) {
    const bufTs = Buffer.alloc(4);
    bufTs.writeUInt32BE(timestamp, 0);

    const signKey = crypto.createHmac('sha256', bufTs).update(key).digest();

    const bufSalt = Buffer.alloc(4);
    bufSalt.writeUInt32BE(salt, 0);

    return crypto.createHmac('sha256', bufSalt).update(signKey).digest();
}

function sign(key, data) {
    return crypto.createHmac('sha256', key).update(data).digest();
}

module.exports = {
    sign: sign,
    generateSign: generateSign
}