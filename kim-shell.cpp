#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include "trie.h"
#include <termios.h>
#include <stdio.h>

#define KSH_READLINE_BUFSIZE 1024
#define ULONG unsigned long int
#define KSH_TOKEN_BUFSIZE 64
#define KSH_TOKEN_DELIMITER " \t\r\n\a\"'"

void ksh_loop(void);
char* ksh_read_line(void);
char** ksh_split_line(char* line);
int ksh_create_process(char** args);
int ksh_execute(char** args);
char* ksh_increase_buffer_size(char* buffer, ULONG* current_buffsize, ULONG additional_bufsize);
char** ksh_increase_buffer_size(char** buffer, ULONG* current_buffsize, ULONG additional_bufsize);

// BUILTIN FUNCS

int ksh_cd(char** args);
int ksh_help(char** );
int ksh_exit(char** );
int ksh_num_builtins();

const char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &ksh_cd,
    &ksh_help,
    &ksh_exit
};


int main() 
{ 
    ksh_loop();
    return EXIT_SUCCESS; 
}

char getch() {
    struct termios oldattr, newattr;
    char ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void ksh_loop(void) 
{
    std::cout << "\x1b[H\x1b[2J";

    char *line{};
    char **args{};
    int status{};  
    
    Trie KSH_TRIE{};
    std::ifstream terminal_command_file;
    terminal_command_file.open("terminal_commands.txt");
    
    if (terminal_command_file){
        char command[64];

        while (terminal_command_file.getline(command, 64)) {
            KSH_TRIE.insert(command);
        }
    
    } else {
        std::cout << "ksh: Unable to load command highlighting" << '\n';
    }

    terminal_command_file.close();

    // char mutable_str[] = "l"; 
    
    // char* ptr = mutable_str;  
    // std::cout << std::boolalpha;
    // std::cout << KSH_TRIE.search(ptr) << '\n';



    


    do {
        
        std::cout << "$ " << std::flush;
        line = ksh_read_line();
        // std::cout << line << '\n';

        //? Maybe add what the person left out in parsing 
        //? Change parsing to have the delimiter
        args = ksh_split_line(line);

        status = ksh_execute(args);

        free(line);
        free(args);
    } while (status);
}

char *ksh_read_line(void) 
{
    ULONG bufsize{KSH_READLINE_BUFSIZE};
    ULONG position{0};
    char* buffer = static_cast<char*>(malloc(sizeof(char) * bufsize));
    int c{};
    
    // inquote [ " , ' ] = {bool, bool}
    int inquote[2]{0, 0};

    if (!buffer) 
    {
        free(buffer);
        std::cerr << "ksh: Allocation error";
        exit(EXIT_FAILURE);
    }

    while (1) {

        c = getch();

        if (c == EOF || c == '\n') 
        {
            while (inquote[0] || inquote[1]) {
                std::cout << "dquote> " ; 
                int dquote_char{getchar()};
                if (dquote_char == EOF || dquote_char == '\n') {
                    continue;
                }
                buffer[position++] = static_cast<char>(dquote_char);
                
                if (position >= bufsize) 
                {
                    buffer = ksh_increase_buffer_size(buffer, &bufsize, KSH_READLINE_BUFSIZE);
                    if (buffer == nullptr) exit(EXIT_FAILURE);
                }
                if (static_cast<char>(dquote_char) == '"')
                {
                    inquote[0] = !inquote[0];
                    while ((dquote_char = getchar()) != '\n' && c != EOF); 
                    break;
                } 
                if (static_cast<char>(dquote_char) == '\'')
                {
                    inquote[1] = !inquote[1];
                    while ((dquote_char = getchar()) != '\n' && c != EOF); 
                    break;
                } 
            }
            buffer[position] = '\0';
            std::cout << '\n';
            return buffer;
        } 
        else if (c == '\x7f')
        {
            if (position == 0) {
                continue;
            }

            char char_to_remove = buffer[--position];
            if (char_to_remove == '"') {
                inquote[0] = !inquote[0];
            } else if (char_to_remove == '\'')  {
                inquote[1] = !inquote[1];
            } 

            buffer[position] = '\0';

        }
        else 
        {
            if (!inquote[1] && static_cast<char>(c) == '"')
            {
                inquote[0] = !inquote[0];
            }
            if (!inquote[0] && static_cast<char>(c) == '\'')
            {
                inquote[1] = !inquote[1];
            }
            buffer[position] = static_cast<char>(c);
            position++;
        }


        if (position >= bufsize) 
        {
            buffer = ksh_increase_buffer_size(buffer, &bufsize, KSH_READLINE_BUFSIZE);
            if (buffer == nullptr) exit(EXIT_FAILURE);
        }

        buffer[position] = '\0';
        std::cout << "\r\x1b[2K";
        std::cout << "$ " << buffer << std::flush;
    }
}

char** ksh_split_line(char* line)
{
    ULONG token_bufsize{KSH_TOKEN_BUFSIZE};
    ULONG position{0};
    char** tokens{static_cast<char**>(std::malloc(sizeof(char*) * token_bufsize))};
    
    
    if (!tokens)
    {
        free(tokens);
        std::cout  << "ksh: Allocation Error";
        exit(EXIT_FAILURE);
    }

    while (*line != '\0')
    {
        if (isspace(static_cast<int>(*line))) {
            line++;
            continue;
        }

        char* token_start = line;

        char* read_head{token_start};
        char* write_head{token_start};

        // read_head_inquote [ " , ' ] = {bool, bool}
        int read_head_inquote[2] = {0, 0};

        while (*read_head != '\0')
        {
            
            if (!read_head_inquote[1] && *read_head == '"') 
            {
                read_head_inquote[0] = !read_head_inquote[0];
                read_head++;
                line++;
                continue;
            }

            if (!read_head_inquote[0] && *read_head == '\'') 
            {
                read_head_inquote[1] = !read_head_inquote[1];
                read_head++;
                line++;
                continue;
            }

            if (!read_head_inquote[0] && !read_head_inquote[1] && isspace(static_cast<int>(*read_head))) {
                break;
            }

            *write_head = *read_head;
            write_head++;
            read_head++;
            line++;
        }

        // If we are at a space replace with null terminator
        // If we are at the end of the token string this will never be true
        if (*line != '\0') {
            *line = '\0';
            line++;
        }

        *write_head = '\0';
        tokens[position++] = token_start;
        
        if (position >= token_bufsize) 
        {
            tokens = ksh_increase_buffer_size(tokens, &token_bufsize, KSH_TOKEN_BUFSIZE);
            if (tokens == nullptr) exit(EXIT_FAILURE);
        }
        
    }
    tokens[position] = NULL;
    return tokens;
}


int ksh_create_process(char** args)
{
    pid_t pid{};
    int status{};    

    pid = fork();

    if (pid < 0)
    {
        std::cerr << "ksh: Unable to fork" << '\n';
    } 
    else if (pid == 0) 
    {
        if (execvp(args[0], args) == -1) {
            std::cerr << "ksh: Unable to exec" << '\n';
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int ksh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int ksh_cd(char **args) 
{
    if (args[1] == NULL) 
    {
        std::cerr << "ksh: expected argument to \"cd\"" << '\n';
    }
    else 
    {
        if (chdir(args[1]) != 0)
        {
            std::cerr << "ksh: unable to switch directories" << '\n';
        }
    }
    return 1;
}

int ksh_help(char **)
{
  int i{};
  std::cout << "Kim's KSH Shell\n";
  std::cout << "Type program names and arguments, and hit enter.\n";
  std::cout << "The following are built in:\n";

  for (i = 0; i < ksh_num_builtins(); i++) {
    std::cout << " " << builtin_str[i] << '\n';
  }

  std::cout << "Use the man command for information on other programs.\n";
  return 1;
}

int ksh_exit(char **)
{
  return 0;
}

int ksh_execute(char ** args)
{
  int i{};

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < ksh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return ksh_create_process(args);
}

char* ksh_increase_buffer_size(char* buffer, ULONG* current_buffsize, ULONG additional_bufsize)
{
    *current_buffsize += additional_bufsize;
    char* new_buffer = static_cast<char*>(std::realloc(buffer, (sizeof(char) * (*current_buffsize))));
    if (!new_buffer) {
        std::cout << "ksh: Allocation Error \n";
        free(new_buffer);
    }
    return new_buffer;
}
char** ksh_increase_buffer_size(char** buffer, ULONG* current_buffsize, ULONG additional_bufsize)
{
    *current_buffsize += additional_bufsize;
    char** new_buffer = static_cast<char**>(std::realloc(buffer, (sizeof(char*) * (*current_buffsize))));
    if (!new_buffer) {
        std::cout << "ksh: Allocation Error \n";
        free(new_buffer);
    }
    return new_buffer;
}