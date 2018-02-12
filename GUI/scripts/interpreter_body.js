$ = require("jquery");
remote = require("electron").remote;
dialog = remote.dialog;

console.log("Loaded interpreter_body.js");

$("#choose-file").on("click", () => {
    console.log("File opener button clicked");
    console.log(dialog.showOpenDialog((fileNames) => {
        console.log(fileNames[0]);
    }));
});