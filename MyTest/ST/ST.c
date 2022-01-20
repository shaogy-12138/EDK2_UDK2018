#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable 
    )
{
    EFI_STATUS      Status;
    UINTN           Index;
    EFI_INPUT_KEY   Key;
    CHAR16 StrBuffer[8] = {0};
    SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &Index);
    Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
    
    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = 's';
    StrBuffer[2] = 'g';
    StrBuffer[3] = 'y';
    StrBuffer[4] = '\n';
    gST->ConOut->OutputString(SystemTable->ConOut, StrBuffer);


    return EFI_SUCCESS;
}
