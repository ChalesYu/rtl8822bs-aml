#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"

wf_file *wf_os_api_file_open(const char *path)
{
	NTSTATUS Status = STATUS_SUCCESS;
	//HANDLE SourceFileHandle = NULL;
	OBJECT_ATTRIBUTES ObjectAttributes;
	//IO_STATUS_BLOCK IoStatusBlock;
	wf_file *file = NULL;
	UNICODE_STRING SourceFilePath;

	RtlInitUnicodeString(&SourceFilePath, (PCWSTR)path);

	file = wf_malloc(sizeof(wf_file));
	if (file == NULL) {
		LOG_E("Create file node fail!!\n");
		return NULL;
	}

	InitializeObjectAttributes(
		&ObjectAttributes,
		&SourceFilePath,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);
	Status = ZwCreateFile(
		&file->handle, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,
		&ObjectAttributes, &file->io_status, NULL,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_RANDOM_ACCESS | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);
	if (!NT_SUCCESS(Status))
	{
		LOG_E("Open source file fault !! - %#x\n", Status);
		wf_free(file);
		return NULL;
	}

	return file;
}

int wf_os_api_file_read(wf_file *file, loff_t offset, unsigned char *data, unsigned int size)
{
	NTSTATUS Status = STATUS_SUCCESS;
	LARGE_INTEGER  offs = { 0 };

	offs.QuadPart = offset;

	Status = ZwReadFile(
		file->handle, NULL, NULL, NULL,
		&file->io_status, data, size, &offs, NULL);
	if (!NT_SUCCESS(Status))
	{
		LOG_E("read file fail!!\n - %#X", Status);
		ZwClose(file->handle);
		return Status;
	}

	return Status;
}

void wf_os_api_file_close(wf_file *file)
{
	ZwClose(file->handle);
	wf_free(file);
}
