#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>

#include "common.h"
#include "parse.h"


int parse_trade(char *s, struct Trade *trade) {
	char *ticker = strtok(s, DELIMETER);
	char *date = strtok(NULL, DELIMETER);
	char *amount = strtok(NULL, DELIMETER);
	char *price = strtok(NULL, DELIMETER);
	char *side = strtok(NULL, DELIMETER);

	strncpy(trade -> ticker, ticker, sizeof(trade -> ticker));
	strncpy(trade -> date, date, sizeof(trade -> date));
	trade -> amount = atoi(amount);
	trade -> price = atoi(price);
	strncpy(trade -> side, side, sizeof(trade -> side));

	return STATUS_SUCCESS;
}

void print_trade(struct Trade *trade) {
	printf("%s %s %x %x %s\n", 
		trade -> ticker, 
		trade -> date, 
		trade -> amount, 
		trade -> price, 
		trade -> side
	);
	return;
}

int read_trades(int fd, struct DatabaseHeader *header, struct Trade **trades_out) {
	if (fd < 0) {
		printf("ERROR: Got a bad file-descriptor from the user.\n");
		return STATUS_ERROR;
	}

	int count = header -> count;
	struct Trade *trades = calloc(count, sizeof(struct Trade));
	if (trades == NULL) {
		printf("ERROR: Failed to allocate memory for the trades.\n");
		return STATUS_ERROR;
	}

	ssize_t trades_bytes = count * sizeof(struct Trade);
	if (read(fd, trades, trades_bytes) < trades_bytes) {
		printf("ERROR: Failed to read all trades.\n");
		return STATUS_ERROR;
	}

	// Convert from network to host
	for (int i = 0; i < count; i++) {
		trades[i].amount = ntohl(trades[i].amount);
		trades[i].price = ntohl(trades[i].price);
	}


	*trades_out = trades;

	return STATUS_SUCCESS;
	
}

int create_database_header(int fd, struct DatabaseHeader **header_out) {
	struct DatabaseHeader *header = calloc(1, sizeof(struct DatabaseHeader));
	if (header == NULL) {
		printf("ERROR: Failed to allocate memory for the database header.\n");
		return STATUS_ERROR;
	}

        header -> magic = HEADER_MAGIC;
	header -> version = 0x1;
	header -> count = 0x0;
	header -> filesize = sizeof(struct DatabaseHeader);

	*header_out = header;

	return STATUS_SUCCESS;
}

int load_database_header(int fd, struct DatabaseHeader **header_out) {
	if (fd < 0) {
		printf("ERROR: Got a bad file-descriptor from the user.\n");
		return STATUS_ERROR;
	}

	struct DatabaseHeader *header = calloc(1, sizeof(struct DatabaseHeader));
	if (header == NULL) {
		printf("ERROR: Failed to allocate memory for the database header.\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct DatabaseHeader)) != sizeof(struct DatabaseHeader)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	// Convert from network to host endianness
	header -> magic = ntohl(header -> magic);
	header -> version = ntohs(header -> version);
	header -> count = ntohs(header -> count);
	header -> filesize = ntohl(header -> filesize);

	if (header -> magic != HEADER_MAGIC) {
		printf("ERROR: Improper header magic.\n");
		free(header);
		return STATUS_ERROR;
	}

	if (header -> version != 1) {
		printf("ERROR: The database is in version 1, while the header is %d.\n", header -> version);
		free(header);
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	if (fstat(fd, &dbstat) == -1) {
		perror("stat");
		return STATUS_ERROR;
	}

	if (header -> filesize != dbstat.st_size) {
		printf("ERROR: Corrupted database. The size on the header '%x' does not match the file size '%llx'.\n", header -> filesize, dbstat.st_size);
		free(header);
		return STATUS_ERROR;
	}

	*header_out = header;

	return STATUS_SUCCESS;
}

int serialize_database(int fd, struct DatabaseHeader *header, struct Trade *trades) {
	if (fd < 0) {
		printf("ERROR: Got a bad file descriptor from the user.\n");
		return STATUS_ERROR;
	}
	// Store count before converting to network endianness
	ssize_t count = header -> count; 

	// Convert to network endianness
	header -> magic = htonl(header -> magic);
	header -> version = htons(header -> version);
	header -> count = htons(header -> count);
	header -> filesize = htonl(header -> filesize);

	if (lseek(fd, 0, SEEK_SET) != 0) {
		printf("ERROR: Failed to set the cursor to the beginning of the file.");
		return STATUS_ERROR;
	};	

	if (write(fd, header, sizeof(struct DatabaseHeader)) != sizeof(struct DatabaseHeader)) {
		printf("ERROR: Failed to write all the bytes in the file");
		return STATUS_ERROR;
	}

	for (int i = 0; i < count ; i++) {
		trades[i].amount = htonl(trades[i].amount);
		trades[i].price = htonl(trades[i].price);
		if(write(fd, &trades[i], sizeof(struct Trade)) < sizeof(struct Trade)) {
			printf("ERROR: Failed to write all trades into file.\n");
			return STATUS_ERROR;
		}
	}

	return STATUS_SUCCESS;
}

