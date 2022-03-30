#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "so_stdio.h"

#define SO_BUFFER_SIZE 4096
#define SO_READ_OP 1
#define SO_WRITE_OP 2

#define SO_NO_ERR 0
#define SO_ERR 1

struct _so_file {
	int fd;
	int flags;
	int oflags;
	unsigned char buffer[SO_BUFFER_SIZE];
	int lastOp;
	long offset;
	int hasErr;
	int bufferPtr;
	int bufferSize;
} _so_file;

FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	int fd;
	int flags;
	int oflags;
	SO_FILE * f = malloc(sizeof(SO_FILE));

	if (f == NULL) {
		return NULL;
	}
	if (strcmp(mode, "r") == 0) {
		flags = 1;
		oflags = O_RDONLY;
	} else if (strcmp(mode, "r+") == 0) {
		flags = 1;
		oflags = O_RDWR;
	} else if (strcmp(mode, "w") == 0) {
		flags = 3;
		oflags = O_WRONLY | O_CREAT | O_TRUNC;
	} else if (strcmp(mode, "w+") == 0) {
		flags = 3;
		oflags = O_RDWR | O_CREAT | O_TRUNC;
	} else if (strcmp(mode, "a") == 0) {
		flags = 2;
		oflags = O_APPEND | O_WRONLY;
	} else if (strcmp(mode, "a+") == 0) {
		flags = 2;
		oflags = O_APPEND | O_RDWR;
	} else {
		free(f);
		return NULL;
	}

	if (mode[0] == 'w')
		fd = open(pathname, oflags, 0644);
	else
		fd = open(pathname, oflags);
	if (fd < 0) {
		free(f);
		return NULL;
	}

	f->fd = fd;
	f->flags = flags;
	f->oflags = oflags;
	f->lastOp = 0;
	f->hasErr = SO_NO_ERR;
	f->bufferPtr = 0;
	f->bufferSize = 0;

	if (flags == 1)
		f->offset = lseek(fd, 0, SEEK_CUR);
	else if (flags == 2)
		f->offset = lseek(fd, 0, SEEK_END);
	else
		f->offset = 0;

	if (f->offset < 0) {
		free(f);
		return NULL;
	}

	memset(f->buffer, 0, SO_BUFFER_SIZE);

	return f;
}

FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream)
{
	if (stream == NULL)
		return SO_EOF;

	if (stream->lastOp == SO_WRITE_OP) {
		if (so_fflush(stream) < 0) {
			free(stream);
			return SO_EOF;
		}
	}

	int retStatus = close(stream->fd);

	free(stream);

	if (retStatus < 0)
		return SO_EOF;

	return 0;
}

FUNC_DECL_PREFIX int so_fileno(SO_FILE *stream)
{
	return stream->fd;
	stream->lastOp = 0;
}

FUNC_DECL_PREFIX int so_fflush(SO_FILE *stream)
{
	int i;
	int retStatus;

	if (stream->bufferSize == 0)
		return 0;

	retStatus = write(stream->fd, &stream->buffer, stream->bufferSize);
	if (retStatus < 1) {
		stream->hasErr = SO_ERR;
		return SO_EOF;
	}

	stream->lastOp = 0;
	stream->bufferSize = 0;
	stream->bufferPtr = 0;

	return 0;

}

FUNC_DECL_PREFIX int so_fseek(SO_FILE *stream, long offset, int whence)
{
	long newOffset;
	int retStatus;

	if (stream->lastOp == SO_WRITE_OP) {
		retStatus = so_fflush(stream);
		if (retStatus < 0) {
			stream->hasErr = SO_ERR;
			return SO_EOF;
		}
	}

	newOffset = lseek(stream->fd, offset, whence);
	if (newOffset < 0) {
		stream->hasErr = SO_ERR;
		return SO_EOF;
	}

	stream->offset = newOffset;
	stream->lastOp = 0;



	return 0;
}

long so_ftell(SO_FILE *stream)
{
	return stream->offset;
}

FUNC_DECL_PREFIX
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i;
	int retStatus;
	int count = nmemb * size;

	memset(ptr, 0, count);

	for (i = 0; i < count; i++) {
		if (stream->bufferSize < SO_BUFFER_SIZE && stream->lastOp == SO_READ_OP) {
			if (stream->bufferPtr < stream->bufferSize) {
				stream->bufferPtr++;
				((unsigned char *)ptr)[i] = stream->buffer[stream->bufferPtr];
				stream->lastOp = SO_READ_OP;
			} else
				break;
		} else
			((unsigned char *)ptr)[i] = (unsigned char) so_fgetc(stream) & 0xFFFF;
	}

	return count / size;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int i;
	int retStatus;
	long count = size * nmemb;

	for (i = 0; i < count; i++)
		retStatus = so_fputc(((char *)ptr)[i], stream);

	stream->lastOp = SO_WRITE_OP;

	return count / size;
}

FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream)
{
	int retStatus;
	unsigned char *buffer = stream->buffer;

	if (stream->lastOp == SO_WRITE_OP) {
		retStatus = so_fflush(stream);
		if (retStatus < 0) {
			stream->hasErr = SO_ERR;
			return SO_EOF;
		}
	}


	if (stream->bufferPtr == SO_BUFFER_SIZE - 1 || stream->lastOp != SO_READ_OP) {
		memset(buffer, 0, SO_BUFFER_SIZE);
		retStatus = read(stream->fd, buffer, SO_BUFFER_SIZE);
		if (retStatus < 0) {
			stream->hasErr = SO_ERR;
			stream->offset += 1;
			return SO_EOF;
		}
		stream->bufferSize = retStatus;
		stream->bufferPtr = -1;
	}

	stream->offset += 1;
	stream->lastOp = SO_READ_OP;
	stream->bufferPtr += 1;

	if (stream->bufferSize == 0)
		return SO_EOF;

	if (stream->bufferSize == stream->bufferPtr)
		return SO_EOF;


	return buffer[stream->bufferPtr];
}

FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream)
{
	int retStatus;
	char *buffer = stream->buffer;

	if (stream->lastOp != SO_WRITE_OP) {
		stream->bufferPtr = -1;
		stream->bufferSize = 0;
		memset(stream->buffer, 0, SO_BUFFER_SIZE);
	}

	if (stream->bufferSize == SO_BUFFER_SIZE) {
		retStatus = so_fflush(stream);
		stream->bufferPtr = -1;
		stream->bufferSize = 0;
		memset(stream->buffer, 0, SO_BUFFER_SIZE);
		if (retStatus < 0) {
			stream->hasErr = SO_ERR;
			return SO_EOF;
		}
	}

	stream->bufferPtr++;
	stream->bufferSize++;
	stream->offset++;


	buffer[stream->bufferPtr] = (char) c;

	stream->lastOp = SO_WRITE_OP;

	return c;
}

FUNC_DECL_PREFIX int so_feof(SO_FILE *stream)
{
	if (stream->offset == lseek(stream->fd, 0, SEEK_END) + 1)
		return SO_EOF;
	lseek(stream->fd, stream->offset, SEEK_SET);
	return 0;
}

FUNC_DECL_PREFIX int so_ferror(SO_FILE *stream)
{
	return stream->hasErr;
}

FUNC_DECL_PREFIX SO_FILE *so_popen(const char *command, const char *type)
{
	return NULL;
}
FUNC_DECL_PREFIX int so_pclose(SO_FILE *stream)
{
	return 0;
}
