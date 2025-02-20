#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <errno.h>

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

//CD Function to change directory
int change_directory(cmd_buff_t *cmd) {
    if (cmd->argc == 1) {
        return OK;
    } else if (cmd->argc == 2) {
        //If correct num of args passed, cd to specified path
        if (chdir(cmd->argv[1]) != 0) {
            perror("cd");
            return ERR_EXEC_CMD;
        }
    }
    return OK;
}

//Function to parse command line input
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    //Trim leading whitespace
    while (isspace((unsigned char)*cmd_line))
        cmd_line++;
    //Trim trailing whitespace
    char *end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';

    //Duplicate the trimmed command line into our buffer
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer)
        return ERR_MEMORY;
    cmd_buff->argc = 0;

    //Take command line input and parse into tokens
    char *p = cmd_buff->_cmd_buffer;
    while (*p) {
        //Skip any whitespace between tokens
        while (*p && isspace((unsigned char)*p))
            p++;
        if (!*p)
            break;
        
        char *token_start;
        if (*p == '"') {
            //If the token starts with "", skip the opening quote
            p++;
            token_start = p;
            //Read until we hit the closing quote
            while (*p && *p != '"')
                p++;
            //If a closing quote is found, terminate the token and move past it.
            if (*p == '"') {
                *p = '\0';
                p++;
            }
        } else {
            //For unquoted tokens, record the start of the token and read until white space
            token_start = p;
            while (*p && !isspace((unsigned char)*p))
                p++;
            //Terminate token if not at end of string
            if (*p) {
                *p = '\0';
                p++;
            }
        }
        //If the token is not empy, save it into argv array from dshlib.h
        if (token_start[0] != '\0') {
            cmd_buff->argv[cmd_buff->argc++] = token_start;
            if (cmd_buff->argc >= CMD_ARGV_MAX - 1)
                break;
        }
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
    
    return OK;
}

//Replaces main function in dsh_cli.c
//Function for continously prompting the user for input, parsing the input, and executing the command
//Uses fork() to create a child process to execute the command and execvp() to execute the command
int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc = 0;

    //Start loop prompting the user for input
    while (1) {

        //Prompt user for input and read user input
        printf("%s", SH_PROMPT);
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        //Remove the trailing newline character from cmd_line
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        //Check if user input is exit command
        if (strcmp(cmd_line, EXIT_CMD) == 0) {
            exit(OK);
        }

        //Parse the command line input into cmd_buff_t using our build_cmd_buff function
        rc = build_cmd_buff(cmd_line, &cmd);
        if (rc != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }


        //Use Built_In_Cmds match_command(const char *input);  and Build_In_Cmds enum types to determine if the command is a built in command
        Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
        //If command is CD run change_directory function, assign the return value of cd to rc for returning
        if (cmd_type == BI_CMD_CD) {
            rc = change_directory(&cmd);
        } else if (cmd_type == BI_NOT_BI) { //If command is not a built in command, fork the command into a child process and run exec_vp function
            pid_t pid = fork(); //Uses fork() to duplicate parent process into a new process
            if (pid == 0) {
                if (execvp(cmd.argv[0], cmd.argv) == -1) {
                    perror("execvp");
                    exit(ERR_EXEC_CMD);
                }
            } else if (pid < 0) {
                //Handle fork failure
                perror("fork");
            } else {
                //If fork is successful, wait for child process to finish and get exit status
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    rc = WEXITSTATUS(status);
                }
            }
        }

        free(cmd._cmd_buffer);
    }

    return rc;
}

//Function to match command with built in commands - Got an error when running the test files without this function implemented
Built_In_Cmds match_command(const char *input) {
    if (!input) return BI_NOT_BI;
    
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }
    
    return BI_NOT_BI;
}
