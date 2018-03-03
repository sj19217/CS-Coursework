* Created basic electron files
* Installed electron and other things (following https://www.tutorialspoint.com/electron/)
* Set up JetBrains WebStorm, had problems getting electron to work but eventually managed to set up the config right
  (process_files/Configurations.PNG)
* Followed web tutorial to make basic stuff
* Made skeleton versions of the app's files (ed72b15)
* After much frustration due to bad documentation and differing electron versions, got interpreter file dialog to work
* Got basic child process spawning working here, using properties.json to store the executables's path.
* Made this pass chosen filename onto another HTML file, being serviced by another Javascript file.
* Had some annoying bugs, but eventually got it working. Had to make loading of properties.json block the JS file,
  otherwise it would not have loaded by the time its data was necessary (56d2fab)
* Went and wrote gui.c/gui.h and used pauseUntilPermitted() and scattered its use around the program. 
* After lots of fussing, managed to get interpreter.js to print the interpreter's stderr to the log. Turned out the 
  streams in the interpreter needed flushing.
* Put some boxes on the screen to represent the parts of the program
* Make the table in the memory
* Made the memory scroll and fill in values when "mem" info is given
* Wrote the basic table in the centre and fiddled round with borders and labels
* Wrote the rest of the HTML for registers, etc.
* Made the interpreter print all of its information out properly
* Changed "env" command to write to env.json. 
* Wrote animation queue code
* Wrote the high-level step functions to add animations to the loop
* Wrote atomic functions but there are some graphical bugs still