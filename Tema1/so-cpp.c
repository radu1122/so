#include "hashmap.h"

int main() {
    hashmap * map = newHashmap();

    char key1[10] = "1";
    char value1[10] = "test1";


    int status = hashmapPut(map,(unsigned char *) key1, (unsigned char *) value1);
    printf("status: %d\n", status);
    char output[256];
    status = hashmapGetOne(map, key1, output);

    printf("status: %d\n", status);

    printf("value: %s\n", output);

    hashmapFree(map);

    return 1;
}