#include<stdio.h>
#include<fcntl.h>

#include "file.h"
#include "common.h"


int create_database_file(char *path) {
	if (open(path, O_RDONLY) != -1) {
		printf("ERROR: File %s already exists.\n", path);
		return STATUS_ERROR;
	}

	int fd = open(path, O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}
	return fd;
}


int open_database_file(char *path) {
	int fd = open(path, O_RDWR);
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}
	return fd;

}
