const { ByteBuffer } = require('./util/bytes_utils');

class AppTokenOptions {
    constructor(engineOptions = {}) {
        this.engineOptions = engineOptions;
    }

    setEngineOptions(engineOptions) {
        this.engineOptions = engineOptions != null ? engineOptions : {};
    }

    pack() {
        let buf = new ByteBuffer(0)
        let hasEngineOptions = this.engineOptions !== null

        buf.writeBool(hasEngineOptions)
        if (hasEngineOptions) {
            const engineOptionsEntries = Object.entries(this.engineOptions).sort();
            buf.writeInt32(engineOptionsEntries.length)
            
            for (const [key, value] of engineOptionsEntries) {
                if (key == null || value == null) {
                    throw new Error('Illegal engineOptions entry');
                }

                buf.writeString(key);
                buf.writeString(value);
            }
        }

        return buf.getvalue(true)
    }

    static unpack(buffer) {
        let hasEngineOptions = buffer.readBool();
        if (!hasEngineOptions) {
            return new AppTokenOptions();
        }
        let engineOptions = {};
        let size = buffer.readInt32();
        for (let i = 0; i < size; i++) {
            let key = buffer.readString()
            let value = buffer.readString()
            engineOptions[key] = value;
        }
        return new AppTokenOptions(engineOptions);
    }
}

module.exports.AppTokenOptions = AppTokenOptions
