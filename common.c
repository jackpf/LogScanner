#include "common.h"

bool file_exists(char *filename)
{
    FILE *fh = fopen(filename, "r");
    
    bool exists = fh != null;
    
    fclose(fh);

    return exists;
}