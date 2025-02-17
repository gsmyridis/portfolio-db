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
	char ticker[11];
	char date[11];
	int amount;
	int price;
	char side[5];
};

void print_trade(struct Trade *trade);
int parse_trade(char *s, struct Trade *trade);
int create_database_header(int fd, struct DatabaseHeader **header_out);
int validate_database_header(int fd, struct DatabaseHeader **header_out);
int serialize_database(int fd, struct DatabaseHeader *header, struct Trade *trades);
int read_trades(int fd, struct DatabaseHeader *header, struct Trade **trades_out);

#endif
