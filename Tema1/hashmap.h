#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct mapElem {
    unsigned char key[256];
    int used;
    unsigned char data[256];
} mapElem;

typedef struct hashmap {
    int size;
    int currSize;
    mapElem *data;
} hashmap;
static unsigned int hashing(hashmap * m, unsigned char *str);
static hashmap * newHashmap();
static int getNewKey(hashmap* m, unsigned char *key);
static int mapResize(hashmap* m);
static int hashmapPut(hashmap * m, unsigned char * key, unsigned char * value);
static int hashmapGetOne(hashmap * m, unsigned char * key, unsigned char * value);
static void hashmapFree(hashmap * m);
static int hashmapRemoveOne(hashmap * m, unsigned char * key);

// https://stackoverflow.com/questions/7666509/hash-function-for-string
static unsigned int hashing(hashmap * m, unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % m->size;
}


static hashmap * newHashmap() {
    hashmap* map = (hashmap*) malloc(sizeof(hashmap));
    if(!map) exit(12);

    map->data = (mapElem*) malloc(512 * sizeof(mapElem));
    if(!map->data) exit(12);

    map->size = 512;
    map->currSize = 0;

    for (int i = 0; i < map->size; i++) {
        map->data[i].used = 0;
        memset(map->data[i].data, 0, 256);
        memset(map->data[i].key, 0, 256);
    }

    return map;
}

static int getNewKey(hashmap* m, unsigned char *key) {
    int curr;
    int i;

    if(m->size == m->currSize) return -1;

    curr = (int) hashing(m, key);

    for(i = 0; i < m->size; i++) {
        if(m->data[curr].used == 0) {
            return curr;
        }

        if(m->data[curr].used == 1) {
            if(strcmp(m->data[curr].key, key) == 0) {
                return curr;
            }
        }

        curr = (curr + 1) % m->size;
    }

    return -1;
}

static int mapResize(hashmap* m) {
    int i;
    int oldSize;
    int status;
    mapElem* curr;

    mapElem* temp = (mapElem *)
            calloc(2 * m->size, sizeof(mapElem));
    if(!temp) exit(12);

    curr = m->data;
    m->data = temp;

    oldSize = m->size;
    m->size = 2 * m->size;
    m->currSize = 0;

    for(i = 0; i < oldSize; i++){
        status = hashmapPut(m, (unsigned char *) curr[i].key, curr[i].data);
        if (status != 1)
            return -1;
    }

    for (i = oldSize; i < m->size; i++) {
        m->data[i].used = 0;
        memset(m->data[i].data, 0, 256);
        memset(m->data[i].key, 0, 256);
    }

    free(curr);

    return 1;
}

static int hashmapPut(hashmap * m, unsigned char * key, unsigned char * value) {
    int index;

    index = getNewKey(m, key);
    while(index == -1){
        if (mapResize(m) == -1) {
            exit(12);
        }
        index = getNewKey(m, key);
    }

    memcpy(m->data[index].data, value, strlen(value));
    memcpy(m->data[index].key, key, strlen(key));
    m->data[index].used = 1;
    m->currSize++;

    return 1;
}

static int hashmapGetOne(hashmap * m, unsigned char * key, unsigned char * value) {
    int i;
    int curr;

    value[0] = 0;

    if (m->currSize <= 0) {
        return -1;
    }

    curr = (int) hashing(m, key);

    for(i = 0; i < m->size; i++) {
        if(m->data[curr].used == 1) {
            if(strcmp(m->data[curr].key, key) == 0) {
                memcpy(value, m->data[curr].data, strlen(m->data[curr].data));
                value[strlen(m->data[curr].data)] = 0;
                return 1;
            }
        }

        curr = (curr + 1) % m->size;
    }


    return -1;
}

static int hashmapRemoveOne(hashmap * m, unsigned char * key) {
    int i;
    int curr;

    if (m->currSize <= 0) {
        return -1;
    }

    curr = (int) hashing(m, key);

    for(i = 0; i < m->size; i++) {
        if(m->data[curr].used == 1) {
            if(strcmp(m->data[curr].key, key) == 0) {
                strcpy(m->data[curr].data,"");
                strcpy(m->data[curr].key, "");
                m->data[curr].used = 0;
                m->currSize = m->currSize - 1;
                return 1;
            }
        }

        curr = (curr + 1) % m->size;
    }


    return -1;
}




static void hashmapFree(hashmap * m){
    free(m->data);
    free(m);
}