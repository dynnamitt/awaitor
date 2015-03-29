#include <stdlib.h>
#include <string.h>

int starts_with( const char *str, const char *pre )
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}