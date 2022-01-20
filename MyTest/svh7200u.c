/** @file
 **/
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
#include <Protocol/EfiShell.h>
#include <Guid/FileInfo.h>
#include <Library/IoLib.h>
#include <Uutil.h>


#define MAX_N 1024*1024*2
#define MAC1  0xE000    // MAC1 Memory Space Address
#define MAC2  0xE000    // Mac2 Memory Space Address

UINT8  MacNum = 1;  // Mac port number : 1 ? 2 ? if num > 2 , need modify TestWrite func
CHAR16 Mac[128] = {0}; // save mac Address
CHAR16 OldFileInfo[MAX_N + 5] = {0}; // save old file info (file size :max 1M), if want to save more info ,can modify MAX_N,but this xxx.efi file Size will more large
UINTN  OldFileBufSize = MAX_N;  // save old file real size 
CHAR16 *FileName = L"readmac.txt"; 

CHAR16 *TestReadMac(UINT8 n){
    UINT8 i = 0, index = 0, ReadMac = 0;
    for( ; i < 6; i++ ){
        ReadMac = ((n == 0)? ( IoRead8(MAC1 + i) ): ( IoRead8(MAC2 + i) ));
        UINT8 ReadMac_1 = ReadMac / 16; // 175 : 10 = 'A'
        UINT8 ReadMac_6  = ReadMac % 16; // 175 : 15 = 'F'
        
        Mac[index++] = ((ReadMac_1 < 10)?(ReadMac_1 + '0'):(ReadMac_1 % 10 + 'A')); // 10 => 0 + 'A'  8 => 8 + '0'
        Mac[index++] = ((ReadMac_6 < 10)?(ReadMac_6 + '0'):(ReadMac_6 % 10 + 'A')); // 15 => 5 + 'A'  8 => 8 + '0'
        (i != 5) && (Mac[index++] = '-');
    }


    return Mac;
}
EFI_STATUS TestOpen()
{
	EFI_STATUS  Status = 0;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
    EFI_FILE_PROTOCOL *Root = 0;
    EFI_FILE_PROTOCOL *EfiDirectory = 0;

    Status = gBS->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            (VOID**)&SimpleFileSystem
    );
    if (EFI_ERROR(Status)) {

        return Status;
    }
    Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);

    // CREATE efi Directory
    Status = Root->Open(
            Root, 
            &EfiDirectory,
            (CHAR16*)L"efi",
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
            EFI_FILE_DIRECTORY
            );

 
    // Open file readme.txt, if not exists, Create this file 
    {
        EFI_FILE_PROTOCOL *ReadMe = 0;
        Status = EfiDirectory ->Open(
                EfiDirectory, 
                &ReadMe, 
                FileName, 
                EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
                0);
        Status = ReadMe -> Close(ReadMe);
        
    }
    Print(L"[ Open file ]\n");

    // Create hidden file 
    {
        EFI_FILE_PROTOCOL *SystemFile = 0;
        Status = EfiDirectory ->Open(EfiDirectory, 
                                     & SystemFile,
                                     (CHAR16*)L"..\\system", 
                                     EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                                     EFI_FILE_SYSTEM | EFI_FILE_HIDDEN);
        Status = SystemFile->Close(SystemFile);

    }

    Status = EfiDirectory->Close(EfiDirectory);
    return Status;


}

EFI_STATUS 
GetFileIo( EFI_FILE_PROTOCOL** Root)
{
	EFI_STATUS  Status = 0;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;

    Status = gBS->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            (VOID**)&SimpleFileSystem
    );
    
    if (EFI_ERROR(Status)) {
   
        return Status;
    }
    Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, Root);
    return Status;
}

EFI_STATUS 
TestReadFile()
{
	EFI_STATUS  Status = 0;
    EFI_FILE_PROTOCOL               *Root;
    EFI_FILE_PROTOCOL *SystemFile = 0;
    // CHAR16 Buf[1025];
    // UINTN BufSize = MAX_N;

    SAFECALL(Status = GetFileIo(&Root));
    SAFECALL(Status = Root->Open(
            Root,     
            & SystemFile,
            FileName, 
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
            0
            ));

    Status = SystemFile -> Read ( SystemFile,
            &OldFileBufSize,
            OldFileInfo
            );
    if(EFI_ERROR(Status)){
        Print(L"Read file failed\n");
    }
    if(!EFI_ERROR(Status)){
        OldFileInfo[OldFileBufSize] = 0;
    }
    Status = SystemFile->Close(SystemFile);
    return Status;
}

EFI_STATUS 
TestWrite( EFI_FILE_PROTOCOL* Root)
{
	EFI_STATUS                      Status = 0;
    UINTN                           BufSize;
    CHAR16                          *Log  = L"===== SVH7200U MAC =====";
    CHAR16                          *LAN1 = L"\r\nMAC1: ";
    CHAR16                          *LAN2 = L"\r\nMAC2: ";
    CHAR16                          *LAN = L"\r\nMAC :";
    CHAR16                          Buf1[128] = {0};
    CHAR16                          Buf2[128] = {0};
    CHAR16                          *NewLine = L"\r\n";
    EFI_FILE_PROTOCOL               *ReadMe = 0;

    TestReadMac(0);
    StrnCpyS(Buf1, 128, (CONST CHAR16 *)Mac, StrLen(Mac));
    if( MacNum == 2 ){
        TestReadMac(1);
        StrnCpyS(Buf2, 128, (CONST CHAR16 *)Mac, StrLen(Mac));
        Print(L"[ MAC1: %s ]\n[ MAC2: %s ]\n", Buf1, Buf2);
    }
    else{
        Print(L"[ MAC: %s ]\n", Buf1);
    }

    TestReadFile();
    
    Status = Root->Open(
            Root, 
            &ReadMe, 
            FileName,  
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
            0);
    CheckStatus(L" EFI_FILE_PROTOCOL Create file ");

    if( ReadMe && !EFI_ERROR(Status)) {
        // read old info
        BufSize = StrLen(OldFileInfo) * 2;
        //Print(L"BufSize = %d\n", BufSize);
        if( BufSize ){
            Status = ReadMe->Write(ReadMe, &BufSize, OldFileInfo);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
            BufSize = StrLen(NewLine) * 2;
            Status =ReadMe->Write(ReadMe, &BufSize, NewLine);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
        }
        BufSize = StrLen(Log) * 2;
        Status = ReadMe ->Write (ReadMe, &BufSize, Log);
        CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
       
        if( MacNum == 1 ){
            
            BufSize = StrLen(LAN) * 2;
            Status = ReadMe ->Write (ReadMe, &BufSize, LAN);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
        
            BufSize = StrLen(Buf1) * 2;
            Status = ReadMe ->Write (ReadMe, &BufSize, Buf1);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");

        }
        else{
            
            BufSize = StrLen(LAN1) * 2;
            Status = ReadMe ->Write (ReadMe, &BufSize, LAN1);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");

            BufSize = StrLen(Buf1) * 2;
            Status = ReadMe ->Write (ReadMe, &BufSize, Buf1);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
              
            BufSize = StrLen(LAN2) * 2;
            Status = ReadMe ->Write (ReadMe, &BufSize, LAN2);
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");

            BufSize = StrLen(Buf2) * 2;
            Status = ReadMe ->Write (ReadMe, &BufSize, Buf2);
            
            CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
        }
        
        Status = ReadMe ->Close (ReadMe);
        CheckStatus(L"EFI_FILE_PROTOCOL Close file ");
    }
    Print(L"[ Write success ]\n");
    return Status;
}
EFI_STATUS OpenShellProtocol( EFI_SHELL_PROTOCOL            **gEfiShellProtocol )
{
    EFI_STATUS                      Status;
    Status = gBS->OpenProtocol(
            gImageHandle,
            &gEfiShellProtocolGuid,
            (VOID **)gEfiShellProtocol,
            gImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
    if (EFI_ERROR(Status)) {
    //
    // Search for the shell protocol
    //
        Status = gBS->LocateProtocol(
                &gEfiShellProtocolGuid,
                NULL,
                (VOID **)gEfiShellProtocol
                );
        if (EFI_ERROR(Status)) {
            gEfiShellProtocol = NULL;
        }
  }
  return Status;
}
EFI_STATUS EFIAPI UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    EFI_FILE_PROTOCOL               *Root;
	EFI_STATUS  Status = 0;
    Status = GetFileIo(&Root);
    //Status = TestOpen();
    Status = TestWrite(Root); 
    return Status;
}
