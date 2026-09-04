#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

#define KSH_READLINE_BUFSIZE 1024
#define ULONG unsigned long int
#define KSH_TOKEN_BUFSIZE 64
#define KSH_TOKEN_DELIMITER " \t\r\n\a\"'"

void ksh_loop(void);
char* ksh_read_line(void);
char** ksh_split_line(char* line);
int ksh_create_process(char** args);
int ksh_execute(char** args);

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

void ksh_loop(void) 
{
    char *line{};
    char **args{};
    int status{};    

    do {
        
        std::cout << "$ ";
        line = ksh_read_line();
        // std::cout << line << '\n';

        //? Maybe add what the person left out in parsing
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
    int inquote{0};

    if (!buffer) 
    {
        free(buffer);
        std::cerr << "ksh: Allocation error";
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') 
        {
            while (inquote) {
                std::cout << "dquote> " ; 
                int dquote_char{getchar()};
                buffer[position++] = static_cast<char>(dquote_char);
                
                if (position >= bufsize) 
                {
                    bufsize += KSH_READLINE_BUFSIZE;
                    buffer = static_cast<char*>(realloc(buffer, bufsize));
                    if (buffer == nullptr) 
                    {
                        free(buffer);
                        std::cerr << "ksh: Allocation error";
                        exit(EXIT_FAILURE);
                    }
                }
                if (static_cast<char>(dquote_char) == '"')
                {
                    inquote = !inquote;
                    while ((dquote_char = getchar()) != '\n' && c != EOF); 
                    break;
                } 
            }
            buffer[position] = '\0';
            return buffer;
        } 
        else 
        {
            if (static_cast<char>(c) == '"')
            {
                inquote = !inquote;
            }
            buffer[position] = static_cast<char>(c);
        }

        position++;

        if (position >= bufsize) 
        {
            bufsize += KSH_READLINE_BUFSIZE;
            buffer = static_cast<char*>(realloc(buffer, bufsize));
            if (buffer == nullptr) 
            {
                free(buffer);
                std::cerr << "ksh: Allocation error";
                exit(EXIT_FAILURE);
            }
        }
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
        int inquote{0};

        while (*line != '\0')
        {
            
            if (*line == '"') 
            {
                inquote = !inquote;
            }

            if (!inquote && isspace(static_cast<int>(*line))) {
                break;
            }
            line++;
        }

        if (*line != '\0') 
        {
            *line = '\0'; 
            line++;
        }
        
        char* read_head{token_start};
        char* write_head{token_start};

        while (*read_head != '\0') 
        {
            if (*read_head == '\'' || *read_head == '"') 
            {
                read_head++;
                continue;
            }

            *write_head = *read_head;
            read_head++;
            write_head++;
        }

        *write_head = '\0';
        tokens[position++] = token_start;

        if (position >= token_bufsize) 
        {
            token_bufsize += KSH_TOKEN_BUFSIZE;
            tokens = static_cast<char**>(std::realloc(tokens, sizeof(char*) * token_bufsize));
            if (tokens == nullptr) 
            {
                free(tokens);
                std::cerr << "ksh: Allocation error";
                exit(EXIT_FAILURE);
            }
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
