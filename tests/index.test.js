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
            new FastInt(Buffer.from('ffffffffffffffffffffffffffffffff', 'hex')),
            new FastInt(Buffer.from('0100000000000000ffffffffffffffff', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('0000000000000000ffffffffffffffff0100000000000000', 'hex'));
    });
    it('should handle overflow', function () {
        const x = FastInt.add(
            new FastInt(Buffer.from('01000000000000ff', 'hex')),
            new FastInt(Buffer.from('0100000000000002', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('02000000000000010100000000000000', 'hex'));
    });
    it('should add 98304 byte number faster than JavaScript BigInt', function () {
        const bufa = crypto.randomBytes(98304);
        const bufb = crypto.randomBytes(98304);

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
    it('should add async using callback', done => {
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
            new FastInt(Buffer.from('0000000000000000ffffffffffffffff01', 'hex')),
            new FastInt(Buffer.from('0100000000000000ffffffffffffffff', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('ffffffffffffffffffffffffffffffff', 'hex'));
    });
    it('should handle overflow', function () {
        const x = FastInt.sub(
            new FastInt(Buffer.from('0001', 'hex')),
            new FastInt(Buffer.from('0100', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('ff00000000000000', 'hex'));
    });
    it('should subtract 98304 byte number faster than JavaScript BigInt', function () {
        const bufa = crypto.randomBytes(98304);
        const bufb = crypto.randomBytes(98304);

        //Make sure a > b
        bufb.writeUInt8(0, 98303);

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
    it('should subtract async using callback', done => {
        FastInt.subAsync(new FastInt(4), new FastInt(3), (x) => {
            expect(x.getBuffer()).toStrictEqual(Buffer.from('0100000000000000', 'hex'));
            done();
        });
    });
    it('should pass random test', function () {
        expect(randomTest.sub()).toStrictEqual(true);
    });
})

describe('mul', function () {
    it('should multiply simple numbers', function () {
        const x = FastInt.mul(
            new FastInt(5),
            new FastInt(4)
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('1400000000000000', 'hex'));
    });
    it('should multiply numbers bigger than 64 bits', function () {
        const x = FastInt.mul(
            new FastInt(Buffer.from('cdab080704030201efbeadde', 'hex')),
            new FastInt(Buffer.from('efef080704030201cefacada', 'hex'))
        );
        expect(x.getBuffer()).toStrictEqual(Buffer.from('63c7e4a27f1ec360101998f0609eac8c30fb970cd98350be', 'hex'));
    });
    it('should multiply 12288 byte number faster than JavaScript BigInt', function () {
        const bufa = crypto.randomBytes(12288);
        const bufb = crypto.randomBytes(12288);

        const fastIntA = new FastInt(bufa);
        const fastIntB = new FastInt(bufb);

        const bigIntA = toBigIntLE(bufa);
        const bigIntB = toBigIntLE(bufb);

        let prev;
        prev = process.hrtime();
        FastInt.mul(fastIntA, fastIntB);
        const fastIntTime = process.hrtime(prev);
        const fastIntNanos = fastIntTime[0]*1000000000 + fastIntTime[1];

        prev = process.hrtime();
        const res = bigIntA * bigIntB;
        const bigIntTime = process.hrtime(prev);
        const bigIntNanos = bigIntTime[0]*1000000000 + bigIntTime[1];
        console.log(`${fastIntNanos} < ${bigIntNanos} (${bigIntNanos - fastIntNanos} ns faster)`);
        expect(fastIntNanos).toBeLessThan(bigIntNanos);
    });
    it('should multiply async using await', async function () {
        const x = await FastInt.mulAsync(
            new FastInt(Buffer.from('cdab080704030201efbeadde', 'hex')),
            new FastInt(Buffer.from('efef080704030201cefacada', 'hex'))
        );
        expect(x.getBuffer().toString('hex')).toStrictEqual('63c7e4a27f1ec360101998f0609eac8c30fb970cd98350be');
    });
    it('should multiply async using callback', done => {
        FastInt.mulAsync(
            new FastInt(Buffer.from('cdab080704030201efbeadde', 'hex')),
            new FastInt(Buffer.from('efef080704030201cefacada', 'hex')),
            (x) => {
                expect(x.getBuffer().toString('hex')).toStrictEqual('63c7e4a27f1ec360101998f0609eac8c30fb970cd98350be');
                done();
            });
    });
    it('should pass random test', function () {
        expect(randomTest.mul()).toStrictEqual(true);
    });
})

describe('div', function () {
    it('should divide simple numbers', function () {
        const x = FastInt.div(
            new FastInt(645434434),
            new FastInt(8675)
        );

        expect(x.getBuffer()).toStrictEqual(Buffer.from('a122010000000000', 'hex'));
    });
    it('should divide numbers bigger than 64 bits', function () {
        const a = new FastInt(Buffer.from('460bc87985c567813f8a691ec2fdefc5f81564c4d38c2ef22f0930f4afd4ab77', 'hex'));
        const b = new FastInt(Buffer.from('6bcb1d5fb56b25d3c9176b7a08728d02d2d1aa0249538e4fa8640568a8b6', 'hex'));
        const x = FastInt.div(
           a, b
        );

        expect(x.getBuffer()).toStrictEqual(Buffer.from('b8a7000000000000', 'hex'));
    });
    it('should pass random test', function () {
        expect(randomTest.div()).toStrictEqual(true);
    });
})

describe('compare', function () {
    it('should compare simple numbers', function () {
        expect(
            FastInt.compare(
                new FastInt(5),
                new FastInt(4)
            )
        ).toStrictEqual(1);
        expect(
            FastInt.compare(
                new FastInt(5),
                new FastInt(40)
            )
        ).toStrictEqual(-1);
        expect(
            FastInt.compare(
                new FastInt(40),
                new FastInt(40)
            )
        ).toStrictEqual(0);
    });
    it('should compare numbers bigger than 64 bits', function () {
        expect(
            FastInt.compare(
                new FastInt(Buffer.from('0000000000000000ff', 'hex')),
                new FastInt(Buffer.from('0500000000000000ef', 'hex'))
            )
        ).toStrictEqual(1);
        expect(
            FastInt.compare(
                new FastInt(Buffer.from('0000000000000000ee', 'hex')),
                new FastInt(Buffer.from('0500000000000000ef', 'hex'))
            )
        ).toStrictEqual(-1);
        expect(
            FastInt.compare(
                new FastInt(Buffer.from('0500000400000000ef', 'hex')),
                new FastInt(Buffer.from('0500000400000000ef', 'hex'))
            )
        ).toStrictEqual(0);
    });
});

describe('compare functions', function () {
    const x = new FastInt(10);
    const e = new FastInt(10);
    const l = new FastInt(9);
    const g = new FastInt(11);

    it('.equals()', function () {
        expect(x.equals(g)).toStrictEqual(false);
        expect(x.equals(e)).toStrictEqual(true);
        expect(x.equals(l)).toStrictEqual(false);
    });

    it('.isLessThan()', function () {
        expect(x.isLessThan(g)).toStrictEqual(true);
        expect(x.isLessThan(e)).toStrictEqual(false);
        expect(x.isLessThan(l)).toStrictEqual(false);
    });

    it('.isGreaterThan()', function () {
        expect(x.isGreaterThan(g)).toStrictEqual(false);
        expect(x.isGreaterThan(e)).toStrictEqual(false);
        expect(x.isGreaterThan(l)).toStrictEqual(true);
    });

    it('.isLessOrEqual()', function () {
        expect(x.isLessOrEqual(g)).toStrictEqual(true);
        expect(x.isLessOrEqual(e)).toStrictEqual(true);
        expect(x.isLessOrEqual(l)).toStrictEqual(false);
    });

    it('.isGreaterOrEqual()', function () {
        expect(x.isGreaterOrEqual(g)).toStrictEqual(false);
        expect(x.isGreaterOrEqual(e)).toStrictEqual(true);
        expect(x.isGreaterOrEqual(l)).toStrictEqual(true);
    });
});