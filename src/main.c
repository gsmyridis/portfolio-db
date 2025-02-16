#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

#include "common.h"
#include "file.h"
#include "parse.h"


void print_usage() {
	printf("\nUSAGE:\n");
	printf("\t-n\t   Creates a new database file.\n");
	printf("\t-f <PATH>  Loads an existing database file.\n");
	printf("\n");
}


void print_help() {
	printf("\nWelcome to Folio DB.\n");
	printf("\nFolio DB is a database system that helps you track you investment moves and focuses on simplicity.\n");
	print_usage();
}


int main(int argc, char *argv[]) {

	int fd = -1;
	bool newfile = false;
	char *filepath = NULL;
	struct DatabaseHeader *header = NULL;
	struct Trade *trades = NULL;

	int opt;
	while ((opt = getopt(argc, argv, "hnf:a:")) != -1 ) {
		switch (opt) {
			case 'h':
				print_help();
				return 0;
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				printf("Adding %s\n", optarg);
				break;
			case '?':
				printf("Unknown option");
				break;
			default:
				return -1;
		}
	}

	if (newfile) {
		int fd = create_database_file(DEFAULT_DB_FILENAME);
		if (fd == STATUS_ERROR) {
			printf("Unable to create database file.\n");
			return STATUS_ERROR;
		}

		if (create_database_header(fd, &header) == STATUS_ERROR) {
			printf("Failed to create database header.\n");
			return STATUS_ERROR;
		}

		if (serialize_header(fd, header) == STATUS_ERROR) {
			printf("Failed to serialize header to file.\n");
			return STATUS_ERROR;
		}

		printf("Created new database file '%s'.\n", DEFAULT_DB_FILENAME);

	} else {
		int fd = open_database_file(filepath);
		if (fd == STATUS_ERROR) {
			printf("Unable to open database file.\n");
			return STATUS_ERROR;
		}

		if (validate_database_header(fd, &header) == STATUS_ERROR) {
			printf("Header validation failed.\n");
			return STATUS_ERROR;
		};

		if (read_trades(fd, header, &trades) == STATUS_ERROR) {
			printf("Failed to read trades from file.\n");
			return STATUS_ERROR;
		}
	}

	close(fd);
	return STATUS_SUCCESS;
}
