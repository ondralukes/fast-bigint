const FastInt = require('../index');
const crypto = require('crypto');
const {toBigIntLE, toBufferLE} = require('bigint-buffer');

module.exports.add = function () {
    for(let len = 1;len < 524288; len*=2){
        for(let i =0;i<256;i++){
            if(!addTest(len)){
                console.log(`add() returned invalid value. length ${len} run ${i}`);
                return false;
            }
        }
    }
    return true;
}

module.exports.sub = function () {
    for(let len = 1;len < 524288; len*=2){
        for(let i =0;i<256;i++){
            if(!subTest(len)){
                console.log(`sub() returned invalid value. length ${len} run ${i}`);
                return false;
            }
        }
    }
    return true;
}

module.exports.mul = function () {
    for(let len = 1;len < 8096; len*=2){
        for(let i =0;i<64;i++){
            if(!mulTest(len)){
                console.log(`mul() returned invalid value. length ${len} run ${i}`);
                return false;
            }
        }
    }
    return true;
}

module.exports.div = function () {
    for(let len = 1;len < 256; len*=2){
        for(let i =0;i< 32;i++){
            if(!divTest(len)){
                console.log(`div() returned invalid value. length ${len} run ${i}`);
                return false;
            }
        }
    }
    return true;
}

function addTest(len) {
    const bufferA = crypto.randomBytes(len);
    const bufferB = crypto.randomBytes(len);

    const bigIntA = toBigIntLE(bufferA);
    const bigIntB = toBigIntLE(bufferB);

    const fastIntA = new FastInt(bufferA);
    const fastIntB = new FastInt(bufferB);

    const fastIntResBuffer = FastInt.add(fastIntA, fastIntB).getBuffer();

    const bigIntResBuffer = toBufferLE(bigIntA + bigIntB, fastIntResBuffer.byteLength);


    const cmp = Buffer.compare(fastIntResBuffer, bigIntResBuffer);
    return cmp === 0;
}


function subTest(len) {
    const bufferA = crypto.randomBytes(len);
    const bufferB = crypto.randomBytes(len);

    let bufferAlast = bufferA.readUInt8(len-1);
    let bufferBlast = bufferB.readUInt8(len-1);

    if(bufferBlast > bufferAlast){
        const tmp = bufferAlast;
        bufferAlast = bufferBlast;
        bufferBlast = tmp;
    } else if(bufferBlast === bufferAlast) {
        bufferBlast = 0x00;
        bufferAlast = 0xff;
    }

    bufferA.writeUInt8(bufferAlast, len-1);
    bufferB.writeUInt8(bufferBlast, len-1);

    const bigIntA = toBigIntLE(bufferA);
    const bigIntB = toBigIntLE(bufferB);

    const fastIntA = new FastInt(bufferA);
    const fastIntB = new FastInt(bufferB);

    const fastIntResBuffer = FastInt.sub(fastIntA, fastIntB).getBuffer();

    const bigIntResBuffer = toBufferLE(bigIntA - bigIntB, fastIntResBuffer.byteLength);


    const cmp = Buffer.compare(fastIntResBuffer, bigIntResBuffer);
    return cmp === 0;
}

function mulTest(len){
    const bufferA = crypto.randomBytes(len);
    const bufferB = crypto.randomBytes(len);

    const bigIntA = toBigIntLE(bufferA);
    const bigIntB = toBigIntLE(bufferB);

    const fastIntA = new FastInt(bufferA);
    const fastIntB = new FastInt(bufferB);

    const fastIntResBuffer = FastInt.mul(fastIntA, fastIntB).getBuffer();

    const bigIntResBuffer = toBufferLE(bigIntA * bigIntB, fastIntResBuffer.byteLength);


    const cmp = Buffer.compare(fastIntResBuffer, bigIntResBuffer);
    return cmp === 0;
}

function divTest(len){
    const bufferA = crypto.randomBytes(len);
    const blen = Math.ceil(Math.random() * len) + 1;
    const bufferB = crypto.randomBytes(blen);

    if(blen === 1 && bufferB.readUInt8(0) === 0) bufferB.writeUInt8(1);
    if(bufferB.readUInt8(blen-1) === 0) bufferB.writeUInt8(1,blen-1);

    const bigIntA = toBigIntLE(bufferA);
    const bigIntB = toBigIntLE(bufferB);

    const fastIntA = new FastInt(bufferA);
    const fastIntB = new FastInt(bufferB);

    const fastIntResBuffer = FastInt.div(fastIntA, fastIntB).getBuffer();

    const bigIntResBuffer = toBufferLE(bigIntA / bigIntB, fastIntResBuffer.byteLength);


    const cmp = Buffer.compare(fastIntResBuffer, bigIntResBuffer);

    return cmp === 0;
}