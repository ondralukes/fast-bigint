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

    equals(b){
        return FastBigInt.compare(this, b) === 0;
    }
    isLessThan(b){
        return FastBigInt.compare(this, b) === -1;
    }
    isGreaterThan(b){
        return FastBigInt.compare(this, b) === 1;
    }
    isLessOrEqual(b){
        return FastBigInt.compare(this, b) !== 1;
    }
    isGreaterOrEqual(b){
        return FastBigInt.compare(this, b) !== -1;
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

    static mul(a, b){
        return new FastBigInt(
            'fromPtr',
            internal.mul(a.#ptr, b.#ptr)
        );
    }

    static compare(a, b){
        return internal.compare(a.#ptr, b.#ptr);
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

    static mulAsync(a, b, callback){
        if(typeof callback === 'undefined'){
            return new Promise((resolve) => {
                internal.mulAsync(a.#ptr, b.#ptr, (ptr) => {
                    resolve(new FastBigInt('fromPtr', ptr));
                });
            });
        } else {
            internal.mulAsync(a.#ptr, b.#ptr, (ptr) => {
                callback(new FastBigInt('fromPtr', ptr));
            });
        }
    }
}

module.exports = FastBigInt;