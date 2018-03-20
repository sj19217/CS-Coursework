let exec = require("child_process").exec;
let fs = require("fs");
let $ = require("jquery");
let prog_lines = [];
let animation_queue = [];

let animations = {
    start: function (data) {
        return function () {
            console.log("Starting");
            $("#commentary").append("Starting compiler<br />");
            $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
        };
    },

    prep_include: function (fname, data) {
        return function () {
            console.log(`Preprocessor include file: ${fname}`);
            $("#commentary").append(`Included file: ${fname}<br />`);
            $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
        };
    },

    prep_define: function (name, value, data) {
        return function () {
            console.log(`Preprocessor defined constant ${name} as ${value}`);
            $("#commentary").append(`Replaced instances of ${name} with ${value}<br />`);
            $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
        }
    },

    prep_ifanalysis: function (desc) {
        return function () {
            console.log(`Completed analysis of if statements: ${desc}`);
            $("#commentary").append("Analysed which if statements apply where<br />");
        };
    },

    prep_if: function (data) {
        return function () {
            console.log(`Replaced if statements`);
            $("#commentary").append("Removed areas that fail an if statement<br />");
            $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
        };
    },

    prep_done: function (data) {
        return function () {
            console.log(`Finished preprocessing: ${data}`);
            $("#commentary").append("Finished preprocessing<br />");
            $("#left").html(data.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
        };
    },

    tree: function (tree) {
        return function () {
            console.log(`Syntax tree in JSON form: ${tree}`);
            $("#commentary").append("Created syntax tree<br />");
        };
    },

    found_global: function (name, type, initial) {
        return function () {
            console.log(`Found global variable: Name is ${name}, type ${type}, initial ${initial}`);
            $("#commentary").append(`Found global variable: Name is ${name}, type ${type}, value ${initial}<br />`);
            $("#globals-table").append("<tr>" +
                `<td>${name}</td>` +
                `<td>${type}</td>` +
                `<td>${initial}</td>` +
                "</tr>");
        };
    },

    locals: function () {
        return function () {
            console.log("Found local variables");
            $("#commentary").append("Found local variables<br />");
        };
    },

    start_codegen: function () {
        return function () {
            console.log("Started code generation");
            $("#commentary").append("Started code generation<br />");
        };
    },

    gen_data: function () {
        return function () {
            console.log("Generating data section from globals");
            $("#commentary").append("Started generating data section from globals<br />");
        };
    },

    gen_data_line: function (name, type, initial, asm) {
        return function () {
            console.log(`Data section line. Name is ${name}, type is ${type}, initial is ${initial}: ${asm}`);
            $("#commentary").append(`Data section line. Name is ${name}, type is ${type}, initial is ${initial}: ${asm}<br />`);
        };
    },

    fin_gen_data: function (asm) {
        return function () {
            console.log(`Finished generating data section: ${asm}`);
            $("#commentary").append("Finished generating data section<br />");
            $("#right").html(asm.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
        };
    },

    gen_text: function () {
        return function () {
            console.log("Generating text section");
            $("#commentary").append("Started generating text section<br />");
        };
    },

    gen_block: function (name) {
        return function () {
            console.log(`Generating code block called ${name}`);
            $("#commentary").append(`Generating code block called ${name}<br />`);
        };
    },

    gen_stmt: function (desc, asm) {
        return function () {
            console.log(`Generating statement: ${desc}`);
            console.log(`Code for statement: ${asm}`);
            $("#commentary").append(`${desc}:<br />${asm}<br />`);
        };
    },

    finish: function (asm) {
        return function () {
            console.log(`Finished. Final assembly code:\n${asm}`);
            $("#right").html(asm.replace(/\n/g, "<br/>").replace(/ /g, "&nbsp;"));
            throw "Finished";
        };
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
    for (let i = 0; i < prog_lines.length; i++) {
        let line = prog_lines[i];
        let instr = line.splitWithTail(" ", 1)[0].trim();
        let after = line.splitWithTail(" ", 1)[1].trim();
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
                animation_queue.push(animations.start(json));
                break;
            case "prep_include":
                animation_queue.push(animations.prep_include(json[0], json[1]));
                break;
            case "prep_define":
                animation_queue.push(animations.prep_define(json[0], json[1], json[2]));
                break;
            case "prep_ifanalysis":
                animation_queue.push(animations.prep_ifanalysis(json));
                break;
            case "prep_if":
                animation_queue.push(animations.prep_if(json));
                break;
            case "prep_done":
                animation_queue.push(animations.prep_done(json));
                break;
            case "tree":
                animation_queue.push(animations.tree(json));
                break;
            case "found_global":
                animation_queue.push(animations.found_global(json[0], json[1], json[2]));
                break;
            case "locals":
                animation_queue.push(animations.locals());
                break;
            case "start_codegen":
                animation_queue.push(animations.start_codegen());
                break;
            case "gen_data":
                animation_queue.push(animations.gen_data());
                break;
            case "gen_data_line":
                animation_queue.push(animations.gen_data_line(json[0], json[1], json[2], json[3]));
                break;
            case "fin_gen_data":
                animation_queue.push(animations.fin_gen_data(json));
                break;
            case "gen_text":
                animation_queue.push(animations.gen_text());
                break;
            case "gen_block":
                animation_queue.push(animations.gen_block(json));
                break;
            case "gen_stmt":
                animation_queue.push(animations.gen_stmt(json[0], json[1]));
                break;
            case "finish":
                animation_queue.push(animations.finish(json));
                break;
            default:
                console.log(`Unknown instruction type: '${instr}'`);
        }
    }
}

function animateCompilerStep()
{
    let action = animation_queue.shift();
    action();
}