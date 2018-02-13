// The main file for the interpreter, once the file has been chosen.

$ = require("jquery");

let spawn = require("child_process").spawn;

console.log("Loaded interpreter_body.js");

let INTERPRETER_EXECUTABLE = "";
let LOADED_JSON = false;

$.ajaxSetup( { "async": false } );  // This will mean the program will wait until the page is loaded
$.getJSON("../properties.json", function(json) {
    INTERPRETER_EXECUTABLE = json["interpreter"];
    LOADED_JSON = true;
});
$.ajaxSetup( { "async": true } );

function beginInterpreter() {
    // Loads up the interpreter executable
    let filename = urlParam("fname");

    if (filename === undefined) {
        console.log("No filename given in URL parameters");
        return;
    }

    filename = decodeURIComponent(filename);    // Get rid of the %20s and stuff like that
    console.log("Loading bytecode file " + filename + " (" + typeof filename + ")");
    console.log("Using executable " + INTERPRETER_EXECUTABLE);

    let interpreter_proc = spawn(INTERPRETER_EXECUTABLE, ["-i", "-f", filename]);
    //interpreter_proc.stdin.setEncoding("utf8");
    //interpreter_proc.stdout.pipe(process.stdout)

    interpreter_proc.stdout.on('data', (data) => {
        console.log(`stdout: ${data}`);
    });
}