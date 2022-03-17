#include "hashmap.h"
#include <stdio.h>
#define MAX_LEN 256
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

int tokenize(char * input, char elems[MAX_LEN][MAX_LEN]) {
    char delimit[] = "\t \"[]{}<>=+-*/%!&|^.,:;()\\";

    int n = 0;
    size_t i = 0;
    char *ret = NULL;
    int first;
    while(i < strlen(input)) {
        ret = strchr(delimit, (int) input[i]);
        if (ret != NULL) {
            strncpy(elems[n], input + i, 1);
            elems[n][1] = 0;
            n++;
            i++;
        } else {
            first = i;
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
    int status = hashmapGetOne(map, (unsigned char *) key, (unsigned char *) waitedValue);
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
    size_t i = 1;
    int existsInFile = 0;
    char inFile[MAX_LEN];

    int existsOutFile = 0;
    char outFile[MAX_LEN];

    char fileName[MAX_LEN];

    char line[MAX_LEN];
    char * statusFgets;
    int status;
    int activeIf = 0;
    int activeIfD = 0;

    int ifsArr[10];
    int ifPpointer = -1;

    FILE * filesPtrArr[10];
    int filesPointer = -1;

    int ghilimeleNumbers = 0;

    FILE * fIn = NULL;
    FILE * fOut = NULL;

    char value[MAX_LEN];
    char key[MAX_LEN];

    char finalLine[MAX_LEN];


    char elems[MAX_LEN][MAX_LEN];

    int tokensNo;

    char finalValue[MAX_LEN];
    char waitedValue[MAX_LEN];

    char *equalPtr = NULL;

    char elem[MAX_LEN];

    size_t j = 0;

    FILE * checkFile = NULL;
    char filePath[MAX_LEN];

    int sw;

    int existsFile;

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
                   memset(key, 0, MAX_LEN);
                   memset(value, 0, MAX_LEN);
                   if (strlen(argv[i]) > 2) {
                       equalPtr = strchr(argv[i], '=');
                       if(equalPtr != NULL) {
                           memcpy(key, argv[i] + 2, equalPtr - argv[i] - 2);
                           memcpy(value, equalPtr + 1, strlen(argv[i]) - strlen(key) - 2);
                       } else {
                           memcpy(key, argv[i] + 2, strlen(argv[i]) - 2);
                       }
                   } else {
                       i++;
                       equalPtr = strchr(argv[i], '=');
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
                        memset(elem, 0, MAX_LEN);
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
                    memset(elem, 0, MAX_LEN);
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


    if (existsInFile == 1) {
        fIn = fopen(inFile, "r");
    }

    if (existsOutFile == 1) {
        fOut = fopen(outFile, "w");
    }




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


            memset(value, 0, MAX_LEN);
            memset(key, 0, MAX_LEN);


            sscanf(line, "#define %s %[^\n]", key, value);
            if (strlen(value) > 0) {
                if (value[strlen(value) - 1] != '\\') {
                    memset(elems, 0, MAX_LEN * MAX_LEN);
                    tokensNo = tokenize(value, elems);

                    strcpy(finalValue, "");
                    for (j = 0; j < tokensNo; j++) {
                        memset(waitedValue, 0, MAX_LEN);
                        status = hashmapGetOne(map, (unsigned char *) elems[j], (unsigned char *) waitedValue);
                        if (status == 1) {
                            strcpy(elems[j], waitedValue);
                        } else {
                            memset(waitedValue, 0, MAX_LEN);
                        }
                        strcat(finalValue, elems[j]);

                    }
                    hashmapPut(map, (unsigned char *) key, (unsigned char *) finalValue);
                } else {
                    memset(elems, 0, MAX_LEN * MAX_LEN);
                    sw = 1;
                    strcpy(line, value);
                    memset(finalValue, 0, MAX_LEN);
                    do {
                        if (line[strlen(line) - 1] == '\\') {
                            line[strlen(line) - 1] = '\0';
                        } else {
                            sw = 0;
                        }
                        memset(elems, 0, MAX_LEN * MAX_LEN);
                        tokensNo = tokenize(line, elems);
                        for (j = 0; j < tokensNo; j++) {
                            memset(waitedValue, 0, MAX_LEN);
                            status = hashmapGetOne(map, (unsigned char *) elems[j], (unsigned char *) waitedValue);
                            if (status == 1) {
                                strcpy(elems[j], waitedValue);
                            }
                            if (strcmp(elems[j],"\"") != 0 && strcmp(elems[j], " ")) {
                                strcpy(finalValue, elems[j]);
                            }
                        }
                        if (existsInFile == 1) {
                            statusFgets = fgets(line, MAX_LEN, fIn);
                        } else {
                            statusFgets = fgets(line, MAX_LEN, stdin);
                        }
                        line[strcspn(line, "\n")] = 0;


                    } while (sw == 1);

                    hashmapPut(map, (unsigned char *) key, (unsigned char *) finalValue);
                }
            } else {
                hashmapPut(map, (unsigned char *) key, (unsigned char *) value);
            }
        } else if (line[0] == '#' && line[1] == 'i' && line[2] == 'f' && line[3] == ' ') {
            memset(key, 0 , MAX_LEN);
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
            memset(key, 0 , MAX_LEN);
            sscanf(line, "#undef %s", key);
            memset(waitedValue, 0 , MAX_LEN);

            status = hashmapGetOne(map, (unsigned char *) key, (unsigned char *) waitedValue);

            if (status == 1) {
                hashmapRemoveOne(map, (unsigned char *) key);
            }
        } else if (line[0] == '#' && line[1] == 'e' && line[2] == 'l' && line[3] == 'i') {
            if (ifsArr[ifPpointer] == 1) {
                memset(key, 0 , MAX_LEN);
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
            memset(fileName, 0, MAX_LEN);
            sscanf(line, "#include %s", fileName);
            if (fileName[0] == '"') {
                memmove(fileName, fileName + 1, strlen(fileName));
                fileName[strlen(fileName) -1] = '\0';
            }


            checkFile = NULL;
            memset(filePath, 0, MAX_LEN);

            strcpy(filePath, "./_test/inputs/");
            strcat(filePath, fileName);
            checkFile = fopen(filePath, "r");
            existsFile = 0;
            if (checkFile == NULL) {
                for (j = 0; j < pathsNo; j++) {
                    strcpy(filePath, "");
                    strcat(filePath, paths[j]);
                    strcat(filePath, "/");
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
            memset(key, 0, MAX_LEN);
            sscanf(line, "#ifdef %s", key);
            memset(waitedValue, 0, MAX_LEN);

            status = hashmapGetOne(map, (unsigned char *) key, (unsigned char *) waitedValue);
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
            memset(key, 0, MAX_LEN);

            sscanf(line, "#ifndef %s", key);
            memset(waitedValue, 0, MAX_LEN);

            status = hashmapGetOne(map, (unsigned char *) key, (unsigned char *) waitedValue);
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
            memset(elems, 0, MAX_LEN * MAX_LEN);
            tokensNo = tokenize(line, elems);
            memset(finalLine, 0, MAX_LEN);

            strcpy(finalLine, "");
            ghilimeleNumbers = 0;

            for (j = 0; j < tokensNo; j++) {
                if (strcmp(elems[j], "\"") == 0) {
                    ghilimeleNumbers++;
                    strcat(finalLine, elems[j]);
                    continue;
                }
                memset(waitedValue, 0, MAX_LEN);

                status = hashmapGetOne(map, (unsigned char *) elems[j], (unsigned char *) waitedValue);
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