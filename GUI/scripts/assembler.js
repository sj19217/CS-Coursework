let exec = require("child_process").exec;
let fs = require("fs");
let prog_lines = [];

(function () {

    console.log(urlParam("fname"));
    let process = exec(`python ../Assembler/assembler_interactive.py ${decodeURIComponent(urlParam("fname"))}`);
    fs.readFile("asmout.txt", "utf8", function (err, data) {
        if (err) throw err;
        prog_lines = data.split("\n");
    });

    process.on("close", afterAssemblerFinish);
})();

function afterAssemblerFinish() {
    for (let i = 0; i < prog_lines.length; i++) {
        let line = prog_lines[i];
        let instr = line.split(" ", 1)[0];
    }
}