#include "hashmap.h"

int main(int argc, char *argv[]) {
    hashmap * map = newHashmap();

    int step = 0;
    char paths[256][256];
    int pathsNo = 0;
    int i = 1;
    int existsInFile = 0;
    char inFile[256];

    int existsOutFile = 0;
    char outFile[256];
    while(i < argc) {
        if (step == 0) {
            if (argv[i][0] == '-') {
                if (argv[i][1] != 'D') {
                    step++;
                    continue;
                } else {
                   char key[256];
                   char value[256];
                   memset(key, 0, 256);
                   memset(value, 0, 256);
                   if (strlen(argv[i]) > 2) {
                       // -DDEBUG
                       char *equalPtr = strchr(argv[i], '=');
                       if(equalPtr != NULL) {
                           memcpy(key, argv[i] + 2, equalPtr - argv[i] - 2);
                           memcpy(value, equalPtr + 1, strlen(argv[i]) - strlen(key) - 2);
                       } else {
                           memcpy(key, argv[i] + 2, strlen(argv[i]) - 2);
                       }
                   } else {
                       i++;
                       char *equalPtr = strchr(argv[i], '=');
                       if(equalPtr != NULL) {
                           memcpy(key, argv[i], equalPtr - argv[i]);
                           memcpy(value, equalPtr + 1, strlen(argv[i]) - strlen(key));
                       } else {
                           memcpy(key, argv[i], strlen(argv[i]));
                       }
                   }
                   hashmapPut(map,(unsigned char *) key, (unsigned char *) value);
                   printf("key: %s\nvalue: %s\n", key, value);
                   i++;
                }
            } else {
                step++;
                continue;
            }
        } else if (step == 1) {
            if (argv[i][0] == '-') {
                if (argv[i][1] != 'I') {
                    step++;
                    continue;
                } else {
                    if (strlen(argv[i]) > 2) {
                        char elem[256];
                        sscanf(argv[i], "-I%s", elem);
                        memcpy(paths[pathsNo], elem, strlen(elem));
                        paths[pathsNo][strlen(elem)] = 0;

                    } else {
                        i++;
                        memcpy(paths[pathsNo], argv[i], strlen(argv[i]));
                        paths[pathsNo][strlen(argv[i])] = 0;
                    }

                    i++;
                }
            } else {
                step++;
                continue;
            }
        } else if (step == 2) {
            if (argv[i][0] == '-') {
                step++;
                continue;
            } else {
                existsInFile = 1;
                memcpy(inFile, argv[i], strlen(argv[i]));
                inFile[strlen(argv[i])] = 0;
                i++;
            }
        } else if (step == 3) {
            if (argv[i][0] == '-' && argv[i][1] == 'o') {
                existsOutFile = 1;
                if (strlen(argv[i]) > 2) {
                    char elem[256];
                    sscanf(argv[i], "-o%s", elem);
                    memcpy(outFile, elem, strlen(elem));
                    outFile[strlen(elem)] = 0;
                } else {
                    memcpy(outFile, argv[i], strlen(argv[i]));
                    outFile[strlen(argv[i])] = 0;
                }
                i++;
            }
        }
    }

    printf("in file:%d %s\n",existsInFile, inFile);
    printf("out file:%d %s\n",existsOutFile, outFile);


    hashmapFree(map);
    return 0;
}