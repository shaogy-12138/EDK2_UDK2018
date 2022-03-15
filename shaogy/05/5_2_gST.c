/*************************************************************************
	> File Name: 5_1_UseSt.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Sat 12 Mar 2022 11:12:52 AM CST
 ************************************************************************/

#include<Uefi.h>
#include<Library/UefiBootServicesTableLib.h>


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


    gBS->WaitForEvent(
        1,
        &gST->ConIn->WaitForKey,
        &Index
        );

    Status = gST->ConIn->ReadKeyStroke(
        gST->ConIn,
        &Key
        );

    StrBuffer[0] = Key.UnicodeChar;
    StrBuffer[1] = '\n';

    gST->ConOut->OutputString(gST->ConOut, StrBuffer);


    return EFI_SUCCESS;
}
