const internal = require('./internal/build/Release/internal');

class FastBigInt{
    #ptr;

    constructor(bufferOrNumber, ptr){
        if(Buffer.isBuffer(bufferOrNumber)){
            this.#ptr = internal.create(this, bufferOrNumber);
        } else if(typeof bufferOrNumber === 'string'){
            this.#ptr = ptr;
            internal.fromPtr(this, ptr);
        } else {
            if(typeof bufferOrNumber === 'number') {
                this.#ptr = internal.create(this);
                this.setNumber(bufferOrNumber);
            } else {
                this.#ptr = internal.create(this);
            }
        }
    }

    static setMaxThreads(count){
        if(typeof count !== 'number')
            throw Error('Thread count must be a number.');

        internal.setMaxThreads(count);
    }

    getBuffer(){
        return internal.getBuffer(this.#ptr);
    }

    setNumber(number) {
        if (number < 0)
            throw Error('Negative numbers are not supported.');

        internal.setUInt64(this.#ptr, number);
    }

    static add(a, b){
        return new FastBigInt(
            'fromPtr',
            internal.add(a.#ptr, b.#ptr)
        );
    }

    static sub(a, b){
        return new FastBigInt(
            'fromPtr',
            internal.sub(a.#ptr, b.#ptr)
        );
    }

    static addAsync(a, b, callback){
        if(typeof callback === 'undefined'){
            return new Promise((resolve) => {
                internal.addAsync(a.#ptr, b.#ptr, (ptr) => {
                   resolve(new FastBigInt('fromPtr', ptr));
                });
            });
        } else {
            internal.addAsync(a.#ptr, b.#ptr, (ptr) => {
                callback(new FastBigInt('fromPtr', ptr));
            });
        }
    }

    static subAsync(a, b, callback){
        if(typeof callback === 'undefined'){
            return new Promise((resolve) => {
                internal.subAsync(a.#ptr, b.#ptr, (ptr) => {
                    resolve(new FastBigInt('fromPtr', ptr));
                });
            });
        } else {
            internal.subAsync(a.#ptr, b.#ptr, (ptr) => {
                callback(new FastBigInt('fromPtr', ptr));
            });
        }
    }
}

module.exports = FastBigInt;