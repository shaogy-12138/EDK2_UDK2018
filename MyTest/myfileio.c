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


//
// VAR DEFINE
//
extern EFI_GUID gEfiShellProtocolGuid;



//
// FUNCTION DECLERA
//

EFI_STATUS EFIAPI BZero(OUT CHAR16 *Destination, IN UINTN Length);
EFI_STATUS OpenShellProtocol(EFI_SHELL_PROTOCOL  **gEfiShellProtocol);
VOID Ascii2UnicodeString(CHAR8 *String, CHAR16 *Unistring);

//
// ShellAppMain
//
INTN EFIAPI ShellAppMain(UINTN Argc, CHAR16 **Argv){
    
    //
    // VER DEFINE
    //
    CHAR16              *OldFileName = NULL;
    CHAR16              *LineBuff = NULL;
    CHAR16              NewFileName[128] = {0};
    CHAR16              *ArrayBuffer = NULL;
    EFI_STATUS          Status;
    SHELL_FILE_HANDLE   FileHandle;
    UINTN               Index = 0;
    UINTN               WbufSize = 0;
    UINTN               FileSize = 0;
    UINTN               i = 0;
    UINTN               StartIndex = 0;
    CHAR8               *Ptr = NULL;


    if(Argc <= 1){
        Print(L"Please input file name!\n");
        return -1;
    }
    
    OldFileName = Argv[1];
    Print(L"The Old file name is %s \n", OldFileName);


    EFI_SHELL_PROTOCOL      *gEfiShellProtocol;
    Status = OpenShellProtocol(&gEfiShellProtocol);
    Print(L"Debug: OpenProtocol...\n");
    Status = gEfiShellProtocol->OpenFileByName((CONST CHAR16*)OldFileName, &FileHandle, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE);
    Print(L"Debug: OpenFileByName...\n");

    if(EFI_ERROR(Status)){
        Print(L"Please input valid Filename\n");
        return -1;
    }

    StrnCpyS(NewFileName, 128, OldFileName, StrLen(OldFileName) - 4);
    StrCatS(NewFileName, 128, L"_new.txt");
    Print(L"NewFileName = %s\n", NewFileName);


    // 
    // delete the have same name File
    //
    gEfiShellProtocol->DeleteFileByName(NewFileName);
    
    //
    // get file size
    //
    Status = gEfiShellProtocol->GetFileSize(FileHandle, &FileSize);
    if(EFI_ERROR(Status)){
        gEfiShellProtocol->CloseFile(FileHandle);
        return -1;
    }
    Print(L"File size is %d \n", FileSize);

    //
    // file whether have content
    //
    if(FileSize < 0){
        Print(L"File content is empty!\n");
        return -1;
    }

    FileSize += 1;

    // 
    // allocate memory accord file size
    //
    Status = gBS->AllocatePool(EfiReservedMemoryType, FileSize, &ArrayBuffer);
    Status = gBS->AllocatePool(EfiReservedMemoryType, FileSize, &LineBuff);

    //
    // set buffer content to 0
    //
    BZero(ArrayBuffer, FileSize);
    BZero(LineBuff, FileSize);
    Status = gEfiShellProtocol->ReadFile(FileHandle, &FileSize, ArrayBuffer);

    if(EFI_ERROR(Status)){
        Print(L"Read file info error\n");
        return -1;
    }

    //
    // create new FileHandle
    //
    Status = gEfiShellProtocol->CreateFile((CONST CHAR16*)NewFileName, 0, &FileHandle);
    if(EFI_ERROR(Status)){
        Print(L"create file %s failed\n", NewFileName);
        return -1;
    }
    
    // 
    // read file content and write in new File
    //
    WbufSize = FileSize;
    Status = gEfiShellProtocol->WriteFile(FileHandle, &WbufSize, ArrayBuffer);

    Status = gEfiShellProtocol->CloseFile(FileHandle);


    Ptr = (CHAR8 * ) ArrayBuffer;
    for(i = 0; i < FileSize; i++){
        if(Ptr[i] == '\n'){
            Ptr[i] = '\0';
            Ascii2UnicodeString(Ptr + StartIndex, LineBuff);
            StartIndex = i + 1;

            Index += 1;
            
            //
            // out put file content
            //
            Print(L"Line %d: %s\n", Index, LineBuff);
            BZero(LineBuff, FileSize);
        }
    }

    
    return 0;
}


//
// FUNCTION DEFINE
//

// Init Destination to zero(0)
EFI_STATUS EFIAPI BZero(OUT CHAR16 *Destination, IN UINTN Length){

    UINT32      i;
    CHAR8       *ptr = (CHAR8 * )Destination;
    for(i = 0; i < 2 * Length; i++){
        ptr[i] = 0;
    }
    return EFI_SUCCESS;
}

// Open need use's Protocol
EFI_STATUS OpenShellProtocol(EFI_SHELL_PROTOCOL  **gEfiShellProtocol){
    
    EFI_STATUS      Status;
    Status = gBS->OpenProtocol(
        gImageHandle,
        &gEfiShellProtocolGuid,
        (VOID **)gEfiShellProtocol,
        gImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );

    if(EFI_ERROR(Status)){
        //
        // Search for the shell Protocol
        //
        
        Status = gBS->LocateProtocol(
            &gEfiShellProtocol,
            NULL,
            (VOID **)gEfiShellProtocol
            );

        if(EFI_ERROR(Status)){
            gEfiShellProtocol = NULL;
        }
    }
    return EFI_SUCCESS;
}

VOID Ascii2UnicodeString(CHAR8 *String, CHAR16 *Unistring){
    
    while(*String != '\0'){
        *(Unistring++) = (CHAR16) *(String++);
    }

    //
    // END HTE Unistring WITH A NULL
    //
    *Unistring = '\0';
}
