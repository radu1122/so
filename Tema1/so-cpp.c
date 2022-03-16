#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_LEN 256

int tokenize(char * input, char elems[MAX_LEN][MAX_LEN]) {
    char delimit[] = "\t \"[]{}<>=+-*/%!&|^.,:;()\\";

    int n = 0;
    int i = 0;
    while(i < strlen(input)) {
        char *ret = NULL;
        ret = strchr(delimit, (int) input[i]);
        if (ret != NULL) {
            strncpy(elems[n], input + i, 1);
            elems[n][1] = 0;
            n++;
            i++;
        } else {
            int first = i;
            i++;
            while(strchr(delimit, input[i]) == NULL) {
                i++;
            }

            strncpy(elems[n], input + first, i - first);
            n++;
        }
    }
    return n;
}

int checkIfIsTrue(hashmap * map, char key[MAX_LEN]) {
    char waitedValue[MAX_LEN];
    int status = hashmapGetOne(map, key, waitedValue);
    if (status == 1) {
        strcpy(key, waitedValue);
    }
    if (strlen(key) == 1) {
        if (strchr("123456789", key[0]) == NULL) {
            return 1;
        }
    } else if (strchr("123456789", (int) key[0]) == NULL ||
               strchr("123456789", (int) key[1]) == NULL) {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    hashmap * map = newHashmap();

    int step = 0;
    char paths[MAX_LEN][MAX_LEN];
    int pathsNo = 0;
    int i = 1;
    int existsInFile = 0;
    char inFile[MAX_LEN];

    int existsOutFile = 0;
    char outFile[MAX_LEN];

    memset(inFile, 0, MAX_LEN);
    memset(outFile, 0, MAX_LEN);
    while(i < argc) {
        if (argv[i][0] == '-') {
            if (argv[i][1] != 'D' && argv[i][1] != 'I' && argv[i][1] != 'o') {
                fprintf(stderr, "Bad arguments");
                return -1;
            }
        }
        if (step == 0) {
            if (argv[i][0] == '-') {
                if (argv[i][1] != 'D') {
                    step++;
                    continue;
                } else {
                   char key[MAX_LEN];
                   char value[MAX_LEN];
                   memset(key, 0, MAX_LEN);
                   memset(value, 0, MAX_LEN);
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
                        char elem[MAX_LEN];
                        sscanf(argv[i], "-I%s", elem);
                        memcpy(paths[pathsNo], elem, strlen(elem));
                        paths[pathsNo][strlen(elem)] = 0;
                        pathsNo++;
                    } else {
                        i++;
                        memcpy(paths[pathsNo], argv[i], strlen(argv[i]));
                        paths[pathsNo][strlen(argv[i])] = 0;
                        pathsNo++;
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
                step++;
            }
        } else if (step == 3) {
            if (argv[i][0] == '-' && argv[i][1] == 'o') {
                existsOutFile = 1;
                if (strlen(argv[i]) > 2) {
                    char elem[MAX_LEN];
                    sscanf(argv[i], "-o%s", elem);
                    memcpy(outFile, elem, strlen(elem));
                    outFile[strlen(elem)] = 0;
                } else {
                    i++;
                    memcpy(outFile, argv[i], strlen(argv[i]));
                    outFile[strlen(argv[i])] = 0;
                }
                i++;
            } else {
                memcpy(outFile, argv[i], strlen(argv[i]));
                outFile[strlen(argv[i])] = 0;
                i++;
                existsOutFile = 1;
            }
            if (argc > i) {
                fprintf(stderr, "bad input file");
                return -1;
            }
        }
    }

//    for(int i = 0; i < pathsNo; i++) {
//        printf("path %d: %s\n", i, paths[i]);
//    }
//    printf("%s\n", inFile);
    FILE * fIn = NULL;
    FILE * fOut = NULL;
    if (existsInFile == 1) {
        fIn = fopen(inFile, "r");
    }

    if (existsOutFile == 1) {
        fOut = fopen(outFile, "w");
    }


    char line[MAX_LEN];
    char * statusFgets;
    int status;
    int activeIf = 0;
    int activeIfD = 0;

    int ifsArr[10];
    int ifPpointer = -1;

    FILE * filesPtrArr[10];
    int filesPointer = -1;

    while (1) {
        if (filesPointer != -1) {
            statusFgets = fgets(line, MAX_LEN, filesPtrArr[filesPointer]);
            if (statusFgets == NULL) {
                fclose(filesPtrArr[filesPointer]);
                filesPointer--;
                continue;
            }
        } else {
            if (existsInFile == 1) {
                statusFgets = fgets(line, MAX_LEN, fIn);
            } else {
                statusFgets = fgets(line, MAX_LEN, stdin);
            }
            if (statusFgets == NULL) {
                break;
            }
        }

        line[strcspn(line, "\n")] = 0;
        if (line[0] == '#' && line[1] == 'd') {
            if (ifPpointer >= 0) {
                if (ifsArr[ifPpointer] > 0) {
                    continue;
                }
            }

            char value[MAX_LEN];
            char key[MAX_LEN];
            sscanf(line, "#define %s %[^\n]", key, value);
            if (value[strlen(value) - 1] != '\\') {
//                if (value[0] == '"') {
//                    memmove(value, value + 1, strlen(value));
//                    value[strlen(value) - 1] = '\0';
//                }
                char elems[MAX_LEN][MAX_LEN];
                memset(elems, 0, MAX_LEN * MAX_LEN);
                int tokensNo = tokenize(value, elems);
                char finalValue[MAX_LEN];

                strcpy(finalValue, "");
                for (int j = 0; j < tokensNo; j++) {
                    char waitedValue[MAX_LEN];
                    memset(waitedValue, 0, MAX_LEN);
                    status = hashmapGetOne(map, elems[j], waitedValue);
                    if (status == 1) {
                        strcpy(elems[j], waitedValue);
                    } else {
                        memset(waitedValue, 0, MAX_LEN);
                    }
                    strcat(finalValue, elems[j]);

                }
                hashmapPut(map, key, finalValue);
//                printf("key: %s\nfinal val: %s\n", key, finalValue);
            } else {
                char finalValue[MAX_LEN];
                char elems[MAX_LEN][MAX_LEN];
                int sw = 1;
                strcpy(line, value);
                memset(finalValue, 0, MAX_LEN);
                do {
                    if (line[strlen(line) - 1] == '\\') {
                        line[strlen(line) - 1] = '\0';
                    } else {
                        sw = 0;
                    }
                    memset(elems, 0, MAX_LEN * MAX_LEN);
                    int tokensNo = tokenize(line, elems);
                    for (int j = 0; j < tokensNo; j++) {
                        char waitedValue[MAX_LEN];
                        memset(waitedValue, 0, MAX_LEN);
                        status = hashmapGetOne(map, elems[j], waitedValue);
                        if (status == 1) {
                            strcpy(elems[j], waitedValue);
                        }
                        if (strcmp(elems[j],"\"") != 0 && strcmp(elems[j], " ")) {
                            strcat(finalValue, elems[j]);
                        }
                    }
                    if (existsInFile == 1) {
                        statusFgets = fgets(line, MAX_LEN, fIn);
                    } else {
                        statusFgets = fgets(line, MAX_LEN, stdin);
                    }
                    line[strcspn(line, "\n")] = 0;


                } while (sw == 1);

                hashmapPut(map, key, finalValue);
//                printf("key: %s\nfinal val: %s\n", key, finalValue);
            }
        } else if (line[0] == '#' && line[1] == 'i' && line[2] == 'f' && line[3] == ' ') {
            char key[MAX_LEN];
            sscanf(line, "#if %s", key);
            activeIf = checkIfIsTrue(map, key);
            if (ifPpointer == -1) {
                ifPpointer = 0;
            } else {
                ifPpointer++;
            }
            if (activeIf == 1) {
                ifsArr[ifPpointer] = 1;
            } else {
                ifsArr[ifPpointer] = -1;
            }
        }  else if (line[0] == '#' && line[1] == 'u' && line[2] == 'n' && line[3] == 'd') {
            char key[MAX_LEN];
            sscanf(line, "#undef %s", key);
            char waitedValue[MAX_LEN];
            int status = hashmapGetOne(map, key, waitedValue);

            if (status == 1) {
                hashmapRemoveOne(map, key);
            }
        } else if (line[0] == '#' && line[1] == 'e' && line[2] == 'l' && line[3] == 'i') {
            if (ifsArr[ifPpointer] == 1) {
                char key[MAX_LEN];
                sscanf(line, "#elif %s", key);
                activeIf = checkIfIsTrue(map, key);
                if (activeIf == 1) {
                    ifsArr[ifPpointer] = 1;
                } else {
                    ifsArr[ifPpointer] = -1;
                }
            }
        } else if (line[0] == '#' && line[1] == 'e' && line[2] == 'l' && line[3] == 's') {
            ifsArr[ifPpointer] = -ifsArr[ifPpointer];
        } else if (line[0] == '#' && line[1] == 'i' && line[2] == 'n' && line[3] == 'c') {
            char fileName[MAX_LEN];
            sscanf(line, "#include %s", fileName);
            if (fileName[0] == '"') {
                memmove(fileName, fileName + 1, strlen(fileName));
                fileName[strlen(fileName) -1] = '\0';
            }
            FILE * checkFile = NULL;
            char filePath[MAX_LEN];
            strcpy(filePath, "./_test/inputs/");
            strcat(filePath, fileName);
            checkFile = fopen(filePath, "r");
            int existsFile = 0;
            if (checkFile == NULL) {
                for (int j = 0; j < pathsNo; j++) {
                    strcpy(filePath, "");
                    strcat(filePath, paths[j]);
                    strcat(filePath, "\\");
                    strcat(filePath, fileName);
                    checkFile = fopen(filePath, "r");
                    if (checkFile != NULL) {
                        existsFile = 1;
                        fclose(checkFile);
                        strcpy(fileName, filePath);
                        break;
                    }
                }
            } else {
                existsFile = 1;
                fclose(checkFile);
                strcpy(fileName, filePath);
            }

            if (existsFile == 0) {
                fprintf(stderr, "Include file not found: %s\n", filePath);
                return -1;
            } else {
                if (filesPointer == -1) {
                    filesPointer = 0;
                } else {
                    filesPointer++;
                }
                filesPtrArr[filesPointer] = fopen(fileName, "r");
            }
        } else if (line[0] == '#' && line[1] == 'e' && line[2] == 'n' && line[3] == 'd') {
            ifsArr[ifPpointer] = 0;
            ifPpointer--;
        } else if (line[0] == '#' && line[1] == 'i' && line[2] == 'f' && line[3] == 'd') {
            char key[MAX_LEN];
            sscanf(line, "#ifdef %s", key);
            char waitedValue[MAX_LEN];
            int status = hashmapGetOne(map, key, waitedValue);
            if (status != 1) {
                activeIfD = 1;
            }
            if (ifPpointer == -1) {
                ifPpointer = 0;
            } else {
                ifPpointer++;
            }
            if (activeIfD == 1) {
                ifsArr[ifPpointer] = 2;
            } else {
                ifsArr[ifPpointer] = -2;
            }
        }  else if (line[0] == '#' && line[1] == 'i' && line[2] == 'f' && line[3] == 'n') {
            char key[MAX_LEN];
            sscanf(line, "#ifdef %s", key);
            char waitedValue[MAX_LEN];
            int status = hashmapGetOne(map, key, waitedValue);
            if (status == 1) {
                activeIfD = 1;
            }
            if (ifPpointer == -1) {
                ifPpointer = 0;
            } else {
                ifPpointer++;
            }
            if (activeIfD == 1) {
                ifsArr[ifPpointer] = 2;
            } else {
                ifsArr[ifPpointer] = -2;
            }
        } else if (line[0] != '#') {
            if (ifPpointer >= 0) {
                if (ifsArr[ifPpointer] > 0) {
                    continue;
                }
            }
            char elems[MAX_LEN][MAX_LEN];
            memset(elems, 0, MAX_LEN * MAX_LEN);
            int tokensNo = tokenize(line, elems);
            char finalLine[256];
            strcpy(finalLine, "");
            int ghilimeleNumbers = 0;

            for (int j = 0; j < tokensNo; j++) {
                if (strcmp(elems[j], "\"") == 0) {
                    ghilimeleNumbers++;
                    strcat(finalLine, elems[j]);
                    continue;
                }
                char waitedValue[MAX_LEN];
                int status = hashmapGetOne(map, elems[j], waitedValue);
                if (status == 1) {

                    if (ghilimeleNumbers % 2 == 0) {
                        strcpy(elems[j], waitedValue);
                    }
                }
                strcat(finalLine, elems[j]);

            }
            if (strlen(finalLine) != 0) {
                if (existsOutFile == 1) {
                    fprintf(fOut, "%s\n", finalLine);
                } else {
                    fprintf(stdout, "%s\n", finalLine);
                }
            }

        }

    }




    hashmapFree(map);

    if (existsInFile == 1) {
        fclose(fIn);
    }

    if (existsOutFile == 1) {
        fclose(fOut);
    }
    return 0;
}