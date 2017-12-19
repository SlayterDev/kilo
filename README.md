# kilo

Simple text editor written in C. A few of its features are:
* Syntax Highlighting (C/C++, Python, and Javascript supported)
* Auto-Indentation
* Insert Mode
* Status Bar which displays useful info
    - Filename
    - Number of lines in file
    - Filetype (if recognized)
    - The current line number

kilo was written following [this tutorial](https://viewsourcecode.org/snaptoken/kilo/) from snapdragon. I
have since made some changes to it to add some of my own features. One such feature being configuration
files. Currently the config file only customizes two options, the tab stop, and the number of times to hit
quit when a file has unsaved changes. A standard config file would be named `.kilorc` and located in the
user's home directory. A sample config file would look like the following:

    tab_stop=4
    quit_times=3

I'll probably end up renaming this project at some point, in order to make it my own and that the name
matches too many system built in commands for tab completion.
