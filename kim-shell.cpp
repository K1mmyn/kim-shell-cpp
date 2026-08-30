#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#define KSH_READLINE_BUFSIZE 1024
#define ULONG unsigned long int
#define KSH_TOKEN_BUFSIZE 64
#define KSH_TOKEN_DELIMITER " \t\r\n\a"

void ksh_loop(void);
char* ksh_read_line(void);
char** ksh_split_line(char* line);

int main() 
{ 
    ksh_loop();
    return EXIT_SUCCESS; 
}

void ksh_loop(void) 
{
    char *line{};
    char **args{};
    int status{1};    

    do {
        std::cout << "> ";
        line = ksh_read_line();
        std::cout << line << '\n';

        //? Maybe add what the person left out in parsing
        args = ksh_split_line(line);
        ULONG position{0};

        while (!args[position])
        {
            std::cout << args[position] << '\n';
            position++;
        };

        // status = ksh_execute(args);

        free(line);
        // free(args);
    } while (status);
}

char *ksh_read_line(void) 
{
    ULONG bufsize{KSH_READLINE_BUFSIZE};
    ULONG position{0};
    char* buffer = static_cast<char*>(malloc(sizeof(char) * bufsize));
    int c{};

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
            buffer[position] = '\0';
            return buffer;
        } 
        else 
        {
            buffer[position] = static_cast<char>(c);
        }

        position++;

        if (position >= bufsize) 
        {
            bufsize += KSH_READLINE_BUFSIZE;
            buffer = static_cast<char*>(realloc(buffer, bufsize));
            if (!buffer) 
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
    char** tokens = static_cast<char**>(std::malloc(sizeof(char*) * token_bufsize));
    char* token{};

    if (!tokens)
    {
        free(tokens);
        std::cout  << "ksh: Allocation Error";
        exit(EXIT_FAILURE);
    }

    token = strtok(line, KSH_TOKEN_DELIMITER);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= token_bufsize) {
            token_bufsize += KSH_TOKEN_BUFSIZE;
            tokens = static_cast<char**>(std::realloc(tokens, token_bufsize));
            if (!tokens) {
                free(tokens);
                std::cout  << "ksh: Allocation Error";
                exit(EXIT_FAILURE);
            }
        };

        token = strtok(NULL, KSH_TOKEN_DELIMITER);
    }
    tokens[position] = NULL;
    return tokens;
}