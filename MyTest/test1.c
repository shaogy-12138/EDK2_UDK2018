#include <Uefi.h> 
#include <Library/UefiLib.h> 
#include <Library/ShellCEntryLib.h> 
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DebugLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/ShellLibHiiGuid.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UnicodeCollation.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/SortLib.h>
#include <Library/FileHandleLib.h>

#include <Protocol/EfiShell.h>
#include <Guid/FileInfo.h>

#include <stdio.h>
#include <string.h>

#include <Library/IoLib.h>
#define FILE_IO                 0x0000000000000020
#define LAN_1                   0x4000
#define LAN_2                   0x3000

// GLOBAL VAR DEFINE

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem;
EFI_FILE_PROTOCOL               *gFileRoot;
CHAR16              readmac[7] = {0};

// FUNCTION DECLAREATION

EFI_STATUS OpenFile(EFI_FILE_PROTOCOL **fileHandle, CHAR16 *fileName, UINT64 OpenMode);
EFI_STATUS ReadFile(EFI_FILE_PROTOCOL *fileHandle, UINTN *bufSize, VOID *buffer);
EFI_STATUS WriteFile(EFI_FILE_PROTOCOL *fileHandle, UINTN *bufSize, VOID *buffer);
EFI_STATUS SetFilePosition(EFI_FILE_PROTOCOL *fileHandle, UINT64 position);
EFI_STATUS GetFilePosition(EFI_FILE_PROTOCOL *fileHandle, UINT64 *position);
EFI_STATUS LocateFileRoot(void);
UINT64 InitGlobalProtocols(UINT64 flag);
char *GetMac(void);


// MAIN FUNCTION

int main(
    IN int Argc,
    IN char **Argv
    )
{
    UINT64  flag;
    gST->ConOut->OutputString(gST->ConOut, L"================ | UDK2018 Sample | ==================\n\r");
    gST->ConOut->OutputString(gST->ConOut, L"================ | FUN: Read Mac  | ==================\n\r");
    gST->ConOut->OutputString(gST->ConOut, L"================ | Date:2021.12.31| ==================\n\r");

    //Print(L">>debug1: before init... ");
    
    flag = InitGlobalProtocols(FILE_IO);
    Print(L"Flag = %x\n\r",flag);

    //GetMac();
    
    {
        EFI_STATUS          Status;
        EFI_FILE_PROTOCOL   *file;
        CHAR16              *filename = L"mac.txt";
        UINTN               bufLength = 20;
        char               *bufStr   = NULL;
        CHAR16              bufStr1[200];
        
        bufStr =L"shaogy";
       // bufStr = GetMac();
        printf("bufStr = %s\n\r", bufStr);
        Print(L"--open mac.txt file--\n\r");
        Status = OpenFile(&file, filename, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE);
        

        if(Status == EFI_SUCCESS){
            Print(L"Open file: %s success\n\r",filename);
        }
        else{
            
            Print(L"Open file: %s failed\n\r", filename);
            return 0;
        }

        bufLength = strlen(bufStr)*2 + 2;

        Status = file->Write(file, &bufLength, bufStr);
        if(Status == EFI_SUCCESS){
            Print(L"write info success\n\r");
            printf("bufLength = %d, bufStr = %s\n\r", bufLength, bufStr);
        }
        else{
            Print(L"write info failed\n\r");
            return 0;
        }
        
       /* Status = WriteFile(file, &bufLength, bufStr);
        if(Status == EFI_SUCCESS){
            Print(L"Write file: %s success\n\r",filename);
        }
        else{
            
            Print(L"Write file: %s failed\n\r",filename);
            return 0;
        }
        
        // bufLength = 10;
        //Status = ReadFile(file, &bufLength, bufStr1);
        //if(Status == EFI_SUCCESS){
          //    Print(L"Read file: %s success\n\r", filename);
       // }
       // else{
            
          //    Print(L"Read file: %s failed\n\r", filename);
          //  return 0;
       // }

       // printf("bufLength = %d, bufStr1 = %s\n\r", bufLength, bufStr1);
        */
        file->Close(file);
        
    
    }
    
    
    
    return 0;
}






// FUNCTION DEFINE 

char  *GetMac(void){
    
    UINT8               i = 0;
    UINT8               readmac00 = 0;
    UINT8               readmac01 = 0;
    UINT8               readmac02 = 0;
    UINT8               readmac03 = 0;
    UINT8               readmac04 = 0;
    UINT8               readmac05 = 0;
    char                str[50] = {0};
    char *s1;
    char *s2;
    char *s3;
    char *s4;
    char *s5;
    char *s6;
    //CHAR16              *macAdd = "Mac Add:";

    for(i = 0; i < 0; i++ ){
        readmac[i] = (CHAR16)IoRead8(0xE000 + i);
        printf("mac %d: %c\n\r",i, readmac[i]);
    }
    //printf("\n\r");
    //readmac[i] = '\0';
    readmac00 = IoRead8(LAN_1);
    readmac01 = IoRead8(LAN_1 + 1);
    readmac02 = IoRead8(LAN_1 + 2);
    readmac03 = IoRead8(LAN_1 + 3);
    readmac04 = IoRead8(LAN_1 + 4);
    readmac05 = IoRead8(LAN_1 + 5);
    
    //strcat(macAdd, strcat(" ", (CHAR16)readmac00));
    //strcat(macAdd, strcat("-", (CHAR16)readmac01));
    //strcat(macAdd, strcat("-", (CHAR16)readmac02));
    //strcat(macAdd, strcat("-", (CHAR16)readmac03));
    //strcat(macAdd, strcat("-", (CHAR16)readmac04));
    //strcat(macAdd, strcat("-", (CHAR16)readmac05));
    //
    sprintf(s1, "%.2x-", readmac00);
    sprintf(s2, "%.2x-", readmac01);
    sprintf(s3, "%.2x-", readmac02);
    sprintf(s4, "%.2x-", readmac03);
    sprintf(s5, "%.2x-", readmac04);
    sprintf(s6, "%.2x" , readmac05);

    //printf("%s%s%s%S%s%s", s1, s2, s3, s4, s5, s6);
    printf("Mac Address:%.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", 
           readmac00, \
           readmac01, \
           readmac02, \
           readmac03, \
           readmac04, \
           readmac05);
   sprintf(str, "Mac Address:%.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n\r", readmac00, readmac01, readmac02, readmac03, readmac04, readmac05);
    
    printf("2. %s\n", str);
    //Print(L"%s\n", str);
    return str;

}
UINT64 InitGlobalProtocols(UINT64 flag){

    EFI_STATUS      Status;
    UINT64 retVal   =    0;
    
    if((flag&FILE_IO) == FILE_IO){
        Status = LocateFileRoot();
        if(EFI_ERROR(Status)){
            retVal |= FILE_IO;
        }
    }
    
    return retVal;

}

EFI_STATUS LocateFileRoot(void){
    EFI_STATUS Status =  0;
    Status = gBS-> LocateProtocol(
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        (VOID**)&gSimpleFileSystem
        );

    if(EFI_ERROR(Status)){
        return Status;
    }

    Status = gSimpleFileSystem->OpenVolume(gSimpleFileSystem, &gFileRoot);

    return Status;
}

EFI_STATUS OpenFile(EFI_FILE_PROTOCOL **fileHandle, CHAR16 *fileName, UINT64 OpenMode){

    EFI_STATUS Status = 0;
    Status = gFileRoot->Open(
        gFileRoot,
        fileHandle,
        (CHAR16*)fileName,
        OpenMode,
        0 // open file mode: 0 - create, 1 - read, 2 -write
        );

    return Status;
}

EFI_STATUS ReadFile(EFI_FILE_PROTOCOL *fileHandle, UINTN *bufSize, VOID *buffer){
    
    EFI_STATUS Status = 0;
    Status = fileHandle->Read(fileHandle, bufSize, buffer);    
    
    return Status;

}

EFI_STATUS WriteFile(EFI_FILE_PROTOCOL *fileHandle, UINTN *bufSize, VOID *buffer){



    EFI_STATUS Status = 0;
    Status = fileHandle->Write(fileHandle, bufSize, buffer);
    
    return Status;

}

EFI_STATUS SetFilePosition(EFI_FILE_PROTOCOL *fileHandle, UINT64 position){
       
    EFI_STATUS Status = 0;
    Status = fileHandle->SetPosition(fileHandle, position);
    
    return Status;

}

EFI_STATUS GetFilePosition(EFI_FILE_PROTOCOL *fileHandle, UINT64 *position){


    EFI_STATUS Status = 0;
    Status = fileHandle->GetPosition(fileHandle, position);
    
    return Status;

}



