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
  otherwise it would not have loaded by the time its data was necessary.