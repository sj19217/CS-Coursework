let exec = require("child_process").exec;
let fs = require("fs");
let prog_lines = [];

let animations = {
    start_text: function (text) {

    },
    remove_comments: function (text) {

    },
    remove_empty_lines: function (text) {

    },
    remove_dup_wspace: function (text) {

    },

    splitWithTail: function (json) {

    },

    start_proc_meta: function () {

    },

    read_meta_line: function (line) {

    },

    ustd_meta_line: function (desc) {

    },

    start_proc_data: function () {

    },

    read_data_line: function (line) {

    },

    ustd_data_line: function (desc) {

    },

    start_proc_text: function () {

    },

    read_text_line: function (line) {

    },

    ustd_text_line: function (desc) {

    },

    start_lv_detect: function () {

    },

    found_var: function (name, mrel, type) {

    },

    found_label: function (lname, instrnum) {

    },

    mem_offsets: function (dict) {

    },

    place_addrs: function () {

    },

    conv_meta: function (bytes) {

    },

    conv_instr: function (opcode, opbyte, operand1, operand2) {

    },

    end: function (final) {

    }
};

(function () {

    console.log(decodeURIComponent(urlParam("fname")));
    let process = exec(`python ../Assembler/assembler_interactive.py ${decodeURIComponent(urlParam("fname"))}`);
    fs.readFile("asmout.txt", "utf8", function (err, data) {
        if (err) throw err;
        prog_lines = data.splitWithTail("\n");
    });

    process.on("close", afterAssemblerFinish);
})();

function afterAssemblerFinish() {
    for (let i = 0; i < prog_lines.length; i++) {
        console.log(prog_lines[i]);
        let line = prog_lines[i];
        console.log("ABC: " + line.splitWithTail(" ", 1));
        let instr = line.splitWithTail(" ", 1)[0];

        let parts;

        switch (instr.toLowerCase()) {
            case "start_text":
                console.log("start_text: " + line.splitWithTail(" ", 1)[1]);
                animations.start_text(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "remove_comments":
                console.log("remove_comments: " + line.splitWithTail(" ", 1)[1]);
                animations.remove_comments(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "remove_empty_lines":
                console.log("remove_empty_lines: " + line.splitWithTail(" ", 1)[1]);
                animations.remove_empty_lines(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "remove_dup_wspace":
                console.log("remove_dup_wspace: " + line.splitWithTail(" ", 1)[1]);
                animations.remove_dup_wspace(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "splitWithTail":
                console.log("splitWithTail: " + line.splitWithTail(" ", 1)[1]);
                animations.splitWithTail(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "start_proc_meta":
                console.log("start_proc_meta");
                animations.start_proc_meta();
                break;
            case "read_meta_line":
                console.log("read_meta_line: " + line.splitWithTail(" ", 1)[1]);
                animations.read_meta_line(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "ustd_meta_line":
                console.log("ustd_meta_line: " + line.splitWithTail(" ", 1)[1]);
                animations.ustd_meta_line(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "start_proc_data":
                console.log("start_proc_data: " + line.splitWithTail(" ", 1)[1]);
                animations.start_proc_data();
                break;
            case "read_data_line":
                console.log("read_data_line: " + line.splitWithTail(" ", 1)[1]);
                animations.read_data_line(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "ustd_data_line":
                console.log("ustd_data_line: " + line.splitWithTail(" ", 1)[1]);
                animations.ustd_data_line(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "start_proc_text":
                console.log("start_proc_text");
                animations.start_proc_text();
                break;
            case "read_text_line":
                console.log("read_text_line: " + line.splitWithTail(" ", 1)[1]);
                animations.read_text_line(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "ustd_text_line":
                console.log("ustd_text_line: " + line.splitWithTail(" ", 1)[1]);
                animations.ustd_text_line(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "start_lv_detect":
                console.log("start_lv_detect");
                animations.start_lv_detect();
                break;
            case "found_var":
                parts = line.splitWithTail(" ");
                console.log("found_var: " + parts.toString());
                animations.found_var(parts[1], parts[2], parts[3]);
                break;
            case "found_label":
                parts = line.splitWithTail(" ");
                console.log("found_label: " + parts.toString());
                animations.found_label(parts[1], parts[2]);
                break;
            case "mem_offsets":
                console.log("mem_offsets: " + line.splitWithTail(" ", 1)[1]);
                animations.mem_offsets(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "place_addrs":
                console.log("place_addrs");
                animations.place_addrs();
                break;
            case "conv_meta":
                console.log("conv_meta");
                animations.conv_meta(JSON.parse(line.splitWithTail(" ", 1)[1]));
                break;
            case "conv_instr":
                parts = line.splitWithTail(" ");
                console.log("conv_instr: " + parts.slice(1).toString());
                animations.conv_instr(JSON.parse(parts[1]),
                    JSON.parse(parts[2]),
                    JSON.parse(parts[3]),
                    JSON.parse(parts[4]));
                break;
            case "end":
                console.log("end: " + line.splitWithTail(" ", 1)[1]);
                animations.end(JSON.parse(line.splitWithTail(" ", 1)[1]));
        }
    }
}