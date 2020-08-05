# fast-bigint
Module for fast operations with long integers.

Arithmetic operations are written in C.
# Warning
This module is in early development state. It only supports addition, subtraction and multiplication.
# Usage
```javascript
const FastBigInt = require('fast-bigint');

// Create using number
const a = new FastBigInt(123);

// Create using little-endian buffer
const b = new FastBigInt(Buffer.from([0xff, 0xee, 0xff]));

// Add using 'add(a, b)'
const x = FastBigInt.add(a, b);
// Subtract using 'sub(a, b)'
const x = FastBigInt.sub(a, b);
// Multiply using 'mul(a, b)'
const x = FastBigInt.mul(a, b);


// Set thread limit for async functions (default 8)
FastBigInt.setMaxThreads(8);

// Run async on new thread
const x = await FastBigInt.addAsync(a, b);
const x = await FastBigInt.subAsync(a, b);
const x = await FastBigInt.mulAsync(a, b);

FastBigInt.addAsync(a, b, (x) => {
    // Callback
});
FastBigInt.subAsync(a, b, (x) => {
    // Callback
});
FastBigInt.mulAsync(a, b, (x) => {
    // Callback
});

//Get little-endian buffer
//Buffer size is padded to multiple of 8
const x = new FastBigInt(8);
x.getBuffer() // = <Buffer 08 00 00 00 00 00 00 00>

//Compare using compare(a, b)
//returns
// -1 if a < b
// 0 if a = b
// 1 if a > b
FastBigInt.compare(
    new FastBigInt(10),
    new FastBigInt(15)
); // = -1

//Compare using compare functions
new FastBigInt(5).isGreaterOrEqual(new FastBigInt(4)); // = true
```

### Warning
This module only supports positive numbers, so `sub(a, b)` when `a < b` will return invalid values
# Performance
It's currently faster than JavaScript BigInt
* When adding or subtracting numbers larger than ~98304 bytes
* When multiplying numbers larger than ~12288 bytes

You can perform calculations on multiple threads with async functions (unlike JavaScript, which is single-threaded)