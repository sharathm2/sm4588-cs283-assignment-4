1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  The fork system call creates a new process by duplicating the current process. The reason we use this in conjunction with execvp is because execvp replaces the current process, meaning using it without a fork would result in the shell being replaced by this new process. By using fork, we can duplicate the current process into a child process and run execvp on the child process. The parent process(the shell) will still continue to run.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, it returns a -1 to our parent process(the shell), and no child process is created. In my implementation, this scenario is handled by checking if the fork process returns a value less then 0, in this case, -1. If this happens, we print perror("fork") which provides the user with an error message.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: Execvp() finds the command to execute by searching through the directories in the PATH environment variable. The PATH variable contains a list of directories which the system searches through to find a command. If the command is found within one of the directories, execvp() will execute the command, creating a new process. If the command is not found, execvp() will return -1, an error.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() system call is used in the parent process after forking in order to wait for the child process to finish executing and terminate. This allows the parent to get the exit status of the child process. If we didn't call wai(), the child process would continue to run until the parent process terminates.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() is used to get the exit status of the child process. This status value is returned by wait() or waitpid(). It provides the 8-bit exit status which is then passed into exit(). This is important because it allows the parent process to retrieve the exit status of the child process, which is used to determine if the command failed, or succeeded in running.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() handles quoted arguments by checking if a token starts with a double quote character. If it does, the function will read until it hits the matching closing double quote character, and it treats the entire quote as a single token. This is necessary because it allows users to pass in arguments with spaces in them, such as strings with spaces.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  In this assignment, I made changes to my parsing logic to handle quoted arguments. The previous assignment had parsing logic that involved handling multiple arguments seperated by pipe characters. In this assignment, the logic was simplified to handle only one command at a time, and then executing that specific command. This required changes that included tokenizing the command line input and storing the tokens. I ran into an error when I was dealing with the built-in command list provided in dshlib.h. I originally did not create a function to match the command with a command from the built-in command list, but I recieved an error when compiling my file which indicated that I had an undefined reference to match_command. So I fixed this by creating a function to match the command with a command from the built-in command list.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system are used to notify a process that an event has occurred. They are a form of IPC that allows a process to send a signal to another process. An example of this event would be a keyboard interrupt in the terminal. They differ from other forms of IPC because they are asynchronous, meaning that the sending process and receiving process do not need to be running at the same time. They also differ because signals are simple, and carry limited information.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: A signal that is used to forcefully terminate a process. It can't be ignored or caught meaning it will always kill the intended process. A usecase for this would be to kill a process that is running in an infinite loop. 
    
    SIGTERM: A signal that is used to request a process to terminate. It can be ignored, caught and handled. A usecase for this would be to terminate a process that is running, but needs to finish some tasks before terminating. "Polite request"
    
    SIGINT: A signal that is sent to a process when the user uses an keyboard interrupt character(CTRL+C) in the terminal. It is used to interrupt and terminate a process, but it can also be caught and handled in order to clean up resources before terminating.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is sent a signal to stop execution. It is immediately stopped and suspended but not terminated. SIGSTOP cannot be caught or ignored like SIGINT because it is designed to be a way to pause a process. It can be used for debugging purposes, or pausing a process for a specific amount of time.
