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
//#include <MainData.h>
#include <unistd.h>



#include <Library/IoLib.h> 


int main(int argc,char **argv) {

    UINT8 i = 0;
    UINT8 readmac00 = 0;
    UINT8 readmac01 = 0;
    UINT8 readmac02 = 0;
    UINT8 readmac03 = 0;
    UINT8 readmac04 = 0;
    UINT8 readmac05 = 0;
    unsigned char recvbuf[32];
    //EFI_STATUS Status = 0;
    //Status = InitStdlib();
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
    //DestoryStdLib();
    FILE *fp = fopen("./max.txt","r+");

    printf("%x %p\n",fp, *fp);

    fclose(fp);
	return EFI_SUCCESS;
}
