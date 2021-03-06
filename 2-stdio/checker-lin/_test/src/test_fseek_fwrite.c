#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "so_stdio.h"
#include "test_util.h"

#include "hooks.h"

int num_sys_write;
int target_fd;

ssize_t hook_write(int fd, void *buf, size_t len);

struct func_hook hooks[] = {
	[0] = { .name = "write", .addr = (unsigned long)hook_write, .orig_addr = 0 },
};


//this will declare buf[] and buf_len
#include "large_file.h"


ssize_t hook_write(int fd, void *buf, size_t len)
{
	ssize_t (*orig_write)(int, void *, size_t);

	orig_write = (ssize_t (*)(int, void *, size_t))hooks[0].orig_addr;

	if (fd == target_fd)
		num_sys_write++;

	return orig_write(fd, buf, len);
}


int main(int argc, char *argv[])
{
	SO_FILE *f;
	int ret;
	char *test_work_dir;
	char fpath[256];

	install_hooks("libso_stdio.so", hooks, 1);

	if (argc == 2)
		test_work_dir = argv[1];
	else
		test_work_dir = "_test";

	sprintf(fpath, "%s/large_file", test_work_dir);


	/* --- BEGIN TEST --- */
	f = so_fopen(fpath, "w");
	FAIL_IF(!f, "Couldn't open file: %s\n", fpath);

	target_fd = so_fileno(f);

	num_sys_write = 0;

	ret = so_fwrite(buf, 1, 2048, f);
	FAIL_IF(ret != 2048, "Incorrect return value for so_fwrite: got %d, expected %d\n", ret, 2048);
	FAIL_IF(num_sys_write != 0, "Incorrect number of write syscalls: got %d, expected %d\n", num_sys_write, 0);

	ret = so_fflush(f);
	FAIL_IF(ret != 0, "Incorrect return value for so_fflush, got %d, expected %d\n", ret, 0);

	ret = so_fseek(f, 1024, SEEK_SET);
	FAIL_IF(ret != 0, "Incorrect return value for so_fseek: got %d, expected %d\n", ret, 0);

	ret = so_fwrite("AAAAAAAAAAAAAAAA", 1, 16, f);
	FAIL_IF(ret != 16, "Incorrect return value for so_fwrite: got %d, expected %d\n", ret, 16);

	ret = so_fseek(f, 1024, SEEK_SET);
	FAIL_IF(ret != 0, "Incorrect return value for so_fseek: got %d, expected %d\n", ret, 0);

	ret = so_fseek(f, -128, SEEK_CUR);
	FAIL_IF(ret != 0, "Incorrect return value for so_fseek: got %d, expected %d\n", ret, 0);

	ret = so_fwrite("BBBBBBBBBBBBBBBB", 1, 16, f);
	FAIL_IF(ret != 16, "Incorrect return value for so_fwrite: got %d, expected %d\n", ret, 16);

	ret = so_fclose(f);
	FAIL_IF(ret != 0, "Incorrect return value for so_fclose: got %d, expected %d\n", ret, 0);

	// replicate the operations on buf
	memcpy(&buf[1024], "AAAAAAAAAAAAAAAA", 16);
	memcpy(&buf[1024-128], "BBBBBBBBBBBBBBBB", 16);

	FAIL_IF(!compare_file(fpath, buf, 2048), "Incorrect data in file\n");



	return 0;
}
