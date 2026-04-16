# myshell
Systems Programming Project 3
by Joel Anderson (jea211) and Ryan Lilly (rrl88)

TESTING STRATEGY:
To test our program, we created a tests folder containing subfolders for each important feature of our terminal. This includes the ability to handle piping, wildcards, and redirection as well as basic commands such as pwd, ls, and cd. Finally, error handling is tested as several incorrect commands are given in a variety of contexts.


CURRENT TESTS:

=====================BASIC==================
-echo.sh: Terminal repeats "repeat" based on the shell script "echo repeat"

-echo_bin.sh: Terminal repeats "repeat" from /bin/echo showing that mysh recognizes /bin/

-ls.sh: Terminal successfully registers ls command

===================BUILTINS================

-cd.sh: Path is printed before, after, and inbetween multiple cd .. commands

-cd_specific.sh: Path is printed before and after changing working directory to tests folder

-exit.sh: echos a message before and after exit. 

-pwd.sh: Simply prints the current working directory

==================ERRORS==================

-cd_error.sh: Tries to cd to a non-existent directory, resulting in an error

-cmd_error.sh: Tries to call a non-existent command, resulting in an error

-pipe_error.sh: Tries to pipe to a non-existent function, should run the first (existing) command but fail the second command.

-redir_error.sh: Tries to redirect to a fake file,

===================PIPES================

-pipes1.sh: Sends an echo command to "cat"

-pipes2.sh: Pipes to cat twice. If piping works, should be able to pipe multiple times so it shouldn't result in an error.

===============REDIRECTION===============

-in.sh, out.sh: Tests input and output redirection respectively in input.txt and out.txt files.

-in-n-out.sh: Tests multiple redirections within a single command.

===============WILDCARDS=================

-Populated with 5 .txt files (a-e).
-Change directory to wildcards and test by using:
--echo *.txt (should return all a-e.txt files)
--echo a* (should just return a)
--echo lmnop* (since no results, should just return lmnop*)