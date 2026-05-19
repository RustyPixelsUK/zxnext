#include "fileio.h"

#include <errno.h>
#include <arch/zxn/esxdos.h>

static int8_t map_errno(void)
{
	switch (errno)
	{
	case 0:
		return FILEIO_OK;
	case ESX_ENOENT:
		return FILEIO_ERR_NOTFOUND;
	case ESX_EACCES:
	case ESX_ERDONLY:
		return FILEIO_ERR_DENIED;
	case ESX_EINUSE:
	case ESX_EDRIVEBUSY:
	case ESX_EDEVICEBUSY:
		return FILEIO_ERR_BUSY;
	case ESX_ENODEV:
	case ESX_ENODRV:
		return FILEIO_ERR_NODEV;
	case ESX_EBADF:
		return FILEIO_ERR_BADHANDLE;
	case ESX_EINVAL:
	case ESX_ENAMETOOLONG:
		return FILEIO_ERR_BADARG;
	default:
		return FILEIO_ERR_FAIL;
	}
}

static uint8_t map_mode(uint8_t mode)
{
	uint8_t out = 0;
	if (mode & FILEIO_O_READ)
		out |= ESX_MODE_R;
	if (mode & FILEIO_O_WRITE)
		out |= ESX_MODE_W;
	if (mode & FILEIO_O_TRUNC)
		out |= ESX_MODE_OPEN_CREAT_TRUNC;
	else if (mode & FILEIO_O_CREATE)
		out |= ESX_MODE_OPEN_CREAT;
	else
		out |= ESX_MODE_OPEN_EXIST;
	return out;
}

int8_t fileio_open(const char *path, uint8_t mode)
{
	if (!path)
		return FILEIO_ERR_BADARG;
	errno = 0;
	uint8_t h = esx_f_open(path, map_mode(mode));
	if (errno)
		return map_errno();
	return (int8_t)h;
}

int8_t fileio_close(int8_t handle)
{
	if (handle < 0)
		return FILEIO_ERR_BADHANDLE;
	errno = 0;
	esx_f_close((uint8_t)handle);
	return errno ? map_errno() : FILEIO_OK;
}

int16_t fileio_read(int8_t handle, void *buf, uint16_t n)
{
	if (handle < 0 || !buf)
		return (int16_t)FILEIO_ERR_BADARG;
	errno = 0;
	uint16_t got = esx_f_read((uint8_t)handle, buf, n);
	if (errno)
		return (int16_t)map_errno();
	return (int16_t)got;
}

int16_t fileio_write(int8_t handle, const void *buf, uint16_t n)
{
	if (handle < 0 || !buf)
		return (int16_t)FILEIO_ERR_BADARG;
	errno = 0;

	uint16_t put = esx_f_write((uint8_t)handle, (void *)buf, n);
	if (errno)
		return (int16_t)map_errno();
	return (int16_t)put;
}

int8_t fileio_seek(int8_t handle, uint32_t offset)
{
	if (handle < 0)
		return FILEIO_ERR_BADHANDLE;
	errno = 0;
	esx_f_seek((uint8_t)handle, offset, ESX_SEEK_SET);
	return errno ? map_errno() : FILEIO_OK;
}

int32_t fileio_size(int8_t handle)
{
	if (handle < 0)
		return (int32_t)FILEIO_ERR_BADHANDLE;
	struct esx_stat st;
	errno = 0;
	esx_f_fstat((uint8_t)handle, &st);
	if (errno)
		return (int32_t)map_errno();
	return (int32_t)st.size;
}

int8_t fileio_exists(const char *path)
{
	if (!path)
		return FILEIO_ERR_BADARG;
	struct esx_stat st;
	errno = 0;
	esx_f_stat(path, &st);
	if (errno == 0)
		return 1;
	if (errno == ESX_ENOENT)
		return 0;
	return map_errno();
}

int8_t fileio_delete(const char *path)
{
	if (!path)
		return FILEIO_ERR_BADARG;
	errno = 0;
	esx_f_unlink(path);
	if (errno == 0)
		return FILEIO_OK;
	if (errno == ESX_ENOENT)
		return FILEIO_OK;
	return map_errno();
}

int8_t fileio_load(const char *path, void *buf, uint16_t buf_max, uint16_t *out_len)
{
	if (!path || !buf)
		return FILEIO_ERR_BADARG;
	int8_t h = fileio_open(path, FILEIO_O_READ);
	if (h < 0)
		return h;

	int32_t sz = fileio_size(h);
	if (sz < 0)
	{
		fileio_close(h);
		return (int8_t)sz;
	}
	if ((uint32_t)sz > (uint32_t)buf_max)
	{
		fileio_close(h);
		return FILEIO_ERR_TRUNC;
	}

	int16_t got = fileio_read(h, buf, (uint16_t)sz);
	int8_t cr = fileio_close(h);
	if (got < 0)
		return (int8_t)got;
	if (out_len)
		*out_len = (uint16_t)got;
	return cr;
}

int8_t fileio_save(const char *path, const void *buf, uint16_t len)
{
	if (!path || (!buf && len))
		return FILEIO_ERR_BADARG;
	int8_t h = fileio_open(path, FILEIO_O_WRITE | FILEIO_O_TRUNC);
	if (h < 0)
		return h;

	int16_t put = (len == 0) ? 0 : fileio_write(h, buf, len);
	int8_t cr = fileio_close(h);
	if (put < 0)
		return (int8_t)put;
	if ((uint16_t)put != len)
		return FILEIO_ERR_FAIL;
	return cr;
}
