#include "common.h"

bool file_exists(char *filename)
{
    FILE *fh = fopen(filename, "r");
    
    if (fh != NULL) {
        fclose(fh);
        return true;
    } else {
        return false;
    }
}