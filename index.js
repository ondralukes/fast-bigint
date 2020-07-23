const internal = require('./internal/build/Release/internal');

class Index{
    constructor(bufferOrNumber){
        const t = this;
        if(Buffer.isBuffer(bufferOrNumber)){
            t.setBuffer(bufferOrNumber);
        } else {
            if(typeof bufferOrNumber === 'number') {
                this.setBuffer(Buffer.alloc(8));
                t.setNumber(bufferOrNumber);
            } else {
                this.setBuffer(Buffer.alloc(1024));
            }
        }
    }

    setBuffer(buffer) {
        if(buffer.length % 8 !== 0){
            this.buffer = Buffer.alloc(Math.ceil(buffer.length / 8) * 8);
            buffer.copy(this.buffer);
        } else {
            this.buffer = buffer;
        }
    }

    setNumber(number) {
        if (number < 0)
            throw Error('Negative numbers are not supported.');

        internal.setUInt64(this.buffer, number);
    }

    static add(a, b){
        let resultLength = a.buffer.length > b.buffer.length ? a.buffer.length : b.buffer.length;
        if(a.buffer.length === b.buffer.length){
            //Allocate extra byte for overflow
            if(a.buffer.readUInt8(resultLength-1) + b.buffer.readUInt8(resultLength-1) > 255) resultLength++;
        }

        resultLength = Math.ceil(resultLength / 8) * 8;

        const resultBuffer = Buffer.allocUnsafe(resultLength);
        internal.add(resultBuffer, a.buffer, b.buffer);
        return new Index(resultBuffer);
    }
}

module.exports = Index;