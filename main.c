#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "msvg.h"
/*
	kMemMapNormal = 0,
	kMemMapData = 1,
	kMemMapCode = 2,
	kMemMapStack = 3,
	kMemMapBSS = 4,
	kMemMapReserved = 5,
	kMemMapUnused = 6,
	kMemMapUserDefined = 7
*/
static char type_to_colour[8][8] = {
	"#fff", // normal
	"#ecc", // Data
	"#cec", // Code
	"#cce", // Stack
	"#eec", // BSS
	"#cee", // Reserved
	"#ccc", // Unused
	"#f0f", // User defined
};

int main(int argc, char** argv) {
	if(argc != 3) {
		printf("Usage: %s input_file output_file\n", argv[0]);
		return 0;
	}

	// Try to parse the input file
	memmap_entry_t* memoryMap = parse_memmap(argv[1]);

	if(!memoryMap) {
		perror("Could not open or parse memory map.\n");
		return -1;
	}

	printf("Writing memory map to %s...\n", argv[2]);

	// Create SVG root element
	MsvgElement *root, *son;
	root = MsvgNewElement(EID_SVG, NULL);
	MsvgAddAttribute(root, "version", "1.2");
	MsvgAddAttribute(root, "baseProfile", "tiny");
	MsvgAddAttribute(root, "width", "300");

	// We need to add all the XMLN's
	MsvgAddAttribute(root, "xmlns:dc", "http://purl.org/dc/elements/1.1/");
	MsvgAddAttribute(root, "xmlns:cc", "http://creativecommons.org/ns");
	MsvgAddAttribute(root, "xmlns:rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
	MsvgAddAttribute(root, "xmlns:svg", "http://www.w3.org/2000/svg");
	MsvgAddAttribute(root, "xmlns", "http://www.w3.org/2000/svg");

	memmap_entry_t* entry = memoryMap;
	int y, height;

	y = 2; height = 50;

	// Loop through everything
	while(entry != NULL) {
		char buffer[33];
		char buffer2[33];

		// Ideally, the height is representative of the size of this
		height = (entry->end >> 8) - (entry->start >> 8);

		// Limit height of the box
		if(height < 50) {
			height = 50;
		} else if(height > 175) {
			height = 175;
		}

		if(entry->type == kMemMapReserved) {
			height = 50;	
		}

		son = MsvgNewElement(EID_RECT, root);
		MsvgAddAttribute(son, "x", "75");
		MsvgAddAttribute(son, "y", itoa(y, buffer, 10));
		MsvgAddAttribute(son, "width", "225");
		MsvgAddAttribute(son, "height", itoa(height, buffer2, 10));
		MsvgAddAttribute(son, "stroke", "#000");
		MsvgAddAttribute(son, "stroke-width", "1");
		MsvgAddAttribute(son, "fill", (char*) &type_to_colour[entry->type]);

/*		son = MsvgNewElement(EID_RECT, root);
		MsvgAddAttribute(son, "x", "0");
		MsvgAddAttribute(son, "y", itoa(y, buffer, 10));
		MsvgAddAttribute(son, "width", "75");
		MsvgAddAttribute(son, "height", itoa(height, buffer2, 10));
		MsvgAddAttribute(son, "stroke", "#000");*/

		entry = entry->next;

		y += height + 4;
	}


	char buffer[33];
	MsvgAddAttribute(root, "height", itoa(y + 16, buffer, 10));

	if(!MsvgWriteSvgFile(root, argv[2])) {
		perror("Could not write SVG output file\n");
		return -1;
	}

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
	memmap_entry_t *entry;
	memmap_entry_t *firstEntry = NULL;
	memmap_entry_t *lastEntry = NULL;

	char *linePointer; // used by strtok_r
	char *line = strtok_r(file, "\n", &linePointer);

	// Loop through each line
	while(line) {
		entry = malloc(sizeof(memmap_entry_t));
		memset(entry, 0x00, sizeof(memmap_entry_t));

		if(!firstEntry) {
			firstEntry = entry;
		}

		// Ignore comments
		if(line[0] != '#') {
			parse_line(entry, line);
		}

		// Find next line
		line = strtok_r(NULL, "\n", &linePointer);

		if(lastEntry) {
			lastEntry->next = entry;
			printf("0x%X 0x%X\n", lastEntry, entry);
		}

		lastEntry = entry;
	}

	// We're done here
	return firstEntry;
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