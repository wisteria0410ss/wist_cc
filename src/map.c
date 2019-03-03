#include <stdlib.h>
#include <string.h>
#include "wist_cc.h"

Map *map_new(void){
    Map *map = (Map *)malloc(sizeof(Map));
    map->keys = vector_new();
    map->vals = vector_new();

    return map;
}

void map_put(Map *map, char *key, void *val){
    vector_push(map->keys, key);
    vector_push(map->vals, val);
}

void *map_get(Map *map, char *key){
    for(int i=map->keys->len-1;i>=0;i--){
        if(strcmp(map->keys->data[i], key) == 0)
            return map->vals->data[i];
    }
    return NULL;
}