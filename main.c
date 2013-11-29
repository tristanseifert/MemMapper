#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		printf("Usage: %s input_file output_file\n", argv[0]);
		return 0;
	}

	// Try to parse the input file
	memmap_entry_t* memoryMap = parse_memmap(argv[1]);

	if(!memoryMap) {
		perror("Could not open or parse memory map.\n");
		return 1;
	}

	printf("Writing memory map to %s...", argv[2]);

	return 0;
}

/*
 * This function attempts to read the memory map file and parse it.
 */
memmap_entry_t* parse_memmap(char* filename) {
	FILE* fp = fopen(filename, "rb");
	if(!fp) return NULL;

	// Get filesize
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	// Read file to memory
	char* file = malloc(sz + 2);
	memset(file, 0x00, sz+2);
	fread(file, sz, 1, fp);
	fclose(fp);

	// Allocate memory for first entry
	memmap_entry_t *entry, *lastEntry;

	char *linePointer; // used by strtok_r
	char *line = strtok_r(file, "\n", &linePointer);

	// Loop through each line
	while(line) {
		entry = malloc(sizeof(memmap_entry_t));
		memset(entry, 0x00, sizeof(memmap_entry_t));

		// Ignore comments
		if(line[0] != '#') {
			parse_line(entry, line);
		}

		// Find next line
		line = strtok_r(NULL, "\n", &linePointer);

		if(lastEntry) {
			lastEntry->next = entry;
		}
	}

	// We're done here
	return entry;
}

/*
 * Populates a memmap_entry_t struct from a line read from a memmap file
 */
void parse_line(memmap_entry_t *entry, char* line) {
	int counter = 0;
	char *item, *itemPointer;
	item = strtok_r(line, "\t", &itemPointer);

	// Deal with the items.
	while(item) {
		switch(counter) {
			case 0: // start
				entry->start = strtol(item, (char **) NULL, 16);
				break;

			case 1: // end
				entry->end = strtol(item, (char **) NULL, 16);
				break;

			case 2: // type
				entry->type = strtol(item, (char **) NULL, 10);
				break;

			case 3: // name
				entry->name = item;
				break;

			default: // we got more, wtf?
				break;
		}

		counter++;
		item = strtok_r(NULL, "\t", &itemPointer);		
	}

	printf("0x%.8X to 0x%.8X, type %i: %s\n", entry->start, entry->end, entry->type, entry->name);
}

#if defined(__MINGW32__)
char* strtok_r(char *str, const char *delim, char **nextp) {
	char *ret;

	if (str == NULL) {
		str = *nextp;
	}

	str += strspn(str, delim);

	if (*str == '\0') {
		return NULL;
	}

	ret = str;

	str += strcspn(str, delim);

	if (*str) {
		*str++ = '\0';
	}

	*nextp = str;

	return ret;
}
#endif