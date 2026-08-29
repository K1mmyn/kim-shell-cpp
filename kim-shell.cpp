#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#define KSH_READLINE_BUFSIZE 1024
#define ULONG unsigned long int

void ksh_loop(void);
char* ksh_read_line(void);

int main() 
{ 
    ksh_loop();
    return EXIT_SUCCESS; 
}

void ksh_loop(void) 
{
    char *line{};
    // char **args{};
    int status{1};    

    do {
        std::cout << "> ";
        line = ksh_read_line();
        std::cout << line << '\n';
        // args = ksh_split_lines(line);
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