#include <stdint.h>

typedef enum {
	kMemMapNormal = 0,
	kMemMapData = 1,
	kMemMapCode = 2,
	kMemMapStack = 3,
	kMemMapBSS = 4,
	kMemMapReserved = 5,
	kMemMapUnused = 6,
	kMemMapUserDefined = 7
} memmap_type_t;

typedef struct memmap_entry {
	uint32_t start;
	uint32_t end;
	memmap_type_t type;
	char* name;

	struct memmap_entry* next;
} memmap_entry_t;

memmap_entry_t* parse_memmap(char* filename);
void parse_line(memmap_entry_t *entry, char* line);

// MinGW is stupid and doesn't have strtok_r
#if defined(__MINGW32__)
char* strtok_r(char *str, const char *delim, char **nextp);
#endif