# Minishell
<br>

![image](https://user-images.githubusercontent.com/66511903/124838282-6ba85780-df7e-11eb-9f8a-42596efb1173.png)

In this project we are asked to mimic the behaviour of a bash shell.
We don't need to create a new window/ tabs our vertical splits like our iTerm (this is what I imagined the assignment would be).
Our assignment consists in creating a ./minishell executable that when opened, will show a prompt (`>msh`) and wait for user input.
The user can then type any shell commands like echo, ls, env export, pwd and we will show the exact same information / error messages as bash.
Fun fact: We can call other executables inside our minishell, including minishell itself.
So we can type `./minishell` to enter our minishell then `./minishell` again. We will then have to exit twice to finish all our programs.

### **DREAM TEAM**
This project was done in pairs. It taught us how to write cleaner code so that it can be easily used by our coding partner. We also learned the ancient art of the gitflow to smoothly sail through the new features and the nagging bugs.

I'd like to thank Gonçalo for the hard work he put in and for his commitment to write clean and sexy code. Keep sharing your knowledge like you brillantly did with this sweet README.
It was great working with you buddy! May we code together again at 42 or for a cool project!

#### Our work was generally split in the following way:
<br>

> **[Dimitri](https://github.com/DimitriDaSilva)** 
> 
> Extracting Information, Parsing, Termcaps, Executables, Pipes and Redirections

> **[Goncalo](https://github.com/gleal42)**
> 
> Environment Variables, Builtin Functions and Signal Handling

___

### Main challenges

1. [Extracting Information](#1-extracting-information)
2. [Parsing - Abstract Synthax Tree](2-parsing---abstract-synthax-tree)
3. [Environment Variables](#3-environment-variables)
4. [Termcaps](#4-termcaps)
5. [Execution](#5-execution)
   1. [Builtin functions](#51-builtin-functions)
   2. [Running other executables from our terminal](#52-running-other-executables-from-our-terminal)
   3. [Library executables (e.g. cat, ls)](#53-running-library-executables-eg-cat-ls)
   3. [Exit status ($?)](#54-exit-status-)
   4. [Assynchronous Vs Synchronous (Pros, cons and our hybrid approach)](#55-assynchronous-vs-synchronous-pros-cons-and-our-hybrid-approach)
6. [Signals](#6-signals)
7. [Pipes and Redirections](#7-pipes-and-redirections)
   1. [Redirections](#71-redirections)
   2. [Pipes](#72-pipes)
   3. [Multiple Redirections](#73-multiple-redirections)
   4. [Combining Pipes and Redirections](#74-combining-pipes-and-redirections)

___

### 1. Extracting Information

A minishell (in it's simplest form) will extract a full line (breaking the while loop when it reaches `\n`) and analyzing it.
To follow the initial logic, just imagine we're using get_next_line, saving each line. We will see later that, in order to use the arrows to navigate history we will have to modify the get_next_line function to allow us to deploy different functions when the arrows characters are pressed. These, modifications will be described in the Termcaps section.

**Output**

Full line exactly as user typed it:

Example that we will try to solve in the end of this tutorial:
`echo helllo > test; echo testiiing > a if it works | echo hmmm < a | cat > b > c < test`

Don't be scared. I'll try to make the previous example a piece of cake by the end.
___

### 2. Parsing - Abstract Synthax Tree

> **Sources**
>> https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf


The best way to explain this concept is to say that instead of creating functions for every case that user writes we interpret what is being written in categories and subcategories:

Instead of trying to guess what the user is going to write:

```
if (strncmp(input, "echo helllo", 7) == 0 && ft_strchr(input, '>')) == 0 && ft_strchr(input, '|') == 0)
	ft_echo_25_no_redirection_no_pipes();
else if (strncmp(input, "echo helllo", 7) == 0 && ft_strchr(input, '>')) != 0 && ft_strchr(input, '|') == 0)
	ft_echo_25_with_redirection_no_pipes();
...
```

We create the following categories to help us interpret the input:

#### Command tables
Independent groups of commands and redirections which can be separated by `;` `&&` `||`, depending on the exit status of the commands inside it (exit status will be explained in the environment variables section).

#### Commands
Default interpretation
- `[command identifier` (e.g. echo)] 
- `[command arguments]` starting with flags(-n in the case of echo) in case they exist)
- Separated by pipes `|`

#### Redirections
Interpreted as such if redirection signs (`>` `<` or `>>`) are found
- `[name of file to be created, updated or read (single word)]`
	 - Once the output from a command is redirected, following commands (separated by pipes)
 	 	 - e.g. `echo okay baby > a sure| cat` will have standard output empty 
 	 	 	 - file `a` will have `okay sure` written inside (okay before redirection and sure after redirection)
 	 	 	 - cat will receive nothing as input and it will print nothing as output

We will dwelve further into redirections in pipes in their sections.

#### Tokens
Finally, white spaces will be ignored used as split delimiters for our command tables input (except when they are inbetween single or double quotes). 


I recommend that you check our [parse](src/parse/parse.c) and [structs](src/structs.h) to better understand the processes that were conducted to convert the raw input into the aforementioned categories.

___

### 3. Environment Variables

> **Sources**
>> - https://stackoverflow.com/questions/2085302/printing-all-environment-variables-in-c-c 
>> - https://man7.org/linux/man-pages/man7/environ.7.html
>> - https://stackoverflow.com/questions/23608202/can-i-setenv-using-a-envp

These work very similarly to Makefile Variables. They are associated to specific functions and tasks and get replaced in the moment of execution.

Some examples of environment variables:
- `USER=gleal42` used by our 42 header
- `PATH` used everytime we write a command (shell will look for executables in different directories specified in the variable)
- `PWD` and `OLDPWD`updated everytime we change directories. OLDPWD is used for `cd -` to go back to previous directory.
- `HOME`used for `cd` (with no arguments)
- `SHLVL` used in case we call `./minishell`

Environment variables will be important in our Minishell in the following ways:

> If the user types:
> 
> `echo $PWD$USER`

> The standard output should print something like:
> 
> `/Users/gleal42/Desktop/42_Minishellgleal42`

This list of variables can be acessed in two ways:

1. By adding a third argument to main `(int main (int argc, char **argv, char **envp))`
2. By calling the environ global varialbe `extern char **environ`

If we were allowed to use functions that can actually alter environment variables such as `putenv` `setenv` and `unsetenv`, the second option (using the extern global variable *environ*) would be great. But, the only function we're allowed to use related to environment variables is `getenv`, and we also have a limit on the global variables that we can use (just one), which is why we chose to duplicate this array of strings (in a linked list for easier setting and unsetting of variables).

___

### 4. Termcaps

> **Sources**
>> - https://man7.org/linux/man-pages/man3/isatty.3.html
>> - https://www.mkssoftware.com/docs/man5/struct_termios.5.asp
>> - https://stackoverflow.com/questions/36258224/what-is-isatty-in-c-for/36258471
>> - https://linux.die.net/man/3/tcgetattr
>> - ibm.com/docs/en/aix/7.2?topic=library-understanding-terminals-curses
>> - https://man7.org/linux/man-pages/man5/termcap.5.html
>> - https://pubs.opengroup.org/onlinepubs/7990989799/xcurses/terminfo.html
>> - https://www.gnu.org/software/libc/manual/html_node/Canonical-or-Not.html
>> - https://pubs.opengroup.org/onlinepubs/7908799/xbd/termios.html
>> - https://smnd.sk/anino/programming/c/glibc-manual-0.02/library_16.html
>> - https://www.gnu.org/software/libc/manual/html_node/Noncanonical-Input.html
>> - https://www.ibm.com/docs/en/aix/7.2?topic=files-termiosh-file
>> - https://unix.stackexchange.com/questions/137842/what-is-the-point-of-ctrl-s

Termcaps stands for terminal capabilities. This 1992 Library is not easy to understand! Shout out to **[Dimitri](https://github.com/DimitriDaSilva)** for taming this monster. In this chapter I will summarize Dimitri's code comments and the main concepts. To better understand each function please read the comments in the code. 

As an old library, it has many and very complicated steps in order to work:

1. Protect the standard input file descriptor:

`if (!isatty(STDIN_FILENO))`

isatty is a function that checks if the standard input is pointing to our terminal (which is usually the case for 0, 1 and 2 file descriptors). This is important because we will use the STDIN fd in many termcaps functions. If it is not valid it is important to identify this error as soon as possible to display the correct error message (using the appropriate errno).

2. Save terminal settings before we change them using tcgetattr.
The settings are saved in a struct termios

```
struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
	speed_t c_ispeed;
	speed_t c_ospeed;
};
```
3. Using the tgetent function we check if there is a terminfo database (database with all terminal capabilities) for the TERM environment variable in our envp array and load the functions internally, so that the tgetstr function works.

4. We use tgetstr to check and save in a struct and tputs to activate the following capabilities:

	 1. We save ks -> keys_on (Transmit numpad/arrows). We we imediately use tputs to activate this functionality to output an ANSI code when these keys are pressed.
   2. We check and save ke -> keys_off capability that is the opposite of the previous capability so that we can activate it with (tputs) in order to quit the program correctly. Nothing happens when we turn it off but this way the terminal has exactly the same settings as when it started.
   3. The arrows and backspace ANSII code might also change from terminal to terminal so we need to check and save the key up (ku) key down (kd) key backspace (kb) characters in order to be compared with the ones read from the standard input.
   4. In order to reset the line we are also using the capabilities to delete the standard output (delete line "dl"). And, in case the cursor doesn't reset we use the "cr" capability to place it in the beginning of the current line. VSCode's and linux's terminals automatically reset the cursors but on the 42 MAC's it has to be done manually (we found out the hard way).

5. We change the terminal settings to change the way our read loop interprets the standard input and signals:

I think a great way to illustrate why we need to do this is if you use the command `cat` with no commands.
You can write as much as you want but it will only print a copy to the standard output once you press enter.
This is how our terminal is working in the beginning. Input is only interpreted if the read function finds a new_line.
However, when we press arrow up and down we want our terminal to interpret those characters, in order to navigate through our double linkedlist with the previously saved input.

The way that settings are changed is through bitwise operations:

| Initial settings(illustrative example) | 1 | 1 | 1 | 0 |
| ---------------- | - | - | - | - |
| ICANON (canonycal mode mask)| 0 | 1 | 0 | 0 |
| ~ICANON (inverted mask)| 1 | 0 | 1 | 1 |
| Init. Set &= ~ICANON | 1 | 0 | 1 | 0 |

Summing up:

~ is the BITWISE NOT operator inverts all bits
&= works exactly like a Mask in [Digital Art](https://www.youtube.com/watch?v=sbS5oUuGbGY):
- If mask is 0 then original value becomes 0.
- If mask is 1 then the original value doesn't change.

Okay, applying this logic we change several settings:

- Turn off canonycal processing ICANON
- Turn off local echo (we will have to write to standard output manually in order to see which characters were typed). ECHO
- Turn off specific signals (ctrl-c (SIGINT) and ctrl-z (SIGSTOP)) ISIG
- Turn off ^V chracter (otherwise an invisible character would be written (double enter needed). IEXTEN
- Turn off ctrl-s and ctrl-q, which were created in the time of teleprinters to make sure that the printers had time to physically print (in a paper) the code sent by the terminal. Obviously we won't need this feature because we don't print our code. We just post it on Github. IXON
- Read returns after single byte `termcaps->new_term.c_cc[VMIN] = 1;`
- Input waits 0 decisseconds before processing the read. `termcaps->new_term.c_cc[VTIME] = 0;` 

We then use `tcsetattr` to apply the new settings to our terminal.

6. We can now extract the input using the new settings and terminal capabilities:
	 1. We create a `char *buf[8192]` buffer to use in the read and write functions
	 2. We use ft_bzero to fill it with `0` to make sure that the string read is always null terminated.
	 3. We read 1-2 bytes at time because some extended ASCII characters, which can take up more than 1 byte.
	 4. We create different functions for each possible character:
			- `delete_single_char` will replace the last character with a `\0` and write it in the STDIO.
			- `parse_input_history` will clean current standard output and buffer, re-write the prompt and go through the double linkedlist, copy all characters of each command table to the buffer and write it in the STDIO.
			- `reset_cmd_line` - Replicate the ctrl-c behaviour
			- `exit_program` - Replicate the ctrl-d behaviour
			- Protect against printing invisible characters
			- Write all the other ASCII characters
	 5. If we find a line break we duplicate the buffer (allocating heap memory) and return it so it can be saved in the double linked list.
7. We reset the terminal setting to the original ones using `tcsetattr` with the termios struct we saved previously in `termcaps->old_term` in case we execute a different function that requires input (e.g. cat) in order for these to work normally.
8. We continue with parsing and execution (parsing described before).
___

### 5. Execution

So now we're successfully extracting the text and categorizing it in different [structures](src/structs.h):
**Command tables** are composed of commands which in turn are made of **tokens** and whose output or input can be redirected to or from files (**redirections**).

As we previously mentioned, the first token of a command is used to identify the command itself and the following tokens are the arguments.

In our assignement we are asked to recreate some standard library commands, while keeping the remaining commands working the same way.

I'll first describe how we recreated the builtins, how we can execute `.o` files from our minishell (including the minishell itself), how we can call other library functions like `cat` `ls` or `sleep` and explain how our assynchronous approach recreates the terminal expected behaviour (including the exit status behaviour).

### 5.1 Builtin functions

We were asked to remake the following builtins:

- `echo` with option ’-n’
- `cd` with only a relative or absolute path
- `pwd` without any options
- `export` without any options
- `unset` without any options
- `env` without any options and any arguments
- `exit` without any options

I'll just describe the main things to take into consideration and the allowed functions that we used:



It writes all the non-flag tokens to the standard input in order, separated by spaces, followed by a linebreak (`\n`)

If user uses ’-n’ flag then the tokens are not followed by a linebreak.

Edge Cases:

```
echo -nnnnnnn -nnnnnnn hello           baby
hello babymsh →
```
```
echo -na
-na
msh →
```

```
echo -nnnnn -na -nn
-na -nnmsh →
```
As you can see, after echo, there can be multiple arguments indicating the -n flag, and only then there will come the arguments that will be printed out to the standard output. If the first argument is a valid flag then there won't be a line break printed after the arguments.

#### `cd` with only a relative or absolute path

>functions
>> - `char *getcwd(char *buf, size_t size)`
>> - `int chdir(const char *path)`
>> - `DIR *opendir(const char *name);`
>> - `int closedir(DIR *dirp);`
>> - `int stat(const char *path, struct stat *buf);`

So maybe for cd we overcomplicated it a little bit and did a bit more than was asked.

We considered:
1. cd with no arguments
2. cd with argument `-` (which changes to the previous directory)
3. cd with relative path
4. cd with absolute path

Not only this but if our cd command is followed by a pipe `|`, then cd will not change directories BUT it will print error messages.

We also considered this.
So we could have cheated a little bit and try to use the same functions twice but in reverse but we have morals XD.
So we had to gather the same error information and trigger the same errors without actually changing directories. 

For cd with no pipes (actually changing directories)
You just save the current directory you're in an array of characters using `getcwd(array of chars)`.
Then you change directories using `chdir(first argument)`
Finally, you update the environment variables `PWD` and `OLDPWD` with the previous and current directory.

Now following this logic we can do 3 different things:

1. cd with no arguments - retrieves the value from the environment variable `HOME` and uses it as the argument for the `chdir(char *path)` function.
2. cd with argument `-` - retrieves the value from the environment variable `OLDPWD` and uses it as the argument for the `chdir(char *path)` function
3. cd with any other argument - the argument will be used as the argument for the `chdir(char *path)`

If the cd function is followed by a pipe:

We will use the opendir function to check for errors, since it has similar validations as chdir.
The only difference I found was that the permissions needed to open a directory are not the same as actually changing directories (I believe opening a directory is the read permission and changing directories inside is the executing permission).

To check for this permission we use the function stat (path, &statbuf). It works similarly to the `tcgetattr` in the sense that it loads all information from the path given as argument into the statbuf struct. And then, to change and access the information inside, we need to use bit operations.
In this case we just check the bits corresponsing to the execution permission of the path (`statbuf.st_mode & S_IXUSR`, which will be positive if the user has execution permission and zero if it doesn't).

#### `pwd` without any options

> functions
>> `char *getcwd(char *buf, size_t size)`

We just load the current path directory inside an array of chars using the same function we used in the cd function.

#### `export` without any options

export without options can have 2 functionalities:

1. If no arguments are provided it will print all assigned and unnassigned environment variables (I will show the difference in a bit).
The printing message should follow the following pattern:

**assigned environment variable 1**
`declare -x VAR="2"`

**assigned environment variable 1**
`declare -x VAR=""`

**unassigned environment variable** - export GREETING
`declare -x VAR`

Notice that the value (2) must be in-between quotes so it is important to make the distinction between

Not only that but the environment variables shall be in alphabetical order (or in ASCII order in this case).
We can achieve this with a `ft_lst_sort_str` function.

2. export with multiple arguments shall follow this pattern:

In this case we will need to add the arguments to the array of environment variables.
Because we saved a duplicate in a linked list of strings we can do this simply with a `ft_lstadd_front` function.


The input needs to follow the following pattern:

1. **assigned environment variable** - `export GREETING=HELLO`
2. **assigned environment variable** - `export GREETING=`
3. **unassigned environment variable** - `export GREETING`

You will also need to add some input validations like making sure that the environment variable doesn't have a digit, and display the correct error messages.

#### `unset` without any options

Similar logic to export.

Input shall follow the following pattern: 
`unset VAR`

We conduct some input validation (e.g. `unset VAR=123` and `unset $VAR` are not valid).

#### `env` without any options and any arguments

Print all the assigned environment variables. 
This means that if in our environment variable array we have:

````
VAR=HELLO
OKAY=
GREETING
````

Then `env` will print the following
````
VAR=HELLO
OKAY=
````
(skip the unassigned GREETING environment variable).

#### `exit` without any options

exit with no arguments was easy to replicate.
In this case we would only need to print `exit\n` to the program and use our `quit_program` function to exit the program with no memory leaks.

But we were asked to replicate the behaviour of the special shell variable `$?`.
This variable will save the exit status from the last executed function.
In our case we can check this behaviour by typing:

````
bash
bash

exit 45
echo $?
45
````
By typing bash twice we would enter a bash inside our bash (bash-ception)
By tiping `exit 45` we would set the exit status from the seconds bash to 45.
So when we type `echo $?` we will see the value from that environment variable (`45`)

But exit will behave differentely depending on the following arguments:

- if there are no arguments `exit`
	- We will quit the program with the exit status from the last executed function.

- if first argument is non-digit `exit sad` 
	- We will need to print `numeric argument required\n` and exit with exit status 2 (EXIT_GENERAL_ERROR)

- if there are multiple arguments `exit 1 2`
	- We print error message for `too many arguments`and don't quit the program.

- If there is only one numeric argument `exit 45`
	- We will quit the program with the exit status of the argument, which is what happens in our example:
````
bash
bash

exit 45
echo $?
45
````
If there are multiple commands (`ls | exit 45`) then we will not quit the program but simply set the exit status global variable `g_msh.exit_status` to the same exit status as if there was only one command:

- `ls | exit` - The `g_msh.exit_status` will be set to ls exit status (probably 0 EXIT_SUCCESS)
- `ls | exit ls`- The `g_msh.exit_status` will be set to 2 (EXIT_GENERAL_ERROR)
- `ls | exit 5`- The `g_msh.exit_status` will be set to 5 (the argument number)
- `ls | exit 5 6`- The `g_msh.exit_status` will be set to 1 (EXIT_FAILURE)

___

### 5.2 Running other executables from our terminal

> **Functions**
>> - `pid_t fork(void);`
>> - `pid_t waitpid(pid_t pid, int *status, int options);`
>> - `int execve(const char *pathname, char *const argv[], char *const envp[]);`

> **Sources**
>> - For making the functions work: https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY
>> - For deeper understanding of processes: https://www.youtube.com/watch?v=ss1-REMJ9GA&list=PL9IEJIKnBJjFNNfpY6fHjVzAwtgRYjhPw

The function that allows us to run an executable from our program is `execve`()
`pathname` argument is the relative or absolute path of the executable.
`argv[]` is an array with all the arguments of the function. e.g. cat filename. argv[0] would be filename.
`envp` is our environment variable list (which we had to turn back into array of strings).

The reason why we need to use fork and waitpid is because execve doesn't create a new process to execute the new .o file.

So to enter our minishell we compile the whole program:
`./minishell`

If we then run a execve executable file like the ./cub3d program we've created in previous projects then our ./minishell will be **REPLACED**, by the ./cub3d. 

So when we exit the cub3d we will not go back to our minishell terminal but instead we will go back to our regular terminal (which is not the behaviour we're trying to emulate here).

Okay, so hopefully you've understood what we have to fix. We have to find a way to execute other programs as an additional process, being then able to come back to our program once we're done executing the .o file.

In order to create an additional process which will execute another .o file we need to use the `fork` and `waitpid` functions.

Processes are identified by numbers:

2 tips:

- type `ps -e` or `ps -A` to see all processes currently running on your computer
- type `ps -a` while you're tweaking your functions in order to see if you're creating any dead children.

So in it's simplest form, we could execute a .o file using the following code:

```
if (fork() == 0)
{
	execve(path, argv, ennvp);
}
else
{
	waitpid()
}
```

However, using this formula would cause a problem. 
The next process/ command would only start once the first one is finished.

When we type `sleep 5 | ls`. We are supposed to see the files in the current directory information as soon as we type the command, and not after we wait 5 seconds.

ls is executed right after sleep, not after sleep is finished.

So, if we wanted to properly execute `sleep 5| ls`, in order to make sure that all executables of our command table get executed at the same time we could do (pseudo code for these executables):

```
	pit_t pid;
	int status;

	i = 0;
	while (i < 2)
	{
		pid = fork();
		if (pid == 0)
			execve(exec_path, cmds->data->tokens, envp);
		cmds = cmds->next;
		i++;
	}
	i = 0;
	while (i < 2)
	{
		wait(&status);
		i++;
	}
```

One important thing to note tis that we don't need to add `(if pid > 0)` because the child process ends in execve (wait is just collecting dead children, in the order that they get executed).

Sounds pretty dark, I know!

But that's not the final implementation. Once we get to the exit status environment variable `$?`. I'll explain the genius way that Dimitri came up with in order to keep our program assynchronous (so that sleep 5 | ls) works, and synchronous (so that the correct exit status is returned).

___

#### 5.3 Running library executables (e.g. cat, ls)

If you type `echo $PATH` inside your bash you'll see something like:
`/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/Library/Apple/usr/bin`

These are a series of paths separated by colons (`:`).

```
/usr/local/bin
/usr/bin:/bin
/usr/sbin:/sbin
/Library/Apple/usr/bin
```
In bash when you type `cat` or `ls`. The terminal will look for an executable (similar to the default `./a.out` from our programs) called `cat` inside each of the directories on the list, in the order that was provided.

This is what we tried to emulate. We extract the value of the `PATH` variable from our linked list of environment variables, we run `ft_split` using `:` as a separator and then, we `ft_strjoin` the typed command `cat` to each one of the directories and see if it exists using the stat function:

`if (stat(absolute_path, &statbuf) == EXIT_SUCCESS)`

After we have a path to an executable, we can use execve, fork and waitpid functions in the way described before in the previous sub-chapter.

___

#### 5.4 Exit status ($?)

Everytime we execute a function it can return SUCCESS (0) or FAILURE (not zero), which is called `exit status`.

In order to save this status we have a global variable called `g_msh.exit_status`.

In our program we collect this status differently for builtins and executables:

- Everytime we execute a builtin set `g_msh.exit_status` to their returned value.
  - e.g. `g_msh.exit_status = ft_cd(nonexistent_path, env)` will set the global exit status variable to 1;

- Everytime we execute an executable using `execve` we update the `g_msh.exit_status` using the MACROS:
  - `WIFEXITED(status)` most of the time.
  - `WEXITSTATUS(status)` in case an executable was interrupted by a signal (e.g. we press ctrl-\ after typing sleep 5)

___

#### 5.5 Assynchronous Vs Synchronous (Pros, cons and our hybrid approach)

Okay so we've already explained why it is important for processes to work assynchronously.
`sleep 5 | ls` will show the files in the current directory as soon as we type the command table.

However there's a little problem that we need to solve.  By using `wait(&status);` we are updating the status in the order of execution of the processes.

This means that even though sleep 5 started first, we know that it will be the last one to be fully executed.
So `sleep 5 | cd nonexistent_path; echo $?` will print 0 to the standard output even though we know that `cd nonexistent_path` returns 1.

So how can we solve this?
- We need to switch wait for waitpid so that we can specify the process id we want our program to collect the dead babies from.
- We need to save the process id numbers (pids in an array) so that we can specify that order

Again, using pseudo code to illustrate what I mean:

For `sleep 5 | cd nonexistent_path`

```
	pit_t pid[2];
	
	i = 0;
	while (i < 2)
	{
		pid[i] = fork();
		if (pid == 0)
			execve(exec_path, cmds->data->tokens, envp);
		cmds = cmds->next;
		i++;
	}
	i = 0;
	while (i < 2)
	{
		waitpid(pid[i])
		i++;
	}
```

Now we have another problem. We don't know how big the array will be. We could make an array of 1000 integers or... We make it sexy!

```
	pit_t pid_child;
	pid_t pid_parent;
	
	i = 0;
	while (i < nb_cmds)
	{
		pid_child = fork();
		if (pid_child == 0)
			execve(exec_path, cmds->data->tokens, envp);
		new_node = ft_lstnew((void *)pid_child);
		ft_lstadd_back(&cmd_table->pids, new_node);
		cmds = cmds->next;
		i++;
	}
	while (cmd_table->pids)
	{
		pid_parent = (pid_t)(*pids)->data;
		waitpid(pid, &exit_info, 0);
		ft_lstdel_first(pids, ft_lstdel_int);
	}
```

Okay so this is what is happening:

**Creating forks:**

1. create and save sleep process:

| sleep pid | 0 |
| --------- | - |

2. create and save cd process:

| sleep pid | cd pid | 0 |
| --------- | ------ | - |

**Saving exit_status in parent:**

Start:

| sleep pid | cd pid | 0 |
| --------- | ------ | - |

1. save sleep exit status and delete process id from linked list:

| cd pid | 0 |
| ------ | - |

2. save cd exit status and delete process id from linked list:

| 0 |
| - |

___

### 6. Signals

> **Sources**
>> - https://man7.org/linux/man-pages/man2/signal.2.html
>> - https://man7.org/linux/man-pages/man7/signal.7.html

We were asked to handle the following:

- CTRL-C
- CTRL-D (which is not a signal)
- CTRL-BACKSLASH

Now that we've talked about builtins, executables and about our termcaps it will be easier to understand how these work which.

The main issues that we must solve are that:
- Ctrl-D since it's not a signal, but a character (EOF) we don't need to do anything else. We've already dealt with it on our [Termcaps](#5-termcaps) section.
- Our minishell is an executable. So if we send a SIGINT(CTRL-C) signal or SIGQUIT (CTRL-\) it will self-quit the program. In our termcaps, while we're reading in non-canonycal mode, we already stop signals from doing this and print the correct messages in this case:
	- Ctrl-C it will print ^C and a linebreak
	- Ctrl-\ it will do nothing 

-  But if we were to start a cat or sleep and pressed ctrl-c/ or ctrl-\ we would exit both the cat /sleep executable as well as the minishell itself (not what we want).
- So we have to create signal catchers that will replace the "quitting" behaviour from this signals.
These catchers are made with the `signal` function, which redirects the signals to a specific function that we can define.
After some testing it appears that if we use an executable like `cat` then the signals SIGINT and SIGQUIT will continue to quit all the executables.

It will also print the `^C` character for Ctrl-C and not print anything for Ctrl-\) despite the different signal_catchers in our program.

So the messages we will need to print are:
- Ctrl-C just a linebreak `\n`
- Ctrl-\ the standard message: `Quit: 3\n`

**Warning**

If you're using a waitpid synchronously (meaning that if you write `sleep 4 | sleep 5`, then the process for the second sleep will only fork once the first sleep is finished. Then the signals will not work as expected. What I suggest is that you either change it to asynchronous (how we have it) or:



```
	else if (pid == 0)
	{
		if (g_msh.exit_status == 3 && process_index != 0)
		{
			if (process_index == nb_cmds - 1)
				printf("Quit: 3\n");
			signal(SIGQUIT, SIG_DFL);
			kill(0, SIGQUIT);
		}
```

So basically our signal will quit or interrupt the first process. But for the remaining we will have to do it manually.
We count the number of commands in the command table and we use a index to send the `SIGQUIT` signal the right amount of times. To send the sigquit signal without having to click the ctrl-\ we use the function `kill`. Kill is not just used to kill processes but to send all signals.

Check commit 9e39fc1337f797ac41d4312fd95c162dc8209c58 for first implementation of this previous signal code.

___

### 7. Pipes and Redirections

#### 7.1 Redirections

> **Functions**
>> - `int dup2(int oldfd, int newfd);`
>> - `int open(const char *pathname, int flags);`
>> - `int close(int fd);`

> **Sources**
>> https://www.youtube.com/watch?v=5fnVr-zH-SE


Alright, we're coming close to an end. By the end of this chapter we will have covered all the main concepts needed to do this project. It was a lot of fun.
I also recommend looking at the sources. [Code Vault](https://code-vault.net/) has the best tutorials and he covers this topic really well! He is a life-saver.

Okay if I remember [Tiago's](https://github.com/Olbrien) masterclass correctly (it has been a while but I'll do my best)the first thing we should do is keep tab of the file descriptors:

| STDIN | 0 |
| ----- | - |
| STDOUT| 1 |
| STDERR| 2 |

First we will need to understand the concept of redirection:

Let's start with a basic example to illustrate redirection. We want `ls > file1` to work like the terminal.

ls - Will print to the standard output the name of the files in the current directory.

Our task here is to transfer whatever was printed to the standard output to the file.

In order to do this the first step we need to take is to create a file descriptor for `file1`.

````
int new_file;
new_file = open("file1", O_WRONLY | O_CREAT | O_TRUNC);
````

| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  |
| 3(just created)  | file1|

Okay, now. In order to redirect the standard output to the file we will need to use the function dup2.

`dup2(new_file, STDOUT_FILENO)`

| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    | file1|
| STDERR_FILENO    |   2  |
| 3(just created)  | file1|

Now whatever would be printed into the standard output will instead be written inside file1.
This means that if we were to write:
`printf("yo yooooooo");`
When we compiled the program the message wouldn0't be printed but instead be transferred inside file1.

Now there are 2 opened file descriptors pointing to file1 (STDOUT_FILENO (1) and the new_file descriptor we opened (3)).
Because we don't intend to use the function `write` to add new text manually to file1 we can just close the file descriptor 3.
`close(new_file);`

Alright! And that is a successful basic redirection.

Another useful notion is the redirection in the opposite way (`cat < file1`):

cat - Will read from a file specified in its arguments. In this case (no arguments) cat will read from the standard input and print it to the standard input.
By having this redirection `<` what is happening is:

1. We're opening file1 with reading permissions instead of writing permissions:
- `new_file = open("file1", O_RDONLY;`

2. We make file1 file descriptor a copy of stanrdard input so that when cat tries to read from standard input it actually reads from file1.
- `dup2(new_file, STDIN_FILENO)`

3. We have the same problem where new_file has 2 file descriptors open so we need to close the old file descriptor:
- `close(new_file);`

Since we're on the topic, in order to do the `>>` redirection (where we append the redirected text in case there's already a file with information in it) we just need replace to the flag `O_TRUNC` by the `O_APPEND` flag.
`new_file = open("file1", O_WRONLY | O_CREAT | O_APPEND);`

Now it's time to combine this idea of redirection with the idea of pipes.

#### 7.2 Pipes

> **Functions**
>> - `int pipe(int pipefd[2]);`
>> - `int dup2(int oldfd, int newfd);`
>> - `int close(int fd);`
>> - `int dup(int oldfd);`

> **Sources**
>> - https://www.youtube.com/watch?v=6xbLgZpOBi8
>> - https://www.youtube.com/watch?v=NkfIUo_Qq4c
>> - https://www.youtube.com/watch?v=VzCawLzITh0


We want this command `ls | cat` to work like the terminal.

We could actually recreate this only using the information that we already have if we think about it:

`ls > file1`
`cat < file1`
`remove(file1)`

Okay I know that `remove` is not allowed for the project anyways... but if it was... Nobody would suspect a thing, haha.
But of course a lot could go wrong. For instance, if we have a file already named file1 we would erase everything that was inside.

So what we need is something temporary... Where we can write output at one point and use what we wrote as input for the next command.

This... ladies and gentlemen is what pipes are for.

So again. we start with the original file descriptors:

| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 

Now we have a pipe to create:

So first. The number of pipes will be the number of commands minus 1.

Commands:
- ls
- cat

2 commands so 1 pipe.

For each pipe we will need to create an array of 2 integers (we can allocate the memory or, in our case, for demonstration purposes we declare it on stack).

So we got our 2 int array `int fd[2];`

Then we have to convert this array into a pipe using the pipe function: `pipe(fd)`.
Now the file descriptors fd[0] and fd[1] are open.
Now, whatever we write on fd[1] will be buffered until it is read by fd[0] or if it is closed manually (close function).

Now, one thing that kind of complicates things is the fact that ls and cat are executables (we need to call fork and execve):

```
	pit_t ls;
	pid_t cat;
	int fd[2];

	pipe(fd);
	ls = fork();
	if (ls == 0)
		execve(/bin/ls, NULL, NULL);
	cat = fork ()
	if (cat = 0)
		execve(/bin/cat, NULL, NULL);
	waitpid(ls, NULL, 0);
	waitpid(cat, NULL, 0);
```

The reason why things get complicated is because processes replicate the parent process, including the file descriptors and existing pipes:

**Main Process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 
| read_pipe_fd     | fd[0]|
| write_pipe_fd    | fd[1]|

**ls process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 
| read_pipe_fd     | fd[0]|
| write_pipe_fd    | fd[1]|

**Cat Process**
| File descriptors |  file |
| ---------------- | ----- |
| STDIN_FILENO     |   0   |
| STDOUT_FILENO    |   1   |
| STDERR_FILENO    |   2   | 
| read_pipe_fd     | fd[0] |
| write_pipe_fd    | fd[1] |

Now that we have these temporary file descriptors we can redirect the standard output from the ls executable inside the pipe (so that it can be used as input later by the cat executable):

```
...
	if (ls == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		execve(/bin/ls, NULL, NULL);
	}
...
```

Impact on file descriptors:

**Main Process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 
| read_pipe_fd     | fd[0]|
| write_pipe_fd    | fd[1]|

**ls process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    | fd[1]|
| STDERR_FILENO    |   2  | 
| read_pipe_fd     | fd[0]|
| write_pipe_fd    | fd[1]|

**Cat Process**
| File descriptors |  file |
| ---------------- | ----- |
| STDIN_FILENO     |   0   |
| STDOUT_FILENO    |   1   |
| STDERR_FILENO    |   2   | 
| read_pipe_fd     | fd[0] |
| write_pipe_fd    | fd[1] |

The reason I re-wrote all the file description tables is to show that we will have to deal with the opening and closing of fd's in all the processes (including the main process). All file descriptors that are not being used must be closed, otherwise unwanted behaviour might happen (infinity asking for standard input).

So, looking at the ls process file descriptors we can see that there are 2 unnecessary file descriptors than can be closed now fd[0] and fd[1].


```
...
	if (ls == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve(/bin/ls, NULL, NULL);
	}
...
```

So the final file descriptors of the ls process will be;

**ls process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    | fd[1]|
| STDERR_FILENO    |   2  |


Perfect. Now we just need to follow the same logic for the cat process and main process.

For the cat process we start with the following file descriptors opened:

**Cat Process**
| File descriptors |  file |
| ---------------- | ----- |
| STDIN_FILENO     |   0   |
| STDOUT_FILENO    |   1   |
| STDERR_FILENO    |   2   | 
| read_pipe_fd     | fd[0] |
| write_pipe_fd    | fd[1] |

Now we want to redirect the standard output that was stored in the pipe during the ls process to the standard input of the cat process.

```
...
	if (cat = 0)
	{
		dup2(fd[0], STDIN_FILENO);
		execve(/bin/cat, NULL, NULL);
	}
...

```

**Cat Process**
| File descriptors |  file |
| ---------------- | ----- |
| STDIN_FILENO     |   0   |
| STDOUT_FILENO    | fd[0] |
| STDERR_FILENO    |   2   | 
| read_pipe_fd     | fd[0] |
| write_pipe_fd    | fd[1] |

Now we have the same problem as before. The pipe file descriptors are still opened and not being used so we must close them.

```
...
	if (cat = 0)
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0])
		close(fd[1])
		execve(/bin/cat, NULL, NULL);
	}
...

```

After this, the final file descriptors for the cat process will be

**Cat Process**
| File descriptors |  file |
| ---------------- | ----- |
| STDIN_FILENO     |   0   |
| STDOUT_FILENO    | fd[0] |
| STDERR_FILENO    |   2   | 

We're almost done. There's only one more thing we have to deal with... I think you can guess what it is...

The pipe file descriptors on the main process are still opened (ugh.. file descriptors are such a mess, I know).

**Main Process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 
| read_pipe_fd     | fd[0]|
| write_pipe_fd    | fd[1]|

So we will need to close them, like we did on the other 2 processes.

```
...
	close(fd[0]);
	close(fd[1));
	waitpid(ls, NULL, 0);
	waitpid(cat, NULL, 0);
...
```
The final file descriptors of the main process will be:

**Main Process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 


Summary:

```
	pit_t ls;
	pid_t cat;
	int fd[2];

	pipe(fd);
	ls = fork();
	if (ls == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve(/bin/ls, NULL, NULL);
	}
	cat = fork ()
	if (cat = 0)
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve(/bin/cat, NULL, NULL);
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(ls, NULL, 0);
	waitpid(cat, NULL, 0);
```

File descriptors:

**ls process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    | fd[1]|
| STDERR_FILENO    |   2  |

**Cat Process**
| File descriptors |  file |
| ---------------- | ----- |
| STDIN_FILENO     |   0   |
| STDOUT_FILENO    | fd[0] |
| STDERR_FILENO    |   2   | 

**Main Process**
| File descriptors | file |
| ---------------- | ---- |
| STDIN_FILENO     |   0  |
| STDOUT_FILENO    |   1  |
| STDERR_FILENO    |   2  | 

Now we'll think about how these would work for more than 2 processes and create a general rule in order to apply it to our terminal:

We've already mentioned one of the rules:

1. The number of pipes will be the number of commands minus 1.

2. If instead of 2 processes we had 3 `ls | sort | cat`:
- There would be 2 pipes (4 file descriptors to close).
- ls and cat file descriptors would be treated exactly the same.
- sort would need to receive the ls standard output, sort it and redirect the standard output to cat.

```
	pit_t ls;
	pid_t sort;
	pid_t cat;
	int fd[2][2];

	pipe(fd[0]);
	pipe(fd[1]);
	ls = fork();
	if (ls == 0)
	{
		dup2(fd[0][1], STDOUT_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		execve(/bin/ls, NULL, NULL);
	}
	sort = fork();
	if (sort == 0)
	{
		dup2(fd[0][0], STDIN_FILENO);
		dup2(fd[1][1], STDOUT_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		execve(/bin/ls, NULL, NULL);
	}
	cat = fork ()
	if (cat = 0)
	{
		dup2(fd[1][0], STDIN_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		execve(/bin/cat, NULL, NULL);
	}
	close(fd[0][0]);
	close(fd[0][1]);
	close(fd[1][0]);
	close(fd[1][1]);
	waitpid(ls, NULL, 0);
	waitpid(cat, NULL, 0);
```

3. If instead of 2 processes we had 3 `ls | sort | cat`:
- There would be 2 pipes (2 more file descriptors to close).
- sort would need to receive the ls standard output, sort it and redirect the standard output to cat:
  - `dup2(fd[0][0], STDIN_FILENO);`
  - `dup2(fd[1][1], STDOUT_FILENO);`
- ls and cat file descriptors would be treated exactly the same:
  - The first process doesn't need to do `dup2(fd[0], STDIN)` because it is not receiving redirected output (from another process).
  - The last process doesn't need to do `dup2(fd[1), STDOUT)` because the standard output will not be redirected (it will be printed to the screen).

4. In our case, none of our builtin functions read from the standard input so `echo ola | echo baby | cat` will still work:

```
	pit_t echo1;
	pid_t echo2;
	pid_t cat;
	int fd[2][2];

	pipe(fd[0]);
	pipe(fd[1]);

//echo 1
	dup2(fd[0][1], STDOUT_FILENO);
	printf(ola\n);

//echo 2
	dup2(fd[0][0], STDIN_FILENO);
	dup2(fd[1][1], STDOUT_FILENO);
	printf(baby\n);
//cat
	cat = fork ()
	if (cat = 0)
	{
		dup2(fd[1][0], STDIN_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		execve(/bin/cat, NULL, NULL);
	}
	close(fd[0][0]);
	close(fd[0][1]);
	close(fd[1][0]);
	close(fd[1][1]);
	waitpid(ls, NULL, 0);
	waitpid(cat, NULL, 0);
```

As we can see, the initial standard output `ola\n` will be redirected to the first pipe `dup2(fd[0][1], STDOUT_FILENO);`.
The second echo will read that `ola\n` using `dup2(fd[0][0], STDIN_FILENO);` but only redirect `baby\n` (echo 2 standard output) to cat.


That brings us to the very last thing I will talk about. Which is how pipes and redirections are combined:

#### 7.3 Multiple Redirections

1. As we mentioned previously, file redirections work like pipes but they leave a trail on the way (the files that were created):

What I mean by this is that if we type `echo okay > a > b > c` the standard output `okay\n` will be redirected to `a` then to `b` and finally end up in `c`:
- File a will be empty.
- File b will also be empty
- File c will have the redirected standard output `okay\n`

2. Redirections work in sequence:
   - `echo baby > b > c > d` Standard output will be redirected to `b`. Then it will be redirected to `c` (replacing the previously set in `b`) and finally it will ne redirected to `d` (replacimg the previously set in `c`).
   - `echo baby > b; echo yeah > c; cat < b < c` First input from b would be redirected but then input c would also be redirected replace the b redirection. Only the input redirected ends up being the one from `c` (to cat as standard input).

Okay. These rules should be enough.

#### 7.4 Combining Pipes and Redirections

First, let's start by recapping how we're organizing our input and how we're looping throught the command tables:

```
typedef struct s_cmd
{
	t_list		*tokens;
	t_list		*redirs;
	int			index;
}				t_cmd;
```

A command table is composed by commands. Commands are divided by pipes.

As you can see, commands are a combination of tokens and redirections. 
When we execute our functions inside a command, these can either receive input from files our redirect their output to files before moving on to the next command.
If the output is not redirected to a file then it will move to the next command as input.

Okay I feel like this was a nice overview. Let's get into examples.

Let's wrap everything together and analyze the original command we started with:
`echo helllo > test; echo testiiing > a if it works | echo hmmm < a | cat > b > c < test`

Here we have 2 command tables:
1. `echo helllo > test`
2. `echo testiiing > a if it works | echo hmmm < a | cat > b > c < test`

The first command table is a simple redirection. `dup2(int test_fd, STDOUT_FILENO)`

The second command table is a bit more complex:

It has 3 commands:

1. `echo testiiing > a if it works`
   - tokens: `echo` `testiiing` `if` `it` `works`
   - redirections: `> a`
2. `echo hmmm`
   - tokens `echo` `hmmm`
   - redirections `< a`
3. `cat > b > c < test`
   - tokens `cat`
   - redirections: `> b` `> c` `< test`

1. Let's start with the first command:
First we execute the echo executable `echo testiiing if it works`
Then we redirect the standard output `testiiing if it works` inside the file a (using the function `open("a", O_WRONLY | O_CREAT | O_TRUNC)` and `dup2(int fd, STDOUT_FILENO)`).

2. Okay now we move on to the second command:
First we execute the echo executable `echo hmmm`
Then we execute the input redirection (Using the function `open("file1", O_RDONLY;` `dup2(int fd, STDIN_FILENO)`). This will have absolutely no effect because echo doesn't read from the standard input.


3. And for the grand finale, we have reached the third and final command:

For this one, the most important thing to note is that redirections have priority over pipes redirections.

1. This means that if we create a file with `echo message > file`
And then type the command `ls| cat < file`, ls will not be printed to the standard output.

2. This also means that if we type `echo message > a > b | cat`, the message will not be redirected to cat, stopping the redirection at b.

So, knowing this, what Dimitri did was:
1. Open all the files and do the redirections like they have been described previously.
2. In case there's no redirection do the pipes operations.

I hope you enjoyed this tutorial. It took longer than I wanted but I didn't want to leave it halfway done.

Any question shoot me a message on slack, email or Linkedin.

___

### Other Resources

Good overview
- https://github.com/tokenrove/build-your-own-shell

- https://github.com/brenns10/lsh
	- https://brennan.io/2015/01/16/write-a-shell-in-c/

1. https://hackernoon.com/lets-build-a-linux-shell-part-i-bz3n3vg1
2. https://hackernoon.com/building-a-linux-shell-part-ii-a-step-by-step-guide-pk203ywg
3. https://hackernoon.com/building-a-linux-shell-part-iii-wzo3uoi
4. https://hackernoon.com/building-a-linux-shell-part-iv-h21o3uwl
5. https://hackernoon.com/building-a-linux-shell-part-v-k61a3uai

https://nickolasteixeira.medium.com/want-to-build-a-simple-shell-heres-how-you-do-it-75890647bae8

1. https://indradhanush.github.io/blog/writing-a-unix-shell-part-1/
2. https://indradhanush.github.io/blog/writing-a-unix-shell-part-2/
3. https://indradhanush.github.io/blog/writing-a-unix-shell-part-3/
(Unfinished)


- https://medium.com/swlh/tutorial-to-code-a-simple-shell-in-c-9405b2d3533e
- https://danishpraka.sh/2018/01/15/write-a-shell.html

### Other Githubs
- https://github.com/R4meau/minishell/blob/master/src/main.c
- https://github.com/cclaude42/minishell
- https://github.com/pankratdodo/minishell-42
- https://adrienblanc.com/projects/42/minishell
- https://github.com/jraleman/42.Minishell
- https://git.42l.fr/Fabien/minishell

### Example of exercise from CS course
http://cs.brown.edu/courses/csci0330/shell1/

#### Parsing technique

- https://github.com/Swoorup/mysh

#### Suggested in Discord:

Recommended readings for parsing in minishell:
- https://en.wikipedia.org/wiki/LL_parser
- https://en.wikipedia.org/wiki/LR_parser
- https://en.wikipedia.org/wiki/Abstract_syntax_tree

If you want to go further in language theory:
- https://en.wikipedia.org/wiki/Formal_language
- https://en.wikipedia.org/wiki/Turing_machine
- https://en.wikipedia.org/wiki/Chomsky_hierarchy

- http://www.cems.uwe.ac.uk/~irjohnso/coursenotes/lrc/system/shell/index.html

Bash overview

https://www.gnu.org/software/bash/manual/bash.html#Shell-Operation

Environmental Variables

chdir
https://stackoverflow.com/questions/4998530/can-chdir-accept-relative-paths
https://stackoverflow.com/questions/3642050/chdir-not-affecting-environment-variable-pwd

Pipes
https://swcarpentry.github.io/shell-novice/04-pipefilter/index.html
https://askubuntu.com/questions/404141/why-cant-i-pipe-into-cd
