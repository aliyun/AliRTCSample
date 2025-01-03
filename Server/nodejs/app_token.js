const { Buffer } = require('buffer');
const sign_utils = require('./util/sign_utils');
const bytes_utils = require('./util/bytes_utils');
const { AppTokenOptions } = require('./app_token_options');
const compress_utils = require('./util/compress_utils');
const { Service } = require('./service');

const VERSION_0 = "000";
const VERSION_LENGTH = 3;

class AppToken {
    constructor(appId, appKey, timestamp, issueTimestamp = null, salt = null, service = null, options = null, signature = null) {
        this.appId = appId;
        this.appKey = appKey;
        this.timestamp = timestamp;
        this.issueTimestamp = issueTimestamp || Math.floor(Date.now() / 1000);
        this.salt = salt || Math.floor(Math.random() * this.issueTimestamp);
        this.service = service;
        this.options = options || new AppTokenOptions();
        this.signature = signature;
    }

    setOptions(options) {
        this.options = options || new AppTokenOptions();
    }

    setService(service) {
        if (!service) throw new Error('missing service');
        this.service = service;
    }

    build() {
        if (!this.appKey) throw new Error('missing secretKey');
        if (!this.service) throw new Error('missing service');

        const signKey = sign_utils.generateSign(this.appKey, this.issueTimestamp, this.salt);

        let bufBody = new bytes_utils.ByteBuffer(0)
        bufBody.writeString(this.appId)

        bufBody.writeInt32(this.issueTimestamp)
        bufBody.writeInt32(this.salt)
        bufBody.writeInt32(this.timestamp)
        bufBody.writeBytes(this.service.pack())
        bufBody.writeBytes(this.options.pack())

        const signature = sign_utils.sign(signKey, bufBody.getvalue());

        let buf = new bytes_utils.ByteBuffer(0)
        buf.writeInt32(signature.length)
        buf.writeBytes(signature);
        buf.writeBytes(bufBody.getvalue())

        return VERSION_0 + compress_utils.compress(buf.getvalue()).toString('base64');
    }

    static parse(token) {
        if (!token) throw new Error('empty token');
        if (!token.startsWith(VERSION_0)) throw new Error('unsupported version');

        let buffer = compress_utils.decompress(Buffer.from(token.slice(VERSION_LENGTH), 'base64'))

        buffer = new bytes_utils.ReadonlyByteBuffer(buffer)

        let signatureLength = buffer.readInt32()
        let signature = buffer.readBytes(signatureLength)
        let appId = buffer.readString()
        let issueTimestamp = buffer.readInt32()
        let salt = buffer.readInt32()
        let timestamp = buffer.readInt32()

        let service = Service.unpack(buffer)
        let options = AppTokenOptions.unpack(buffer)

        return new AppToken(appId, null, timestamp, issueTimestamp, salt, service, options, signature)
    }
}

module.exports.AppToken = AppToken
