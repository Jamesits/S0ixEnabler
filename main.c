#include <stdbool.h>
#include <efi.h>
#include <efilib.h>
#include "listacpi.h"
#include "fadt.h"
#include "acpi_checksum.h"

// Application entrypoint (must be set to 'efi_main' for gnu-efi crt0 compatibility)
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
#if defined(_GNU_EFI)
	InitializeLib(ImageHandle, SystemTable);
#endif

	Print(L"\n%HS0ixEnabler%N\n");
	Print(L"\n%Hhttps://github.com/Jamesits/S0ixEnabler%N\n");
	Print(L"Firmware %s Rev %d\n\n", SystemTable->FirmwareVendor, SystemTable->FirmwareRevision);

	EFI_CONFIGURATION_TABLE* ect = SystemTable->ConfigurationTable;
	EFI_GUID AcpiTableGuid = ACPI_TABLE_GUID;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;
	EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* rsdp = NULL;

	bool foundTable = false;
	bool patchSuccessful = false;
	UINT64 ret = EFI_SUCCESS;

	// locate RSDP (Root System Description Pointer) 
	for (int SystemTableIndex = 0; SystemTableIndex < SystemTable->NumberOfTableEntries; SystemTableIndex++) {
		Print(L"Table #%d/%d: ", SystemTableIndex + 1, SystemTable->NumberOfTableEntries);

		if (!CompareGuid(&SystemTable->ConfigurationTable[SystemTableIndex].VendorGuid, &AcpiTableGuid) && !CompareGuid(&SystemTable->ConfigurationTable[SystemTableIndex].VendorGuid, &Acpi2TableGuid)) {
			Print(L"Not ACPI\n");
			goto next_table;
		}

		if (myStrnCmpA((unsigned char*)"RSD PTR ", (CHAR8*)ect->VendorTable, 8)) {
			Print(L"Not RSDP\n");
			goto next_table;
		} 
		
		rsdp = (EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER*)ect->VendorTable;
		Print(L"RSDP Rev %u @0x%x | ", rsdp->Revision, rsdp);

		//CHAR16 GuidStr[100];
		//Guid2String(GuidStr, &(SystemTable->ConfigurationTable[Index].VendorGuid));
		//Print(L"%s", GuidStr);
		//ParseRSDP(rsdp, GuidStr); // dump tables

		// check if we have XSDT
		if (rsdp->Revision < EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION) {
			Print(L"No XSDT\n");
			goto next_table;
		}

		// validate XSDT signature
		EFI_ACPI_SDT_HEADER* Xsdt = (EFI_ACPI_SDT_HEADER*)(rsdp->XsdtAddress);
		if (myStrnCmpA("XSDT", (CHAR8*)(VOID*)(Xsdt->Signature), 4)) {
			Print(L"Invalid XSDT\n");
			goto next_table;
		}

		// yeah we got XSDT!
		CHAR16 OemStr[20];
		Ascii2UnicodeStr((CHAR8*)(Xsdt->OemId), OemStr, 6);
		UINT32 EntryCount = (Xsdt->Length - sizeof(EFI_ACPI_SDT_HEADER)) / sizeof(UINT64);
		Print(L"%HXSDT OEM ID: %s Tables: %d%N\n", OemStr, EntryCount);

		// iterate XSDT tables
		UINT64* EntryPtr;
		CHAR16 SigStr[20];
		EntryPtr = (UINT64*)(Xsdt + 1);
		for (UINT32 XsdtTableIndex = 0; XsdtTableIndex < EntryCount; XsdtTableIndex++, EntryPtr++) {
			EFI_ACPI_SDT_HEADER* Entry = (EFI_ACPI_SDT_HEADER*)((UINTN)(*EntryPtr));
			Ascii2UnicodeStr((CHAR8*)(Entry->Signature), SigStr, 4);
			Ascii2UnicodeStr((CHAR8*)(Entry->OemId), OemStr, 6);
			Print(L"  ACPI table #%d/%d: %s Rev %d OEM ID: %s\n", XsdtTableIndex + 1, EntryCount, SigStr, (int)(Entry->Revision), OemStr);

			// See Advanced Configuration and Power Interface Specification Version 6.2
			// Table 5-30 DESCRIPTION_HEADER Signatures for tables defined by ACPI
			if (!myStrnCmpA((unsigned char *)"FACP", (CHAR8*)(Entry->Signature), 4))
			{
				// check checksum
				Print(L" %HChecking initial checksum...%N");
				if (AcpiChecksum((UINT8*)(Entry), Entry->Length))
				{
					Print(L"%HFAILED%N\n");
					goto next_table;
				}
				Print(L"%HOK%N\n");
				foundTable = true;

				Print(L"  %HPatching FADT table%N\n");
				EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE* FADT = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE*)Entry;
				Print(L"    Flags before: 0x%x", FADT->Flags);

				// patch flag: set LOW_POWER_S0_IDLE_CAPABLE to 1
				// Table 5-35 Fixed ACPI Description Table Fixed Feature Flags
				FADT->Flags |= 1 << 21;
				Print(L" after: 0x%x\n", FADT->Flags);

				// re-calculate checksum
				Print(L"    Checksum before: 0x%x", Entry->Checksum);
				UINT8 ChkSumDiff = AcpiChecksum((UINT8*)(Entry), Entry->Length);
				Entry->Checksum -= ChkSumDiff;
				Print(L" after: 0x%x", Entry->Checksum);

				// re-check checksum for OCD people
				Print(L" recheck...");
				if (AcpiChecksum((UINT8*)(Entry), Entry->Length))
				{
					Print(L"%HFAILED%N\n");
					ret = EFI_CRC_ERROR;
				} else
				{
					Print(L"%HOK%N\n");
					patchSuccessful = true;
				}

				Print(L"  %HFADT Table patch finished%N\n");
			}
		}

		next_table:
		ect++;
	}

	

	if (rsdp == NULL) {
		Print(L"%EERROR: RSDP is not found%N\n");
		ret = EFI_UNSUPPORTED;
	}

	if (!foundTable)
	{
		Print(L"%EERROR: FADT is not found%N\n");
		ret = EFI_UNSUPPORTED;
	}

	if (!patchSuccessful)
	{
		Print(L"%EERROR: Patch failed%N\n");
		ret = EFI_UNSUPPORTED;
	}

#if defined(_DEBUG)
	Print(L"%ES0ixEnabler done, press any key to continue.%N\n\n");

	// empty the console input buffer
	SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);

	// wait for a key
	UINTN Event;
	SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Event);

	// If running in debug mode, use the EFI shut down call to close QEMU
	Print(L"%EResetting system%N\n\n");
	SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
#else
	// if we are running as an EFI driver, then just quit and let other things load
	Print(L"%ES0ixEnabler done%N\n\n");
#endif

	return ret;
}
