#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#define KSH_READLINE_BUFSIZE 1024
using namespace std;

void ksh_loop(void);
char* ksh_read_line(void);

int main(int argc, char** argv) 
{ 
    return EXIT_SUCCESS; 
}

void ksh_loop(void) 
{
    char *line{};
    char **args{};
    int status{};    

    do {
        cout << "> ";
        line = ksh_read_line();
        cout << line;
        // args = ksh_split_lines(line);
        // status = ksh_execute(args);

        free(line);
        // free(args);
    } while (status);
}

char *ksh_read_line(void) 
{
    int bufsize{KSH_READLINE_BUFSIZE};
    int position{0};
    char* buffer = static_cast<char*>(malloc(sizeof(char) * bufsize));
    int c{};

    if (!buffer) 
    {
        cerr << "ksh: Allocation error";
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
            buffer[position] = c;
        }

        position++;

        if (position >= bufsize) 
        {
            bufsize += KSH_READLINE_BUFSIZE;
            buffer = static_cast<char*>(realloc(buffer, bufsize));
            if (!buffer) 
            {
                cerr << "ksh: Allocation error";
                exit(EXIT_FAILURE);
            }
        }
    }
}