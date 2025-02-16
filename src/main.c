#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

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

	bool newfile = false;
	char *filepath = NULL;

	int opt;
	while ((opt = getopt(argc, argv, "hnf:")) != -1 ) {
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
			case '?':
				printf("Unknown option");
				break;
			default:
				return -1;
		}
	}

	printf("Newfile: %d\n", newfile);
	printf("Filepath: %s\n", filepath);

	return 0;
}
