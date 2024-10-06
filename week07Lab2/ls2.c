/* This still very simple ls with lstat and stat calls
   for the file information in a directory.

 */

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char **argv) {

	DIR *dp;
	struct dirent *dirp;
	int regCount = 0;
	int dirCount = 0;
	int charCount = 0;
	int blockCount = 0;
	int fifoCount = 0;
	int symlCount = 0;
	int socCount = 0;
	int otherCount = 0;
	int totalCount = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s dir_name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((dp = opendir(argv[1])) == NULL ) {
		fprintf(stderr, "can't open '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (chdir(argv[1]) == -1) {
		fprintf(stderr, "can't chdir to '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	while ((dirp = readdir(dp)) != NULL ) {
		totalCount++;

		struct stat sb;
		if(stat(dirp->d_name, &sb) == -1){
			fprintf(stderr, "not for stat %s: %s\n", dirp->d_name, strerror(errno));
		}
		if (lstat(dirp->d_name, &sb) == -1) {
			fprintf(stderr,"not for stat %s: %s\n", dirp->d_name,
						strerror(errno));
			continue;
		}
		//printf("%s: ", dirp->d_name);
		if (S_ISREG(sb.st_mode))
			regCount++;
		else if (S_ISDIR(sb.st_mode))
			dirCount++;
		else if (S_ISCHR(sb.st_mode))
			charCount++;
		else if (S_ISBLK(sb.st_mode))
			blockCount++;
		else if (S_ISFIFO(sb.st_mode))
			fifoCount++;
		else if (S_ISLNK(sb.st_mode))
			symlCount++;
		else if (S_ISSOCK(sb.st_mode))
			socCount++;
		else
			otherCount++;

	}
	printf(" Reading the directory: %s\n", argv[1]);
	printf(" Total Count of Regular File: %d\n", regCount);
	printf(" Total Count of Directory: %d\n", dirCount);
	printf(" Total Count of Character Special: %d\n", charCount);
	printf(" Total Count of Block Special: %d\n", blockCount);
	printf(" Total Count of Symbolick Link: %d\n", symlCount);
	printf(" Total Count of Socket: %d\n", socCount);
	printf(" Total Count of Other Files: %d\n", otherCount);
	printf(" Total Count of Directory Entries Processed: %d\n", totalCount);
	closedir(dp);
	exit(EXIT_SUCCESS);
}
