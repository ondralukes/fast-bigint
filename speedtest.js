const FastInt = require('./index');
const crypto = require('crypto');
const chalk = require('chalk');
const {toBigIntLE} = require('bigint-buffer');

const spacer = '-'.repeat(12);
const delimiter = ('|' + spacer).repeat(4) + '|' + '-'.repeat(14) +  '|\n';

process.stdout.write('|Type'.padEnd(13) + '|');
process.stdout.write('Length'.padEnd(12) + '|');
process.stdout.write('Min'.padEnd(12) + '|');
process.stdout.write('Max'.padEnd(12) + '|');
process.stdout.write('Avg'.padEnd(14) + '|\n');
for(let i = 2; i < 4096*4096;i *= 2){
    runTest(i);
}


function runTest(length) {
    process.stdout.write(delimiter);
    const f = runFastIntTest(length);
    const b = runBigIntTest(length);

    process.stdout.write('|' + 'FastBigInt'.padEnd(12));
    process.stdout.write('|' + length.toString().padEnd(12));
    if(f.min < b.min)
        process.stdout.write('|' + chalk.green(('**' + f.min.toString() +  '**').padEnd(12)));
    else
        process.stdout.write('|' + f.min.toString().padEnd(12));
    if(f.max < b.max)
        process.stdout.write('|' + chalk.green(('**' + f.max.toString() +  '**').padEnd(12)));
    else
        process.stdout.write('|' + f.max.toString().padEnd(12));
    if(f.avg < b.avg)
        process.stdout.write('|' + chalk.green(('**' + f.avg.toString() +  '**').padEnd(14)));
    else
        process.stdout.write('|' + f.avg.toString().padEnd(14));
    process.stdout.write('|\n');

    process.stdout.write('|' + 'JS BigInt'.padEnd(12));
    process.stdout.write('|' + length.toString().padEnd(12));
    if(b.min < f.min)
        process.stdout.write('|' + chalk.red(('**' + b.min.toString() +  '**').padEnd(12)));
    else
        process.stdout.write('|' + b.min.toString().padEnd(12));
    if(b.max < f.max)
        process.stdout.write('|' + chalk.red(('**' + b.max.toString() +  '**').padEnd(12)));
    else
        process.stdout.write('|' + b.max.toString().padEnd(12));
    if(b.avg < f.avg)
        process.stdout.write('|' + chalk.red(('**' + b.avg.toString() +  '**').padEnd(14)));
    else
        process.stdout.write('|' + b.avg.toString().padEnd(14));
    process.stdout.write('|\n');
}

function runFastIntTest(len) {
    const bufa = crypto.randomBytes(len);
    const bufb = crypto.randomBytes(len);

    const a = new FastInt(bufa);
    const b = new FastInt(bufb);

    let min = 10000000000;
    let max = 0;
    let avg = 0;
    let res;
    for(let i = 0;i<256;i++){
        let prev = process.hrtime();
        res = FastInt.add(a, b);
        const t = parseHrtime(process.hrtime(prev));
        if(t > max) max = t;
        if(t < min) min = t;
        avg += t;
    }
    avg /= 1000;
    return {
        max: max,
        min: min,
        avg: avg
    };
}

function runBigIntTest(len) {
    const bufa = crypto.randomBytes(len);
    const bufb = crypto.randomBytes(len);

    const a = toBigIntLE(bufa, len);
    const b = toBigIntLE(bufb, len);

    let min = 10000000000;
    let max = 0;
    let avg = 0;
    let res;
    for(let i = 0;i<256;i++){
        let prev = process.hrtime();
        res = a + b;
        const t = parseHrtime(process.hrtime(prev));
        if(t > max) max = t;
        if(t < min) min = t;
        avg += t;
    }
    avg /= 1000;
    return {
        max: max,
        min: min,
        avg: avg,
        res: res
    };
}

function parseHrtime(a){
    return a[0]*1000000000 + a[1];
}