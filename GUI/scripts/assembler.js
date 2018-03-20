let exec = require("child_process").exec;
let fs = require("fs");
let $ = require("jquery");
let prog_lines = [];
let animation_queue = [];

let animations;
animations = {
    start_text: function (text) {
        return function () {
            $("#commentary").append("This is the initial assembly<br />");
            $("#code").html(text[0].replace(/\n/g, "<br />"));
        };
    },

    remove_comments: function (text) {
        return function () {
            $("#commentary").append("Removing comments<br />");
            $("#code").html(text[0].replace(/\n/g, "<br />"));
        };
    },

    remove_empty_lines: function (text) {
        return function () {
            $("#commentary").append("Removing empty lines<br />");
            $("#code").html(text[0].replace(/\n/g, "<br />"));
        };
    },
    remove_dup_wspace: function (text) {
        return function () {
            $("#commentary").append("Removing any duplicate whitespace<br />");
            $("#code").html(text[0].replace(/\n/g, "<br />"))
        };
    },

    split: function (json) {
        return function () {
            // Given JSON is in the form of [meta, data, text]
            $("#commentary").append("Split into separate sections<br />");
            $("#code").html("<div id='section-meta' class='asm-section'></div>" +
                "<div id='section-data' class='asm-section'></div>" +
                "<div id='section-text' class='asm-section'></div>");
            $("#section-meta").html(`<b>Meta</b><br />${json[0].replace(/\n/g, "<br />")}`);
            $("#section-data").html(`<b>Data</b><br />${json[1].replace(/\n/g, "<br />")}`);
            $("#section-text").html(`<b>Text</b><br />${json[2].replace(/\n/g, "<br />")}`);
        };
    },

    start_proc_meta: function () {
        return function () {
            $("#commentary").append("Beginning to process meta section<br />");
        };
    },

    read_meta_line: function (line) {
        return function () {
            $("#commentary").append(`Read meta section line: ${line[0]}<br />`);
        };
    },

    ustd_meta_line: function (desc, name, value) {
        return function () {
            $("#commentary").append(`Understood meta section line: ${desc}<br />`);
            $("#metadata-table").append("<tr>" +
                `<td>${name}</td>` +
                `<td>${value}</td>` +
                "</tr>");
        };
    },

    start_proc_data: function () {
        return function () {
            $("#commentary").append("Beginning to process data section<br />");
        };
    },

    read_data_line: function (line) {
        return function () {
            $("#commentary").append(`Read data section line: ${line[0]}<br />`);
        };
    },

    ustd_data_line: function (desc, name, dtype, value) {
        return function () {
            $("#commentary").append(`Understood data section line: ${desc}<br />`);
            $("#global-table").append(`<tr>` +
                `<td>${name}</td>` +
                `<td>${dtype}</td>` +
                `<td>${value}</td>` +
                `</tr>`);
        };
    },

    start_proc_text: function () {
        return function () {
            $("#commentary").append("Beggining to process text section<br />");
        };
    },

    read_text_line: function (line) {
        return function () {
            $("#commentary").append(`Read text section line: ${line}<br />`);
        };
    },

    ustd_text_line: function (desc, label, mnemonic, dtype, op1, op2) {
        return function () {
            $("#commentary").append(`Understood text section line: ${desc}<br />`);
            $("#text-table").append("<tr>" +
                `<td>${label}</td>` +
                `<td>${mnemonic}</td>` +
                `<td>${dtype}</td>` +
                `<td>${op1}</td>` +
                `<td>${op2}</td>` +
                "</tr>")
        };
    },

    start_lv_detect: function () {
        return function () {
            $("#commentary").append("Starting to detect labels and variables");
        };
    },

    found_var: function (name, mrel, type) {
        return function () {
            $("#commentary").append(`Found variable ${name} at location ${mrel}, with type ${type}<br />`);
        };
    },

    found_label: function (lname, instrnum) {
        return function () {
            $("#commentary").append(`Found label ${lname} pointing to instruction ${instrnum}<br />`);
        };
    },

    mem_offsets: function (dict) {
        return function () {
            let c = $("#commentary");
            let border = "style='border: 1px solid black;'";
            c.append("Memory table:<br />").append(`<table ${border}>`);
            for (let key in dict) {
                if (!dict.hasOwnProperty(key)) continue;
                c.append(`<tr><td ${border}>${key}</td><td ${border}>${dict[key]}</td></tr>`);
            }
            c.append("</table><br />");
        };
    },

    place_addrs: function (instrs) {
        return function () {
            $("#commentary").append("Placing memory addresses<br />");
            $("#text-holder").html("<table id=\"text-table\">\n" +
                "<tr>\n" +
                "<td style=\"font-weight: bold;\">Label</td>\n" +
                "<td style=\"font-weight: bold;\">Mnemonic</td>\n" +
                "<td style=\"font-weight: bold;\">Data type</td>\n" +
                "<td style=\"font-weight: bold;\">Operand 1</td>\n" +
                "<td style=\"font-weight: bold;\">Operand 2</td>\n" +
                "</tr>\n" +
                "</table>");
            for (let i = 0; i < instrs.length; i++) {
                $("#text-table").append("<tr>" +
                    `<td>${instrs[i][0]}</td>` +
                    `<td>${instrs[i][1]}</td>` +
                    `<td>${instrs[i][2]}</td>` +
                    `<td>${instrs[i][3]}</td>` +
                    `<td>${instrs[i][4]}</td></tr>`);
            }
        };
    },

    conv_meta: function (bytes) {
        return function () {
            $("#commentary").append("Converting metadata to bytes<br />");
            let instr_box = $("#instr_list");
            for (let i = 0; i < bytes.length; i++) {
                instr_box.append(`${bytes[i]} `);
            }
        };
    },

    conv_instr: function (opcode, opbyte, operand1, operand2) {
        return function () {
            $("#commentary").append("Converting instruction to bytes<br />");
            let instr_list = $("instr_list");
            instr_list.append(`${opcode} ${opbyte} `);
            for (let i = 0; i < operand1.length; i++) {
                instr_list.append(`${operand1[i]} `);
            }
            for (let i = 0; i < operand2.length; i++) {
                instr_list.append(`${operand2[i]} `);
            }
        };
    },

    end: function (final) {
        return function () {
            $("#commentary").append("Commentary finished<br />");
        };
    }
};

(function () {

    console.log(decodeURIComponent(urlParam("fname")));
    let process = exec(`python ../Assembler/assembler_interactive.py ${decodeURIComponent(urlParam("fname"))}`);

    process.stderr.on("data", function (data) {
        console.log(`stderr: ${data}`);
    });

    process.on("close", function () {
        fs.readFile("asmout.txt", "utf8", function (err, data) {
            if (err) throw err;
            prog_lines = data.splitWithTail("\n");
            afterAssemblerFinish();
        });
    });
})();

function afterAssemblerFinish() {

    for (let i = 0; i < prog_lines.length; i++) {
        console.log(prog_lines[i]);
        let line = prog_lines[i];
        let instr = line.splitWithTail(" ", 1)[0];

        let parts;

        switch (instr.toLowerCase()) {
            case "start_text":
                console.log("Adding start_text");
                // parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                // animations.start_text(parts[0]);
                animation_queue.push(animations.start_text(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "remove_comments":
                animation_queue.push(animations.remove_comments(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "remove_empty_lines":
                animation_queue.push(animations.remove_empty_lines(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "remove_dup_wspace":
                animation_queue.push(animations.remove_dup_wspace(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "split":
                animation_queue.push(animations.split(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "start_proc_meta":
                animation_queue.push(animations.start_proc_meta());
                break;
            case "read_meta_line":
                animation_queue.push(animations.read_meta_line(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "ustd_meta_line":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.ustd_meta_line(parts[0], parts[1], parts[2]));
                break;
            case "start_proc_data":
                animation_queue.push(animations.start_proc_data());
                break;
            case "read_data_line":
                animation_queue.push(animations.read_data_line(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "ustd_data_line":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.ustd_data_line(parts[0], parts[1], parts[2], parts[3]));
                break;
            case "start_proc_text":
                animation_queue.push(animations.start_proc_text());
                break;
            case "read_text_line":
                animation_queue.push(animations.read_text_line(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "ustd_text_line":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.ustd_text_line(parts[0], parts[1], parts[2],
                                                               parts[3], parts[4], parts[5]));
                break;
            case "start_lv_detect":
                animation_queue.push(animations.start_lv_detect());
                break;
            case "found_var":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.found_var(parts[0], parts[1], parts[2]));
                break;
            case "found_label":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.found_label(parts[0], parts[1]));
                break;
            case "mem_offsets":
                animation_queue.push(animations.mem_offsets(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "place_addrs":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.place_addrs(parts));
                break;
            case "conv_meta":
                animation_queue.push(animations.conv_meta(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
            case "conv_instr":
                parts = JSON.parse(line.splitWithTail(" ", 1)[1]);
                animation_queue.push(animations.conv_instr(parts[0], parts[1], parts[2], parts[3]));
                break;
            case "end":
                animation_queue.push(animations.end(JSON.parse(line.splitWithTail(" ", 1)[1])));
                break;
        }
    }
}

function animateAssemblerStep()
{
    let action = animation_queue.shift();
    action();
}