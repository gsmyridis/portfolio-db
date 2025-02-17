#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "file.h"
#include "parse.h"


void print_usage() {
	printf("\nUSAGE:\n");
	printf("\t-n\t   Creates a new database file.\n");
	printf("\t-f <PATH>  Loads an existing database file.\n");
	printf("\n");
	return;
}

int print_help() {
	printf("\nWelcome to Folio DB.\n");
	printf("\nFolio DB is a database system that helps you track you investment moves and focuses on simplicity.\n");
	print_usage();
	return STATUS_SUCCESS;
}

int load_database(char *filepath, struct DatabaseHeader **header_out, struct Trade **trades_out) {
	int fd = open_database_file(filepath);
	if (fd == STATUS_ERROR) {
		printf("ERROR: Failed to open database file");
		return STATUS_ERROR;
	}

	if (load_database_header(fd, header_out) == STATUS_ERROR) {
		printf("ERROR: Failed to load database header.\n");
		return STATUS_ERROR;
	}
	
	if (read_trades(fd, *header_out, trades_out) == STATUS_ERROR) {
		printf("ERROR: Failed to read trades from file.\n");
		return STATUS_ERROR;
	}
	
	return fd;
}

int create_new_database(char *path) {
	struct DatabaseHeader *header = NULL;
	int fd = create_database_file(path);
	if (fd == STATUS_ERROR) {
		printf("ERROR: Failed to create database file.\n");
	 	return STATUS_ERROR;
	}

	if (create_database_header(fd, &header) == STATUS_ERROR) {
		printf("ERROR: Failed to create database header.\n");
		return STATUS_ERROR;
	}

	if (serialize_database(fd, header, NULL) == STATUS_ERROR) {
		printf("ERROR: Failed to serialize header to file.\n");
		return STATUS_ERROR;
	}

	printf("Created new database file '%s'.\n", DEFAULT_DB_FILENAME);
	close(fd);
	return STATUS_SUCCESS;
}

int database_show(char *filepath) {
	struct DatabaseHeader *header = NULL;
	struct Trade *trades = NULL;
	if (load_database(filepath, &header, &trades) == STATUS_ERROR) {
		printf("ERROR: Failed to load database.\n");
		return STATUS_ERROR;
	}

	printf("COUNT: %d\n", header -> count);
	for (int i = 0; i < header -> count; i++) {
		print_trade(&trades[i]);
	}

	return STATUS_SUCCESS;
}

int count_entries(char *filepath) {
	struct DatabaseHeader *header = NULL;
	if (load_database(filepath, &header, NULL) == STATUS_ERROR) {
		printf("ERROR: Failed to load database.\n");
		return STATUS_ERROR;
	}
	printf("COUNT: %d\n", header -> count);
	return STATUS_SUCCESS;
}

int add_entry_to_database(char *filepath, char *entry_string) {

	struct Trade *trades = NULL;
	struct DatabaseHeader *header = NULL;

	struct Trade trade = {0};
	parse_trade(entry_string, &trade);

	int fd = load_database(filepath, &header, &trades);
	if (fd == STATUS_ERROR) {
		printf("ERROR: Failed to load database.\n");
		return STATUS_ERROR;
	}

	header -> count++;
	trades = realloc(trades, sizeof(struct Trade) * (header -> count));
	if (trades == NULL) {
		printf("ERROR: Failed to reallocate space for trades.\n");
		return STATUS_ERROR;
	}
	
	strncpy(trades[header -> count - 1].ticker, trade.ticker, sizeof(trade.ticker));
	strncpy(trades[header -> count - 1].date, trade.date, sizeof(trade.date));
	trades[header -> count - 1].amount = trade.amount;
	trades[header -> count - 1].price = trade.price;
	strncpy(trades[header -> count - 1].side, trade.side, sizeof(trade.side));

	header -> filesize += sizeof(struct Trade);

	if (serialize_database(fd, header, trades) == STATUS_ERROR) {
		printf("ERROR: Failed to serialize databse to file.\n");
		return STATUS_ERROR;
	}

	close(fd);
	return STATUS_SUCCESS;
}


int main(int argc, char *argv[]) {

	char *filepath = NULL;

	int opt;
	while ((opt = getopt(argc, argv, "hnf:ca:s")) != -1 ) {
		switch (opt) {
			case 'h':
				return print_help();
			case 'n':
				return create_new_database(DEFAULT_DB_FILENAME);
			case 'f':
				filepath = optarg;
				break;
			case 'c':
				return count_entries(filepath);
			case 'a':
				return add_entry_to_database(filepath, optarg);
			case 's':
				return database_show(filepath);
			case '?':
				printf("Unknown option");
				break;
			default:
				return STATUS_ERROR;
		}
	}

	return STATUS_SUCCESS;
}
