/*************************************************************************
	> File Name: 5_1_UseSt.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Sat 12 Mar 2022 11:12:52 AM CST
 ************************************************************************/

#include<Uefi.h>

EFI_STATUS 
EFIAPI
UefiMain(
    IN      EFI_HANDLE      ImageHandle,
    IN      EFI_SYSTEM_TABLE *SystemTable
    )
{

    EFI_STATUS      Status;
    UINTN           Index;
    EFI_INPUT_KEY   Key;
    CHAR16          StrBuffer[3] = {0};


    SystemTable->BootServices->WaitForEvent(
        1,
        &SystemTable->ConIn->WaitForKey,
        &Index
        );

    Status = SystemTable->ConIn->ReadKeyStroke(
        SystemTable->ConIn,
        &Key
        );

    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = '\n';

    SystemTable->ConOut->OutputString(SystemTable->ConOut, StrBuffer);


    return EFI_SUCCESS;
}
