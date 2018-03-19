$ = require("jquery");
remote = require("electron").remote;
dialog = remote.dialog;

$("#choose-file").on("click", () => {
    console.log("File opener button clicked");
    console.log(dialog.showOpenDialog((fileNames) => {
        if (fileNames === undefined) {
            console.log("No file given");
            return;
        }

        console.log("File name: " + fileNames[0]);

        window.location.href = "compiler_active.html?fname=" + encodeURIComponent(fileNames[0]);
    }));
});