const internal = require('./internal/build/Release/internal');

class FastBigInt{
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

    static setMaxThreads(count){
        if(typeof count !== 'number')
            throw Error('Thread count must be a number.');

        internal.setMaxThreads(count);
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
        return new FastBigInt(resultBuffer);
    }

    static addAsync(a, b, callback){
        let resultLength = a.buffer.length > b.buffer.length ? a.buffer.length : b.buffer.length;
        if(a.buffer.length === b.buffer.length){
            //Allocate extra byte for overflow
            if(a.buffer.readUInt8(resultLength-1) + b.buffer.readUInt8(resultLength-1) > 255) resultLength++;
        }

        resultLength = Math.ceil(resultLength / 8) * 8;

        const resultBuffer = Buffer.allocUnsafe(resultLength);

        if(typeof callback === 'undefined'){
            return new Promise((resolve) => {
                internal.addAsync(resultBuffer, a.buffer, b.buffer, () => {
                   resolve(new FastBigInt(resultBuffer));
                });
            });
        } else {
            internal.addAsync(resultBuffer, a.buffer, b.buffer, () => {
                callback(new FastBigInt(resultBuffer))
            });
        }

    }
}

module.exports = FastBigInt;