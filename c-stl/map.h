#ifndef __MAP_H_
#define __MAP_H_
#include<stdio.h>
#include<assert.h>
#include<string.h>
#include<stdlib.h>

/** special macro to specify a struct to be an entry of a map **/
#define 		as_map_item 				map_entry __map_entry

/** return value indicates operating map success or fail **/
#define 		OP_MAP_SUCCESS 				(0)
#define 		OP_MAP_FAILURE 				(-1)

/** this value recommand to be 2^n , the larger it is, the less conflict map will cause **/
#define 		DEFAULT_TABLE_SIZE 			32

typedef struct _map_entry
{
    unsigned int 		hashcode;
    char 				*key;
    int 				key_size;
    struct _map_entry 	*next;
} map_entry;

typedef struct _map
{
    size_t __map_size;
    map_entry **table;
    unsigned int table_size;
    unsigned int (*hash)(char*, int);
    size_t conflict; // conflict times.
    size_t puts; // put operate times.
}map;

//遍历哈希表
#define map_foreach(pmap, entry) \
         for(int i = 0; i < (pmap)->table_size; ++i)\
             for((entry) = (pmap)->table[i]; (entry) != NULL; (entry) = (entry)->next)\

// assess the no conflict, we recommand you to call this when map size equals spar, if you use
// map_init_custom(....) to init a map.
double get_conflict_ratio(map *map);
// the default hash function implement.
unsigned int hash(char *data, int len);
//init a map.
int map_init(map *map);
//return map container size.
size_t map_size(map *map);
//init map with hash func. and table size default
void map_init_custom(map *map, size_t spar, unsigned int (*hashfunc)(char*,int));

//插入key为int类型的元素
int map_put(map *map, int key, void *value);
//插入key为string类型的元素
int map_put_str(map *map, char *key, int key_size, void *value);
//根据int类型的值获得元素
map_entry *map_get(map *map, int key);
//根据string类型的值获得元素
map_entry *map_get_str(map *map, char *key, int key_size);

void map_clear(map *map);

map_entry *map_remove(map *map, char *key, int key_size);

// free its own memory, not include its elements.
void map_free_shallow(map *map);

// free its own memory, associated with its elements.
void map_free_deep(map *map);

#endif
