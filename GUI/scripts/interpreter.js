// The main file for the interpreter, once the file has been chosen.

require("string-format");

let beginInterpreter;
let animateInterpreterStep;
let interpreter = {
    "process": null,
    "running": false
};

// A few numbers defining how long certain animations take
anim_times = {
    fade_text: 500
};

// The animation object, containing all of its functions.
animate = (function () {
    return {
        queue: [],
        atomic: {
            notification: function (text) {
                return function () {
                    // Called to execute the animation (i.e. pushing a notification to the box)
                    $("#commentary").append("<br />" + text);
                }
            },

            change_pc: function (pc) {
                return function () {
                    // Changes the PC to the given value
                    $("#box-PC").fadeOut(anim_times.fade_text, function () {
                        $("#box-PC").text(pc).fadeIn(anim_times.fade_text);
                    });
                }
            },

            push_pc_to_mar: function () {
                return function () {
                    // Make PC bold, make MDR fade out and back in with new content
                    $("#box-PC").css("font-weight", "bold");
                    $("#box-MAR").fadeOut(anim_times.fade_text, function () {
                        // Fade out is finished, fade back in
                        $(this).css("font-weight", "bold");
                        $(this).text($("#box-PC").text()).fadeIn(anim_times.fade_text, function () {
                            // Fade in is finished, reset away from bold
                            $(this).css("font-weight", "normal");
                            $("#box-PC").css("font-weight", "normal");
                        });
                    });
                }
            },

            read_memory_to_mdr: function (pc, cmdlen) {
                return function () {
                    // First make the relevant memory boxes bold and record bytes to move
                    let table = document.getElementById("memtable");
                    let bytes = [];
                    for (let i = pc; i < pc + cmdlen; i++) {
                        let cell = $(`#memtable`).find(`tr:eq(${Math.floor(i/10)+1}) td:eq(${i%10 + 1})`);
                        cell.css("font-weight", "bold");
                        bytes.push(cell.text());
                    }


                    // Fade out the MDR
                    $("#box-MDR").fadeOut(500, function () {
                        //Change the value of the MDR and fade it back in
                        $(this).text(bytes.toString()).fadeIn(anim_times.fade_text);
                        // Now the animation is nearly over, stop stuff being bold
                        $("#memtable").find("td").css("font-weight", "normal");
                        $("#box-MDR").css("font-weight", "normal");
                    });
                }
            },

            push_mdr_to_cir: function () {
                // Make MDR bold
                $("#box-MDR").css("font-weight", "bold");
                $("#box-CIR").fadeOut(anim_times.fade_text, function () {
                    // Fade out is finished, fade back in
                    $(this).css("font-weight", "bold");
                    $(this).text($("#box-MDR").text()).fadeIn(anim_times.fade_text, function () {
                        // Fade in is finished, reset away from bold
                        $(this).css("font-weight", "normal");
                        $("#box-MDR").css("font-weight", "normal");
                    });
                });
            },

            change_cmp_reg: function (reg) {
                if (reg === "e") {
                    $("#reg-cmp-e").text("1");
                    $("#reg-cmp-n, #reg-cmp-p").text("0");
                } else if (reg === "n") {
                    $("#reg-cmp-n").text("1");
                    $("#reg-cmp-e, #reg-cmp-p").text("0");
                } else if (reg === "p") {
                    $("#reg-cmp-p").text("1");
                    $("#reg-cmp-e, #reg-cmp-n").text("0");
                }
            }
        },

        // The functions run when updates are given by the interpreter
        // They don't do anything, just add atomic items to the queue.
        fetch: function (pc, opcode, opbyte, operand1, operand2) {
            // First find the total length of the command
            let cmdlen = 2 + operand1.length + operand2.length;

            // Set the PC
            this.queue.push(this.atomic.notification("Incrementing PC"));
            this.queue.push(this.atomic.change_pc(pc));
            // Read the memory
            this.queue.push(this.atomic.notification("Moving PC to MAR"));
            this.queue.push(this.atomic.push_pc_to_mar());
            this.queue.push(this.atomic.notification("Reading memory"));
            this.queue.push(this.atomic.read_memory_to_mdr(pc, cmdlen));
            // Move this to the CIR
            this.queue.push(this.atomic.notification("Moving MAR to CIR"));
            this.queue.push(this.atomic.push_mdr_to_cir());
        },

        decode: function (name) {
            this.queue.push(this.atomic.notification(`Decoded as ${name}`));
        },

        execute: function (funcname, args) {
            // Corresponds to exec_func. Lots of variation.
            if (funcname === "cmp") {
                let parts = args.split(" ");
                this.execute_cmp(parts[0], parts[1], parts[2]);
            }
        },

        execute_cmp: function (arg1, arg2, result) {
            let truth;
            if (result === "e") {
                truth = "equal";
            } else if (result === "p") {
                truth = "greater than";
            } else if (result === "n") {
                truth = "less than";
            } else {
                console.log("Invalid result given for comparison");
                return;
            }
            this.queue.push(this.atomic.notification(`Comparing ${arg1} to ${arg2}: ${truth}`));
            this.queue.push(this.atomic.change_cmp_reg(result));
        }
    };
})();

(function () {

    $ = require("jquery");
    let execFile = require("child_process").execFile;

    console.log("Loaded interpreter_body.js");

    let INTERPRETER_EXECUTABLE = "";
    let LOADED_JSON = false;

    $.ajaxSetup( { "async": false } );  // This will mean the program will wait until the page is loaded
    $.getJSON("../properties.json", function(json) {
        INTERPRETER_EXECUTABLE = json["interpreter"];
        LOADED_JSON = true;
    });
    $.ajaxSetup( { "async": true } );

    beginInterpreter = function ()
    {
        // Loads up the interpreter executable
        let filename = urlParam("fname");

        if (filename === undefined) {
            console.log("No filename given in URL parameters");
            return;
        }

        filename = decodeURIComponent(filename);    // Get rid of the %20s and stuff like that
        console.log("Loading bytecode file " + filename + " (" + typeof filename + ")");
        console.log("Using executable " + INTERPRETER_EXECUTABLE);

        interpreter.process = execFile(INTERPRETER_EXECUTABLE, ["-i", "-d", "2", "-f", filename]);
        interpreter.running = true;
        //interpreter_proc.stdin.setEncoding("utf8");
        //interpreter_proc.stdout.pipe(process.stdout)

        interpreter.process.stderr.on('data', (data) => {
            console.log(`stderr: ${data}`);
        });

        interpreter.process.stdout.on("data", (data) => {
            console.log("Program output received");
            handleOutput(data);
        });

        interpreter.process.on("exit", (event) => {
            interpreter.running = false;
            console.log(`Interpreter process exited (code: ${event.code}, signal: ${event.signal}`);
        });

        interpreter.process.stdin.write("step\n");

        // There is no central animation loop here any more
        // It is all done via calls from controls.json
    };

    function handleOutput(data)
    {
        // This is run when the program gives some output. It will be some piece of data about a thing that has
        // happened, so this function will then cause a relevant animation to happen.

        // Sometimes the data will actually contain multiple commands, so check for that
        let parts = data.split("\n");
        if (parts.length > 1) {
            for (let i = 0; i < parts.length; i++) {
                handleOutput(parts[i]);
            }
            return;
        }


        // Checks for what it is
        if (data.startsWith("config")) {
            // This is the initial config info, including the size of the memory (necessary to set up the table)
            let config_json = data.substr(7);
            let config = JSON.parse(config_json);
            let memsize = config["memorykb"] * 1024;

            let table_row = "<tr>" + "<td style='font-weight: bold;'>{row}</td>" +
                            "<td>000</td>".repeat(10) + "</tr>";
            let table_html = "<tr><td></td>";

            // Add the header row
            for (let i = 0; i < 10; i++) {
                table_html += "<td style='font-weight: bold;'>" + i + "</td>";
            }
            table_html += "</tr>";

            // Add the rows of the table
            for (let i = 0; i < Math.ceil(memsize / 10); i++) {
                // Most of this is padding for the row
                table_html += table_row.replace("{row}", ("000" + i).substr(("000" + i).length - 3))
            }

            // Draw the table in the memory div
            $("#memtable").html(table_html)
        } else if (data.startsWith("mem")) {
            let items = data.split(" ");
            items = items.slice(1);

            console.log("New memory information given");

            // Add all of these bits of memory to the table
            for (let i = 0; i < items.length; i++) {
                let table = document.getElementById("memtable");
                table.children[Math.floor(i / 10) + 1].children[(i % 10) + 1].innerHTML = items[i];
            }
        } else if (data.startsWith("data")) {
            console.log("Reading environment data");
            $.getJSON("../env.json", function (env) {
                console.log("JSON taken from environment file, setting environment.");
                // Been given all data about the environment

                // Program counter
                $("#box-PC").text(env["pc"]);

                // Registers
                let register_names = ["eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp"];
                for (let i = 0; i < register_names.length; i++) {
                    $("#reg-" + register_names[i]).text(env["genregs"][register_names[i]]);
                }

                // Comparison registers
                $("#reg-cmp-p").text(env["cmp"]["p"]);
                $("#reg-cmp-e").text(env["cmp"]["e"]);
                $("#reg-cmp-n").text(env["cmp"]["n"]);

                // Memory
                for (let i = 0; i < env["memory"].length; i++) {
                    let table = document.getElementById("memtable");
                    table.children[Math.floor(i / 10) + 1].children[(i % 10) + 1].innerHTML = String(env["memory"][i]).padStart(3, "0");
                }
            });
        } else if (data.startsWith("fetch")) {
            let parts = data.split(" ");
            let pc = parts[1];
            let opcode = parts[2];
            let opbyte = parts[3];
            let operand1 = JSON.parse(parts[4]);
            let operand2 = JSON.parse(parts[5]);
            animate.fetch(pc, opcode, opbyte, operand1, operand2);
        } else if (data.startsWith("decode")) {
            let parts = data.split(" ");
            animate.decode(parts[1]);
        } else if (data.startsWith("exec_func")) {
            let parts = data.split(data, 2);
            animate.execute(parts[1], parts[2]);
        }
    }

    animateInterpreterStep = function () {
        // Makes one animation. If no animations are available in the queue,
        // step the interpreter and check again.
        if (animate.queue.length === 0) {
            interpreter.process.stdin.write("step\n");
            return false;
        }

        // There is now something in the queue
        let anim_function = animate.queue.shift();
        anim_function();

        return true;
    }

})();