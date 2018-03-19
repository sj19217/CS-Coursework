let exec = require("child_process").exec;
let fs = require("fs");
let $ = require("jquery");
let prog_lines = [];

let animations = {
    start: function (data) {
        console.log("Starting");
        $("#commentary").append("Starting compiler<br />");
        $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
    },

    prep_include: function (fname, data) {
        console.log(`Preprocessor include file: ${fname}`);
        $("#commentary").append(`Included file: ${fname}<br />`);
        $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
    },

    prep_define: function (name, value, data) {
        console.log(`Preprocessor defined constant ${name} as ${value}`);
        $("#commentary").append(`Replaced instances of ${name} with ${value}<br />`);
        $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
    },

    prep_ifanalysis: function (desc) {
        console.log(`Completed analysis of if statements: ${desc}`);
        $("#commentary").append("Analysed which if statements apply where<br />");
    },

    prep_if: function (data) {
        console.log(`Replaced if statements`);
        $("#commentary").append("Removed areas that fail an if statement<br />");
        $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
    },

    prep_done: function (data) {
        console.log(`Finished preprocessing: ${data}`);
        $("#commentary").append("Finished preprocessing<br />");
        $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
    },

    tree: function (tree) {
        console.log(`Syntax tree in JSON form: ${tree}`);
        $("#commentary").append("Created syntax tree<br />");
    },

    found_global: function (name, type, initial) {
        console.log(`Found global variable: Name is ${name}, type ${type}, initial ${initial}`);
        $("#commentary").append(`Found global variable: Name is ${name}, type ${type}, value ${initial}<br />`);
        $("#globals-table").append("<tr>" +
            `<td>${name}</td>` +
            `<td>${type}</td>` +
            `<td>${initial}</td>` +
            "</tr>")
    },

    locals: function () {
        console.log("Found local variables");
    },

    start_codegen: function () {
        console.log("Started code generation");
    },

    gen_data: function () {
        console.log("Generating data section from globals");
    },

    gen_data_line: function (name, type, initial, asm) {
        console.log(`Data section line. Name is ${name}, type is ${type}, initial is ${initial}: ${asm}`);
    },

    fin_gen_data: function (asm) {
        console.log(`Finished generating data section: ${asm}`);
    },

    gen_text: function () {
        console.log("Generating text section");
    },

    gen_block: function (name) {
        console.log(`Generating code block called ${name}`);
    },

    gen_stmt: function (desc, asm) {
        console.log(`Generating statement: ${desc}`);
        console.log(`Code for statement: ${asm}`);
    },

    finish: function (asm) {
        console.log(`Finished. Final assembly code:\n${asm}`);
        throw "Finished";
    }
};

(function () {
    let process = exec(`python ../Compiler/compile.py -i ${decodeURIComponent(urlParam("fname")).replace("\\", "\\\\")}`);
    console.log("Running: "+ `python ../Compiler/compile.py -i ${decodeURIComponent(urlParam("fname")).replace("\\", "\\\\")}`);

    process.stderr.on("data", function (data) {
        console.log(`stderr: ${data}`);
    });

    process.on("close", function () {
        fs.readFile("cplout.txt", "utf8", function (err, data) {
            if (err) throw err;
            prog_lines = data.splitWithTail("\n");
            afterCompilerFinish();
        });
    });
})();

function afterCompilerFinish() {
    console.log("------------afterCompilerFinish()");
    for (let i = 0; i < prog_lines.length; i++) {
        let line = prog_lines[i];
        //console.log(prog_lines[i]);
        let instr = line.splitWithTail(" ", 1)[0].trim();
        let after = line.splitWithTail(" ", 1)[1].trim();
        //console.log(instr + " " + after);
        let json;
        if (after !== "" && after !== undefined) {
            try {
                json = JSON.parse(after);
            } catch (err) {
                console.log(`JSON processing failed: '${after}'`);
            }

        }

        switch (instr.toLowerCase()) {
            case "start":
                animations.start(json);
                break;
            case "prep_include":
                animations.prep_include(json[0], json[1]);
                break;
            case "prep_define":
                animations.prep_define(json[0], json[1], json[2]);
                break;
            case "prep_ifanalysis":
                animations.prep_ifanalysis(json);
                break;
            case "prep_if":
                animations.prep_if(json);
                break;
            case "prep_done":
                animations.prep_done(json);
                break;
            case "tree":
                animations.tree(json);
                break;
            case "found_global":
                animations.found_global(json[0], json[1], json[2]);
                break;
            case "locals":
                animations.locals();
                break;
            case "start_codegen":
                animations.start_codegen();
                break;
            case "gen_data":
                animations.gen_data();
                break;
            case "gen_data_line":
                animations.gen_data_line(json[0], json[1], json[2], json[3]);
                break;
            case "fin_gen_data":
                animations.fin_gen_data(json);
                break;
            case "gen_text":
                animations.gen_text();
                break;
            case "gen_block":
                animations.gen_block(json);
                break;
            case "gen_stmt":
                animations.gen_stmt(json[0], json[1]);
                break;
            case "finish":
                animations.finish(json);
                break;
            default:
                console.log(`Unknown instruction type: '${instr}'`);
        }
    }
}