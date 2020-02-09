#include <efi.h>
#include <efilib.h>
#include "acpi_checksum.h"

// return 0 if checksum passes
UINT8 AcpiChecksum(UINT8 *TablePtr, UINT32 size)
{
	UINT8 ChkSum = 0;
	for (UINT32 i = 0; i < size; ++i, ++TablePtr)
	{
		ChkSum += (*TablePtr);
	}

	return ChkSum;
}