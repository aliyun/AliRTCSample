function nextMultiple(n, baseValue) {
    if (baseValue <= 0 || n <= 0) {
        return 0;
    }
    let result = baseValue;
    while (result < n) {
        result *= 2;
    }
    return result;
}


class ByteBuffer {
    constructor(size) {
        this.buffer = Buffer.alloc(size);
        this.writeOffset = 0
        this.readOffset = 0
    }

    ensureCapacity(additionalSize) {
        const requiredSize = this.writeOffset + additionalSize;
        if (requiredSize > this.buffer.length) {
            let newSize = nextMultiple(requiredSize, 256);
            const newBuffer = Buffer.alloc(newSize);
            this.buffer.copy(newBuffer);
            this.buffer = newBuffer;
        }
    }

    nextMultiple(n, baseValue) {
        if (baseValue <= 0 || n <= 0) {
            return 0;
        }
        let result = baseValue;
        while (result < n) {
            result *= 2;
        }
        return result;
    }

    readBool() {
        return !!this.readInt8();
    }

    readInt8() {
        return this.readInt(1);
    }

    readInt32() {
        return this.readInt(4);
    }

    readInt(byteSize) {
        let value;
        if (byteSize === 4) {
            value = this.buffer.readInt32BE(this.readOffset);
        } else if (byteSize === 1) {
            value = this.buffer.readInt8(this.readOffset);
        } else {
            throw new Error('Unsupported integer size');
        }
        this.readOffset += byteSize;

        return value;
    }

    readString() {
        let len = this.readInt32();
        const value = this.buffer.toString('utf8', this.readOffset, this.readOffset + len);
        this.readOffset += len;
        return value;
    }

    readBytes(len) {
        const value = this.buffer.slice(this.readOffset, this.readOffset + len);
        this.readOffset += len;
        return value;
    }

    writeBool(value) {
        this.writeInt8(value ? 1 : 0);
    }

    writeInt8(value) {
        this.writeInt(value, 1);
    }

    writeInt32(value) {
        this.writeInt(value, 4);
    }

    writeInt(value, byteSize) {
        this.ensureCapacity(byteSize);
        if (byteSize === 4) {
            this.buffer.writeInt32BE(value, this.writeOffset);
        } else if (byteSize === 1) {
            this.buffer.writeInt8(value, this.writeOffset);
        } else {
            throw new Error('Unsupported integer size')
        }

        this.writeOffset += byteSize;
    }

    writeString(value) {
        let len = Buffer.byteLength(value, 'utf-8');
        this.writeInt32(len);
        this.ensureCapacity(len);
        this.writeBytes(value)
    }

    writeBytes(value) {
        let len = Buffer.byteLength(value, 'utf-8');
        this.ensureCapacity(len);

        if (!Buffer.isBuffer(value)) {
            value = Buffer.from(value);
        }
        value.copy(this.buffer, this.writeOffset);
        this.writeOffset += len;
    }

    getvalue(shrink = false) {
        if (shrink && this.writeOffset < this.buffer.length) {
            let newBuffer = Buffer.alloc(this.writeOffset);
            this.buffer.copy(newBuffer);
            return newBuffer
        } else {
            return this.buffer;
        }
    }
}

class ReadonlyByteBuffer extends ByteBuffer {
    constructor(buffer) {
        super(buffer.length)
        this.buffer = buffer;
        this.writeOffset = buffer.length
        this.readOffset = 0
    }

    writeInt(value, byteSize) {
        throw new Error('Unsupported write mode')
    }

    writeString(value) {
        throw new Error('Unsupported write mode')
    }

    writeBytes(value) {
        throw new Error('Unsupported write mode')
    }
}

module.exports.ByteBuffer = ByteBuffer
module.exports.ReadonlyByteBuffer = ReadonlyByteBuffer
