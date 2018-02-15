// The main file for the interpreter, once the file has been chosen.

require("string-format");

let beginInterpreter;
let interpreter_proc;

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

        interpreter_proc = execFile(INTERPRETER_EXECUTABLE, ["-i", "-f", filename]);
        //interpreter_proc.stdin.setEncoding("utf8");
        //interpreter_proc.stdout.pipe(process.stdout)

        interpreter_proc.stderr.on('data', (data) => {
            console.log(`stderr: ${data}`);
        });

        interpreter_proc.stdout.on("data", (data) => {
            handleOutput(data);
        });
    };

    function handleOutput(data)
    {
        // This is run when the program gives some output. It will be some piece of data about a thing that has
        // happened, so this function will then cause a relevant animation to happen.
        if (data.startsWith("config")) {
            // This is the initial config info, including the size of the memory (necessary to set up the table)
            let config_json = data.substr(7);
            let config = JSON.parse(config_json);
            let memsize = config["memorykb"] * 1024;

            let table_row = "<tr>" + "<td>{row}</td>" + "<td>000</td>".repeat(10) + "</tr>";
            let table_html = "<tr><td></td>";

            // Add the header row
            for (let i = 0; i < 10; i++) {
                table_html += "<td>" + i + "</td>";
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
        }
    }

})();