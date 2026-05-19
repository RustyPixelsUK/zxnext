#ifndef FILEIO_H
#define FILEIO_H

#include <stdint.h>

#define FILEIO_OK              0
#define FILEIO_ERR_NOTFOUND   -1
#define FILEIO_ERR_DENIED     -2
#define FILEIO_ERR_BUSY       -3
#define FILEIO_ERR_NODEV      -4
#define FILEIO_ERR_BADHANDLE  -5
#define FILEIO_ERR_TRUNC      -6
#define FILEIO_ERR_FAIL       -7
#define FILEIO_ERR_BADARG     -8

#define FILEIO_O_READ    0x01
#define FILEIO_O_WRITE   0x02
#define FILEIO_O_OPEN    0x00
#define FILEIO_O_CREATE  0x08
#define FILEIO_O_TRUNC   0x0c

int8_t  fileio_load(const char *path, void *buf, uint16_t buf_max, uint16_t *out_len);
int8_t  fileio_save(const char *path, const void *buf, uint16_t len);
int8_t  fileio_exists(const char *path);
int8_t  fileio_delete(const char *path);

int8_t  fileio_open(const char *path, uint8_t mode);
int8_t  fileio_close(int8_t handle);

int16_t fileio_read(int8_t handle, void *buf, uint16_t n);
int16_t fileio_write(int8_t handle, const void *buf, uint16_t n);

int8_t  fileio_seek(int8_t handle, uint32_t offset);

int32_t fileio_size(int8_t handle);

#endif
