#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "parse.h"

int create_database_header(int fd, struct DatabaseHeader **header_out) {

	struct DatabaseHeader *header = calloc(1, sizeof(struct DatabaseHeader));
	if (header == NULL) {
		printf("Failed to allocate memory for the database header.\n");
		return STATUS_ERROR;
	}

        header -> magic = HEADER_MAGIC;
	header -> version = 0x1;
	header -> count = 0;
	header -> filesize = sizeof(struct DatabaseHeader);

	*header_out = header;

	return STATUS_SUCCESS;
}

int validate_database_header(int fd, struct DatabaseHeader **header_out) {
	return 0;
}
