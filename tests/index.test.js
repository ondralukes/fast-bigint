const FastInt = require('../index');
const crypto = require('crypto');
const {toBigIntLE} = require('bigint-buffer');
const randomTest = require('./randomTest');

describe('constructor', function () {
    it('should construct from Number', function () {
        const x = new FastInt(2556);
        expect(x.getBuffer()).toStrictEqual(Buffer.from([0xfc, 0x09, 0, 0, 0, 0, 0, 0]));
    });
    it('should construct from Buffer', function () {
        const x = new FastInt(Buffer.from('deadbeef', 'hex'));
        expect(x.getBuffer()).toStrictEqual(Buffer.from('deadbeef00000000', 'hex'));
    });
    it('should throw an error when called with negative number', function () {
        expect(() => new FastInt(-4)).toThrowError();
    });
});

describe('add', function () {
    it('should add simple numbers', function () {
        const x = FastInt.add(
            new FastInt(5),
            new FastInt(4)
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('0900000000000000', 'hex'));
    });
    it('should add numbers bigger than 64 bits', function () {
        const x = FastInt.add(
            new FastInt(Buffer.from('111111111111111101', 'hex')),
            new FastInt(Buffer.from('aaaaaaaaaaaaaaaa02', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('bbbbbbbbbbbbbbbb0300000000000000', 'hex'));
    });
    it('should handle overflow', function () {
        const x = FastInt.add(
            new FastInt(Buffer.from('01000000000000ff', 'hex')),
            new FastInt(Buffer.from('0100000000000002', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('02000000000000010100000000000000', 'hex'));
    });
    it('should add 131072 byte number faster than JavaScript BigInt', function () {
        const bufa = crypto.randomBytes(131072);
        const bufb = crypto.randomBytes(131072);

        const fastIntA = new FastInt(bufa);
        const fastIntB = new FastInt(bufb);

        const bigIntA = toBigIntLE(bufa);
        const bigIntB = toBigIntLE(bufb);

        let prev;
        prev = process.hrtime();
        FastInt.add(fastIntA, fastIntB);
        const fastIntTime = process.hrtime(prev);
        const fastIntNanos = fastIntTime[0]*1000000000 + fastIntTime[1];

        prev = process.hrtime();
        const res = bigIntA + bigIntB;
        const bigIntTime = process.hrtime(prev);
        const bigIntNanos = bigIntTime[0]*1000000000 + bigIntTime[1];
        console.log(`${fastIntNanos} < ${bigIntNanos} (${bigIntNanos - fastIntNanos} ns faster)`);
        expect(fastIntNanos).toBeLessThan(bigIntNanos);
    });
    it('should add async using await', async function () {
        const x = await FastInt.addAsync(new FastInt(1), new FastInt(2));
        expect(x.getBuffer()).toStrictEqual(Buffer.from('0300000000000000', 'hex'));
    });
    it('should add async using callback', async function () {
        FastInt.addAsync(new FastInt(1), new FastInt(2), (x) => {
            expect(x.getBuffer()).toStrictEqual(Buffer.from('0300000000000000', 'hex'));
            done();
        });
    });
    it('should pass random test', function () {
        expect(randomTest.add()).toStrictEqual(true);
    });
})

describe('sub', function () {
    it('should subtract simple numbers', function () {
        const x = FastInt.sub(
            new FastInt(5),
            new FastInt(4)
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('0100000000000000', 'hex'));
    });
    it('should subtract numbers bigger than 64 bits', function () {
        const x = FastInt.sub(
            new FastInt(Buffer.from('111111111111111101', 'hex')),
            new FastInt(Buffer.from('101000000000000000', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('01011111111111110100000000000000', 'hex'));
    });
    it('should handle overflow', function () {
        const x = FastInt.sub(
            new FastInt(Buffer.from('0001', 'hex')),
            new FastInt(Buffer.from('0100', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('ff00000000000000', 'hex'));
    });
    it('should subtract 131072 byte number faster than JavaScript BigInt', function () {
        const bufa = crypto.randomBytes(131072);
        const bufb = crypto.randomBytes(131072);

        //Make sure a > b
        bufb.writeUInt8(0, 131071);

        const fastIntA = new FastInt(bufa);
        const fastIntB = new FastInt(bufb);

        const bigIntA = toBigIntLE(bufa);
        const bigIntB = toBigIntLE(bufb);

        let prev;
        prev = process.hrtime();
        FastInt.sub(fastIntA, fastIntB);
        const fastIntTime = process.hrtime(prev);
        const fastIntNanos = fastIntTime[0]*1000000000 + fastIntTime[1];

        prev = process.hrtime();
        const res = bigIntA - bigIntB;
        const bigIntTime = process.hrtime(prev);
        const bigIntNanos = bigIntTime[0]*1000000000 + bigIntTime[1];
        console.log(`${fastIntNanos} < ${bigIntNanos} (${bigIntNanos - fastIntNanos} ns faster)`);
        expect(fastIntNanos).toBeLessThan(bigIntNanos);
    });
    it('should subtract async using await', async function () {
        const x = await FastInt.subAsync(new FastInt(2), new FastInt(1));
        expect(x.getBuffer()).toStrictEqual(Buffer.from('0100000000000000', 'hex'));
    });
    it('should subtract async using callback', async function () {
        FastInt.subAsync(new FastInt(4), new FastInt(3), (x) => {
            expect(x.getBuffer()).toStrictEqual(Buffer.from('0100000000000000', 'hex'));
            done();
        });
    });
    it('should pass random test', function () {
        expect(randomTest.sub()).toStrictEqual(true);
    });
})