#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "common.h"
#include "parse.h"


void network_to_host_header(struct DatabaseHeader *header) {
	header -> magic = ntohl(header -> magic);
	header -> version = ntohs(header -> version);
	header -> count = ntohs(header -> count);
	header -> filesize = ntohl(header -> filesize);
}

void host_to_network_header(struct DatabaseHeader *header) {
	header -> magic = htonl(header -> magic);
	header -> version = htons(header -> version);
	header -> count = htons(header -> count);
	header -> filesize = htonl(header -> filesize);
}

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
	if (fd < 0) {
		printf("Got a bad file-descriptor from the user.\n");
		return STATUS_ERROR;
	}

	struct DatabaseHeader *header = calloc(1, sizeof(struct DatabaseHeader));
	if (header == NULL) {
		printf("Failed to allocate memory for the database header.\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct DatabaseHeader)) != sizeof(struct DatabaseHeader)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	network_to_host_header(header);

	if (header -> magic != HEADER_MAGIC) {
		printf("Improper header magic.\n");
		free(header);
		return STATUS_ERROR;
	}

	if (header -> version != 1) {
		printf("The database is in version 1, while the header is %d.\n", header -> version);
		free(header);
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	if (fstat(fd, &dbstat) == -1) {
		perror("stat");
		return STATUS_ERROR;
	}

	if (header -> filesize != dbstat.st_size) {
		printf("Corrupted database. The size on the header does not match the file size.\n");
		free(header);
		return STATUS_ERROR;
	}

	*header_out = header;

	return STATUS_SUCCESS;
}

int serialize_header(int fd, struct DatabaseHeader *header) {
	if (fd < 0) {
		printf("Got a bad file descriptor from the user.\n");
		return STATUS_ERROR;
	}

	host_to_network_header(header);

	if (lseek(fd, 0, SEEK_SET) != 0) {
		printf("Failed to set the cursor to the beginning of the file.");
		return STATUS_ERROR;
	};	

	if (write(fd, header, sizeof(struct DatabaseHeader)) != sizeof(struct DatabaseHeader)) {
		printf("Failed to write all the bytes in the file");
		return STATUS_ERROR;
	}

	return STATUS_SUCCESS;
}
