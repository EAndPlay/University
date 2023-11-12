#include <iostream>
#include <cstring>

char* ReadLine()
{
    const int kMaxLineLength = 1024;
    char *output_line = (char*) malloc(kMaxLineLength);
    memset(output_line, 0, kMaxLineLength);
    gets(output_line);
    return output_line;
}