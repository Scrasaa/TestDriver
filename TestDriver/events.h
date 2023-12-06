#pragma once

#include <ntifs.h>

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessID, PIMAGE_INFO ImageInfo);