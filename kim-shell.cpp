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

