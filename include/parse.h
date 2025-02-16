#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

#include <stdbool.h>

struct DatabaseHeader {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
};

struct Trade {
	char ticker[10];
	char date[10];
	float amount;
	float price;
	bool buy_side;
};

int create_database_header(int fd, struct DatabaseHeader **header_out);
int validate_database_header(int fd, struct DatabaseHeader **header_out);
int serialize_header(int fd, struct DatabaseHeader *header);
int read_trades(int fd, struct DatabaseHeader *, struct Trade **trades_out);

#endif
