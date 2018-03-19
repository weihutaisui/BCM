#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printHelp(const char *programName) {
    printf("usage:  %s newqrcode\n", programName);
}

int main( int argc, char *argv[] ) {
    char str[128] = {};
    const char filename[] = "/tmp/qrcode.txt";
    FILE *fptr;
    
    if (argc == 0 || strcmp(argv[1], "-help")==0 || strcmp(argv[1], "-?")==0) {
        printHelp(argv[0]);
        return 0;
    }

    // for now keep it simple:
    int arglen = 0;
    arglen = strlen(argv[1]);
    if (arglen > sizeof(str)) {
        printf("Error string to long: max size: %d\n", sizeof(str));
        return 1;
    }
    strncpy(str, argv[1], sizeof(str));

    fptr = fopen(filename, "w");
    if (fptr != NULL) {
        fprintf(fptr, "%s", str);
        fclose(fptr);
    }
    else {
        fprintf(stderr, "Error writing to %s\n", filename);
        return 1;
    }
    return 0;
}

