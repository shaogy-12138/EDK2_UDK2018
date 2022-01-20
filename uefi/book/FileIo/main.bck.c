#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Uutil.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <MainData.h>
#include <unistd.h>



#include <Library/IoLib.h> 



INTN EFIAPI InitStdlib(
    IN UINTN Argc,
    IN CHAR16 **Argv
){
    struct __filedes     *mfd;
    INTN ExitVal;
    int i;
    ExitVal = (INTN)RETURN_SUCCESS;
    
    gMD = AllocateZeroPool(sizeof(struct __MainData));
    if( gMD == NULL ){
        ExitVal = (INTN)RETURN_OUT_OF_RESOURCES;
    }
    else{
        extern int __sse2_available;
        __sse2_available = 0;
        _fltused = 1;
        errno = 0;
        EFIerrno = 0;
        gMD->ClocksPerSecond = 1;
        gMD->AppStartTime = (clocl_t)((UINT32)time(NULL));

        mfd = gMD->fdarray;
        for(i = 0; i < (FOPEN_MAX); ++i){
            mfd[i].MyFD = (UINT16)i;    
        }

        i = open("stdin:", O_RDONLY, 0444);
        if(i == 0){
            i = open("stdin:", O_WRONLY, 0222);
            if(i == 1){
                i = open("stderr:", O_WRONLY, 0222);
            }
        }
    }
    return ExitVal;
}


void EFIAPI DestoryStdLib(){
    
    if( gMD != NULL ){
        FreePool( gMD );
    }
}



EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE	      ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
) {
    UINT8 i = 0;
    UINT8 readmac00 = 0;
    UINT8 readmac01 = 0;
    UINT8 readmac02 = 0;
    UINT8 readmac03 = 0;
    UINT8 readmac04 = 0;
    UINT8 readmac05 = 0;
    unsigned char recvbuf[32];
    EFI_STATUS Status = 0;
    Status = InitStdlib();
    printf("hello world\n");
    readmac00 = IoRead8(0x4000);
    readmac01 = IoRead8(0x4001);
    readmac02 = IoRead8(0x4002);
    readmac03 = IoRead8(0x4003);
    readmac04 = IoRead8(0x4004);
    readmac05 = IoRead8(0x4005);
    Print(L"The LAN MAC Address: %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", readmac00, readmac01, readmac02, readmac03, readmac04, readmac05);
    //sPrint(str,L"The LAN MAC Address: %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", readmac00, readmac01, readmac02, readmac03, readmac04, readmac05);

    for (i = 0; i < 6; i++)
    {
        recvbuf[i] = (unsigned char)IoRead8(0x4000 + i);
    }
    for (i = 0; i < 6; i++)
    {
        Print(L"The LAN MAC Address: %.2x", recvbuf[i]);
        Print(L"\n");
    }
    DestoryStdLib();
	return EFI_SUCCESS;
}
