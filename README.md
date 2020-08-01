# fast-bigint
Module for fast operations with long integers.

Arithmetic operations are written in C.
# Warning
This module is in early development state. It only supports addition and subtraction now.
# Usage
```javascript
const FastBigInt = require('fast-bigint');

// Create using number
const a = new FastBigInt(123);

// Create using little-endian buffer
const b = new FastBigInt(Buffer.from([0xff, 0xee, 0xff]));

// Add using 'add(a, b)'
const sum = FastBigInt.add(a, b);
// Subtract using 'sub(a, b)'
const sum = FastBigInt.sub(a, b);


// Set thread limit for async functions (default 8)
FastBigInt.setMaxThreads(8);

// Add or subtract async on new thread
const sum = await FastBigInt.addAsync(a, b);
const sum = await FastBigInt.subAsync(a, b);

FastBigInt.addAsync(a, b, (sum) => {
    // Callback
});
FastBigInt.subAsync(a, b, (sum) => {
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
Addition of two random numbers using `add` function. Time is in nanoseconds. Faster times are **bold**

|Type        |Length      |Min         |Max         |Avg           |
|------------|------------|------------|------------|--------------|
|FastBigInt  |2           |1064        |**28580**   |**380.21**    |
|JS BigInt   |2           |**326**     |500780      |612.053       |
|------------|------------|------------|------------|--------------|
|FastBigInt  |4           |1050        |**3435**    |281.686       |
|JS BigInt   |4           |**327**     |15974       |**110.901**   |
|------------|------------|------------|------------|--------------|
|FastBigInt  |8           |1039        |29361       |324.258       |
|JS BigInt   |8           |**324**     |**699**     |**87.19**     |
|------------|------------|------------|------------|--------------|
|FastBigInt  |16          |1032        |20732       |304.407       |
|JS BigInt   |16          |**322**     |**1541**    |**87.763**    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |32          |1026        |7003        |294.951       |
|JS BigInt   |32          |**332**     |**1605**    |**97.503**    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |64          |909         |23631       |301.334       |
|JS BigInt   |64          |**203**     |**2161**    |**66.259**    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |128         |910         |4330        |266.458       |
|JS BigInt   |128         |**215**     |**2168**    |**71.787**    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |256         |906         |3412        |270.454       |
|JS BigInt   |256         |**244**     |**3326**    |**90.081**    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |512         |971         |8521        |333.724       |
|JS BigInt   |512         |**305**     |**3227**    |**121.527**   |
|------------|------------|------------|------------|--------------|
|FastBigInt  |1024        |1067        |**4866**    |**372.909**   |
|JS BigInt   |1024        |**435**     |617968      |762.743       |
|------------|------------|------------|------------|--------------|
|FastBigInt  |2048        |1232        |14184       |467.21        |
|JS BigInt   |2048        |**704**     |**1164**    |**185.721**   |
|------------|------------|------------|------------|--------------|
|FastBigInt  |4096        |1667        |**18581**   |**738.071**   |
|JS BigInt   |4096        |**1203**    |271388      |844.663       |
|------------|------------|------------|------------|--------------|
|FastBigInt  |8192        |2395        |**8298**    |1005.479      |
|JS BigInt   |8192        |**2205**    |139521      |**714.548**   |
|------------|------------|------------|------------|--------------|
|FastBigInt  |16384       |**3710**    |**19628**   |1724.427      |
|JS BigInt   |16384       |4233        |167814      |**1408.857**  |
|------------|------------|------------|------------|--------------|
|FastBigInt  |32768       |**6420**    |**18066**   |**2256.463**  |
|JS BigInt   |32768       |8289        |152376      |2624.544      |
|------------|------------|------------|------------|--------------|
|FastBigInt  |65536       |**11889**   |**33275**   |**5272.836**  |
|JS BigInt   |65536       |16365       |197377      |5295.716      |
|------------|------------|------------|------------|--------------|
|FastBigInt  |131072      |**22981**   |**130437**  |**10610.895** |
|JS BigInt   |131072      |65506       |540209      |20436.101     |
|------------|------------|------------|------------|--------------|
|FastBigInt  |262144      |**43296**   |**163732**  |**20336.201** |
|JS BigInt   |262144      |126412      |1496305     |37907.847     |
|------------|------------|------------|------------|--------------|
|FastBigInt  |524288      |**88174**   |**304294**  |**48235.052** |
|JS BigInt   |524288      |255788      |4885054     |79489.011     |
|------------|------------|------------|------------|--------------|
|FastBigInt  |1048576     |**193236**  |**546910**  |**110781.476**|
|JS BigInt   |1048576     |516191      |8956821     |161210.026    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |2097152     |**845942**  |**1145478** |**225754.129**|
|JS BigInt   |2097152     |1056813     |16489930    |317485.391    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |4194304     |**1724773** |**2142189** |**453857.298**|
|JS BigInt   |4194304     |2213867     |2574409     |589264.077    |
|------------|------------|------------|------------|--------------|
|FastBigInt  |8388608     |**3463609** |**4012667** |**909698.699**|
|JS BigInt   |8388608     |4423535     |6702394     |1237161.781   |