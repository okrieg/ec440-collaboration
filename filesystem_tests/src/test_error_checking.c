#include <stdio.h>
#include <stdlib.h> // abort()
#include <string.h> // memset()
#include "fs.h"

#define TEST_ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, "%s:%d: Assertion (%s) failed!\n", \
				__FILE__, __LINE__, #x); \
	       	abort(); \
	} \
} while(0)

int main() {
	char disk_name[] = "test_disk";
	char not_created_disk_name[] = "not_created";

	char file_name[] = "test_file";
	char bad_file_name[] = "thisnameis2long!";

	char buf1[8192];
	char buf2[8192];

	for (int i = 0; i < 8192; i++)
	{
		buf1[i] = i / 64;
	}

	for (int i = 0; i < 8192; i++)
	{
		buf2[i] = -1;
	}

	printf("Testing if a disk can be made.\n");
	int status = make_fs(disk_name);
	TEST_ASSERT(status == 0);
	printf("Disk can be made.\n\n");

	printf("Testing if a disk can be mounted.\n");
	status = mount_fs(disk_name);
	TEST_ASSERT(status == 0);
	printf("Disk can be mounted.\n\n");

	printf("Testing if a disk can be unmounted.\n");
	status = umount_fs(disk_name);
	TEST_ASSERT(status == 0);
	printf("Disk can be umounted.\n\n");

	printf("Testing if disk can be remounted.\n");
	status = mount_fs(disk_name);
	TEST_ASSERT(status == 0);
	printf("Disk can be remounted.\n\n");

	printf("Testing if mounting a disk that has not been created causes an error.\n");
	status = mount_fs(not_created_disk_name);
	TEST_ASSERT(status == -1);
	printf("Mounting a disk that has not been created causes an error.\n\n");

	printf("Testing if file with a name that is too long results in error.\n");
	status = fs_create(bad_file_name);
	TEST_ASSERT(status == -1);
	printf("File with a name that is too long results in error.\n\n");

	printf("Testing if a file can be created.\n");
	status = fs_create(file_name);
	TEST_ASSERT(status == 0);
	printf("File can be created.\n\n");

	printf("Testing if a file can be opened.\n");
	int fd1 = fs_open(file_name);
	printf("FD1: %d\n", fd1);
	TEST_ASSERT(fd1 >= 0);
	printf("File can be opened.\n\n");

	printf("Testing if the same file can be opened twice.\n");
	int fd2 = fs_open(file_name);
	printf("FD2: %d\n", fd2);
	TEST_ASSERT(fd2 >= 0);
	printf("File can be opened.\n\n");

	printf("Testing that deleting a file with a valid fd results in error.\n");
	status = fs_delete(file_name);
	TEST_ASSERT(status == -1);
	printf("Deleting a file with a valid fd results in error.\n\n");

	printf("Testing is a file can be closed.\n");
	status = fs_close(fd1);
	TEST_ASSERT(status == 0);
	printf("File can be closed.\n\n");

	printf("Testing is a file can be closed.\n");
	status = fs_close(fd2);
	TEST_ASSERT(status == 0);
	printf("File can be closed.\n\n");

	printf("Testing if reading from a closed fd results in an error.\n");
	status = fs_read(fd1, &buf1, 1);
	TEST_ASSERT(status == -1);
	printf("Reading from a closed fd results in an error.\n\n");

	printf("Testing if reading from a closed fd results in an error.\n");
	status = fs_read(fd2, &buf2, 1);
	TEST_ASSERT(status == -1);
	printf("Reading from a closed fd results in an error.\n\n");

	printf("Testing if a file can be deleted.\n");
	status = fs_delete(file_name);
	TEST_ASSERT(status == 0);
	printf("File can be deleted.\n\n");

	printf("Testing if deleting a file that has been deleted results in error.\n");
	status = fs_delete(file_name);
	TEST_ASSERT(status == -1);
	printf("Deleting a file that has been deleted results in error.\n\n");

	printf("Done with error testing.\n");

	return 0;
}

