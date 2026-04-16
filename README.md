# myshell
Systems Programming Project 3
by Joel Anderson (jea211) and Ryan Lilly (rrl88)

TESTING STRATEGY:
blah blah blah

Current errors:
-Cat, grep and other unix commands don't work
-exit status msgs?
-which


Current Tests:

=====================basic==================
-echo.sh: Terminal repeats "repeat" based on the shell script "echo repeat"

-echo_bin.sh: Terminal repeats "repeat" from /bin/echo showing that mysh recognizes /bin/

-ls.sh: Terminal successfully registers ls command

===================builtins================

-cd.sh: Path is printed before, after, and inbetween multiple cd .. commands

-cd_specific.sh: Path is printed before and after changing working directory to tests folder

-exit.sh: echos a message before and after exit. 

-pwd.sh: Simply prints the current working directory

==================errors==================

-cd_error.sh: Tries to cd to a non-existent directory, resulting in an error

===================pipes================

-pipes1.sh: Sends an echo command to "cat"