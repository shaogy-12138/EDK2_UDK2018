/*************************************************************************
	> File Name: main.c
	> Author: shaogy 
	> Mail: shaogy12138@163.com
	> Created Time: Mon 27 Dec 2021 04:47:50 PM CST
 ************************************************************************/


#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>

//debug
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv)
{

    UINT8 i = 0;
    UINT8 readmac00 = 0;
    UINT8 readmac01 = 0;
    UINT8 readmac02 = 0;
    UINT8 readmac03 = 0;
    UINT8 readmac04 = 0;
    UINT8 readmac05 = 0;

    unsigned char recvbuf[32] = {0};
    char *str = NULL;

    // Read Mac Address
    readmac00 = IoRead8(0x4000);
    readmac01 = IoRead8(0x4001);
    readmac02 = IoRead8(0x4002);
    readmac03 = IoRead8(0x4003);
    readmac04 = IoRead8(0x4004);
    readmac05 = IoRead8(0x4005);

    sprintf(str,"MAC Address = %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", readmac00, readmac01,
         readmac02, readmac03, readmac04,readmac05);

    for ( i = 0; i < 6; i++ ){
        
        recvbuf[i] = (unsigned char)IoRead8(0x4000 + i);
    }

    for ( i = 0; i < 6; i++ ){

        Print(L"MAC Address : %.2x\n",recvbuf[i]);
    }

    // open file 
    FILE *fp = fopen(".\\mac.txt","w+");
    if(0){
        perror("open file error\n");
        return EFI_SUCCESS;
    }
    fputs(str, fp);
    Print(L"fp = %p\n", fp);
    Print(L"open file success\n");
    fclose(fp);

    return EFI_SUCCESS;
}
