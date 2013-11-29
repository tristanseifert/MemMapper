__MemMapper__ is a tool that converts text memory maps to a pretty SVG that can be embedded in HTML, rendered to a raster image, or just left as-is in its full prettyness. It's actually quite neat!

## File Formats
Input files are simple: Each row represents an entry in the memory map. Rows contain four fields, each separated with a tab (`\t`) character. The first two fields are hexadecimal numbers and indicate the start and end of the segment, the third is a base-10 integer specifying type, and the remaining field is the name of the segment:

	0x00000000	0x07FFFFFF	5	OS Reserved
	0x08000000	0x081FFFFF	2	.text
	0x08200000	0x082FFFFF	1	.rodata
	0x08300000	0x0830FFFF	4	.bss
	0x08310000	0xBDFFFFFF	6	Unmapped
	0xBE000000	0xBEFFFFFF	3	Stack
	0xBF000000	0xBFFFFFFF	7	Kernel/Userspace
	0xC0000000	0xFFFFFFFF	5	Kernel reserved