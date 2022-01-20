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
#include <guid/FileInfo.h>
#include <Uutil.h>

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
     //δ�ҵ�EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
        return Status;
    }
    Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);

    // �ڸ�Ŀ¼������efiĿ¼
    Status = Root->Open(
            Root, 
            &EfiDirectory,
            (CHAR16*)L"efi",
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, //��ģʽ
            EFI_FILE_DIRECTORY
            );

    //��efiĿ¼������readme.txt�ļ�
    {
        EFI_FILE_PROTOCOL *ReadMe = 0;
        Status = EfiDirectory ->Open(
                EfiDirectory,    //This ָ��Ŀ¼\efi\ 
                &ReadMe,          //���ļ��ľ��   
                (CHAR16*)L"readme.txt",  //�ļ�ȫ·��Ϊ\efi\readme.txt
                EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,//��������
                0  //������ͨ�ļ���ʹ��Ĭ������
                );
        Status = ReadMe -> Close(ReadMe);
        
    }

    {
        EFI_FILE_PROTOCOL *SystemFile = 0;
        Status = EfiDirectory ->Open(
                EfiDirectory,     //This ָ��Ŀ¼ \efi\ 
                & SystemFile,     //���ļ����
                (CHAR16*)L"..\\system", // ����·��Ϊ \efi\..\system
                EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,//��������
                EFI_FILE_SYSTEM | EFI_FILE_HIDDEN // ���� ϵͳ�������ļ�
                );
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
     //δ�ҵ�EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
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
    CHAR16 Buf[65];
    UINTN BufSize = 64;

    SAFECALL(Status = GetFileIo(&Root));
    SAFECALL(Status = Root ->Open(
            Root,     
            & SystemFile,
            (CHAR16*)L"system", 
            EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
            0
            ));

    Status = SystemFile -> Read ( SystemFile,
            &BufSize,
            Buf
            );
    if(!EFI_ERROR(Status)){
        Buf[BufSize] = 0;
        Print(L"%s\n", Buf);
    }
    Status = SystemFile->Close(SystemFile);
    return Status;
}

EFI_STATUS 
TestReadDirectory()
{
    EFI_FILE_PROTOCOL               *Root;
	EFI_STATUS  Status = 0;
    Status = GetFileIo(&Root);

    // ��Ŀ¼
    {
        UINTN                BufferSize;

        BufferSize = 0;
        Status = Root -> Read( Root, &BufferSize, NULL); 
        if(Status == EFI_BUFFER_TOO_SMALL){
            EFI_FILE_INFO* FileInfo;
            Status = gBS -> AllocatePool( EfiBootServicesCode, BufferSize, (VOID**)&FileInfo); 
            Status = Root -> Read( Root, &BufferSize, FileInfo); 
            Print(L"Size : %d\nFileSize:%d \nPhysical Size:%d %s\n",  FileInfo->Size, FileInfo->FileSize, FileInfo->PhysicalSize, FileInfo->FileName);
            Print(L"%s\n", FileInfo->FileName);
            Status = gBS -> FreePool( FileInfo);
        }

    }
    return Status;
}

typedef VOID (*AccessFileInfo)(EFI_FILE_INFO* FileInfo);

VOID ListFileInfo(EFI_FILE_INFO* FileInfo)
{
    Print(L"Size : %d\nFileSize:%d \nPhysical Size:%d\n",  FileInfo->Size, FileInfo->FileSize, FileInfo->PhysicalSize);
    Print(L"%s\n", FileInfo->FileName);
}

EFI_STATUS
ListDirectory(EFI_FILE_PROTOCOL* Directory, AccessFileInfo callbk)
{
        UINTN                BufferSize;
        UINTN                ReadSize;
        EFI_STATUS  Status = 0;
        EFI_FILE_INFO* FileInfo;

        BufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 512;
        Status = gBS -> AllocatePool( EfiBootServicesCode, BufferSize, (VOID**)&FileInfo); 
        while(1){
            ReadSize = BufferSize;
            Status = Directory -> Read(Directory, &ReadSize, FileInfo); 
            if(Status == EFI_BUFFER_TOO_SMALL){
                BufferSize = ReadSize;
                BREAK_ERR(Status = gBS -> FreePool( FileInfo));
                BREAK_ERR(Status = gBS -> AllocatePool( EfiBootServicesCode, BufferSize, (VOID**)&FileInfo)); 
                BREAK_ERR(Status = Directory-> Read(Directory, &ReadSize, FileInfo)); 
            }

            if(ReadSize == 0) break;
            BREAK_ERR(Status);
            callbk(FileInfo);
        }
        Status = gBS -> FreePool( FileInfo);
        return 0;
}

EFI_STATUS 
TestWrite( EFI_FILE_PROTOCOL* Root)
{
	EFI_STATUS  Status = 0;
    UINTN                           BufSize;
    CHAR16                          *Buf= (CHAR16*)L"This is test file\n";

    EFI_FILE_PROTOCOL *ReadMe = 0;

    //�������ļ�������Ѿ��ļ��������       
    Status = Root->Open(
            Root, 
            &ReadMe, //����ļ����
            ( CHAR16*)L"readme.txt",  //�ļ���
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, //��ģʽ
            0);
    CheckStatus(L" EFI_FILE_PROTOCOL Create file ");
    if( ReadMe && !EFI_ERROR(Status)) {
        //д�ļ�
        BufSize = StrLen(Buf) * 2;
        Status = ReadMe ->Write (ReadMe, &BufSize, Buf);
        CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
        // �ر��ļ�
        Status = ReadMe ->Close (ReadMe);
        CheckStatus(L"EFI_FILE_PROTOCOL Close file ");
    }

    return Status;
}

EFI_STATUS 
TestWrite2( EFI_FILE_PROTOCOL* Root, CONST CHAR16* FileName)
{
	EFI_STATUS  Status = 0;
    UINTN                           BufSize;
    CHAR16                          *Buf= (CHAR16*)L"This is test file\n";

    EFI_FILE_PROTOCOL *SystemFile= 0;

    //�������ļ�������Ѿ��ļ��������       
    Status = Root->Open(
            Root, 
            &SystemFile, //����ļ����
            (CHAR16*)FileName,  //�ļ���
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, //��ģʽ
            EFI_FILE_SYSTEM | EFI_FILE_HIDDEN
            );
    CheckStatus(L" EFI_FILE_PROTOCOL Create file ");
    if( SystemFile && !EFI_ERROR(Status)) {
        //д�ļ�
        BufSize = StrLen(Buf) * 2;
        Status = SystemFile ->Write (SystemFile, &BufSize, Buf);
        CheckStatus(L"EFI_FILE_PROTOCOL Write file ");
        // �ر��ļ�
        Status = SystemFile ->Close (SystemFile);
        CheckStatus(L"EFI_FILE_PROTOCOL Close file ");
    }

    return Status;
}

EFI_STATUS TestOpenDirectory( EFI_FILE_PROTOCOL* Root)
{
	EFI_STATUS  Status = 0;
    EFI_FILE_PROTOCOL *EfiDirectory = 0;
    Status = Root->Open(
            Root, 
            &EfiDirectory,
            (CHAR16*)L"efi",
            EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, //��ģʽ
            EFI_FILE_DIRECTORY
            );
    TestWrite(EfiDirectory);
    TestWrite2(EfiDirectory, L"..\\system");
    Status = EfiDirectory -> Close(EfiDirectory);
    return Status;
}

EFI_STATUS 
TestAsyncFileIo( EFI_FILE_PROTOCOL* Root)
{
	EFI_STATUS  Status = 0;
    EFI_FILE_IO_TOKEN OpenToken0;
    EFI_FILE_IO_TOKEN OpenToken1;
    EFI_FILE_IO_TOKEN ReadToken;
    EFI_FILE_IO_TOKEN WriteToken;
    EFI_FILE_IO_TOKEN FlushToken;

    {
        IF_ERR(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &OpenToken0.Event)){
            goto ERR_0;
        }
        IF_ERR(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &OpenToken1.Event)){
            goto ERR_1;
        }
        IF_ERR(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &ReadToken.Event)){
            goto ERR_2;
        }
        IF_ERR(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &WriteToken.Event)){
            goto ERR_3;
        }
        IF_ERR(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &FlushToken.Event)){
            goto ERR_4;
        }
    }


    Status = gBS->CloseEvent(FlushToken.Event);
ERR_4:
    Status = gBS->CloseEvent(WriteToken.Event);
ERR_3:
    Status = gBS->CloseEvent(ReadToken.Event);
ERR_2:
    Status = gBS->CloseEvent(OpenToken1.Event);
ERR_1:
    Status = gBS->CloseEvent(OpenToken0.Event);
ERR_0:
    return Status;
}

EFI_STATUS TestAsyncOpen(EFI_FILE_PROTOCOL* Root)
{
	EFI_STATUS  Status = 0;
    EFI_FILE_IO_TOKEN OpenToken;
    EFI_FILE_PROTOCOL* AyncFile;
    UINTN Index = 0;
    SAFECALL(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &OpenToken.Event));
    Status = Root->OpenEx(Root,
           &AyncFile,
           L"aync.txt",
           EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, //��ģʽ
           0,
           &OpenToken); 
    IF_ERR(Status){
        gBS->CloseEvent(OpenToken.Event);
        Print(L"Aync Open %r\n", Status);
        return Status;
    }
    gBS->WaitForEvent(1, &OpenToken.Event, &Index);
    Print(L"Anyc Open Status:%d\n", OpenToken.Status);
    gBS->CloseEvent(OpenToken.Event);
    return Status;
}

EFI_STATUS TestAsyncWrite(EFI_FILE_PROTOCOL* File)
{
	EFI_STATUS  Status = 0;
    EFI_FILE_IO_TOKEN WriteToken;
    UINTN Index = 0;
    
    SAFECALL(Status = gBS->CreateEvent(0, TPL_NOTIFY, NULL, NULL, &WriteToken.Event));
    WriteToken.Buffer = L"Hello Async Write";
    WriteToken.BufferSize= StrLen((CHAR16*)WriteToken.Buffer) * 2;

    Status = File -> WriteEx(File, &WriteToken);
    IF_ERR(Status){
        gBS->CloseEvent(WriteToken.Event);
        Print(L"Aync Write %r\n", Status);
        return Status;
    }
    gBS->WaitForEvent(1, &WriteToken.Event, &Index);
    Print(L"Anyc Open Status:%d\n", WriteToken.Status);
    gBS->CloseEvent(WriteToken.Event);
    return Status;
}

VOID ReadNotification(EFI_EVENT event, VOID* Context)
{
    EFI_FILE_IO_TOKEN *ReadToken = (EFI_FILE_IO_TOKEN *)Context;
    // ���ReadToken.Status;
    // ���ReadToken.BufferSize;
    (VOID)ReadToken;
}

EFI_STATUS TestAsyncRead(EFI_FILE_PROTOCOL* File)
{
	EFI_STATUS  Status = 0;
    EFI_FILE_IO_TOKEN ReadToken;
    UINTN Index = 0;
    
    SAFECALL(Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_NOTIFY, ReadNotification, (VOID*)&ReadToken, &ReadToken.Event));
    ReadToken.BufferSize= 1024; 
    IF_ERR(Status = gBS -> AllocatePool( EfiBootServicesCode, ReadToken.BufferSize, (VOID**)&ReadToken.Buffer)){
        goto ERR_0;
    } 
    Status = File -> ReadEx(File, &ReadToken);
    IF_ERR(Status){
        goto ERR_1;
    }
    gBS->WaitForEvent(1, &ReadToken.Event, &Index);
    Print(L"Anyc Write Status:%d\n", ReadToken.Status);
    Print(L"Anyc Write %d bytes\n", ReadToken.BufferSize);
ERR_1:
    gBS->FreePool(ReadToken.Buffer);
ERR_0:
    gBS->CloseEvent(ReadToken.Event);
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

EFI_STATUS TestShellFile()
{
    EFI_STATUS                      Status;
    SHELL_FILE_HANDLE               FileHandle;
    UINTN                           WbufSize, RbufSize = 256;
    CHAR16                          *Wbuf= (CHAR16*)L"This is test file\n";
    CHAR16                          Rbuf[256] ; 
    EFI_SHELL_PROTOCOL            *gEfiShellProtocol;

    Status = OpenShellProtocol(&gEfiShellProtocol);
    WbufSize = StrLen(Wbuf) * 2;    
    Status = gEfiShellProtocol->CreateFile((CONST CHAR16*)L"testfile.txt", 0, &FileHandle); 
    Status = gEfiShellProtocol->WriteFile(FileHandle, &WbufSize, Wbuf);
    Status = gEfiShellProtocol->CloseFile(FileHandle); 
  
    RbufSize = 256;
    Status = gEfiShellProtocol->OpenFileByName((CONST CHAR16*)L"testfile.txt", &FileHandle, EFI_FILE_MODE_READ); 
    Status = gEfiShellProtocol->ReadFile(FileHandle, &RbufSize ,Rbuf  ); 
    Status = gEfiShellProtocol->CloseFile(FileHandle);
 
    return EFI_SUCCESS;

}

EFI_STATUS TestVar(CONST CHAR16* var)
{
    EFI_STATUS                      Status;
    SHELL_FILE_HANDLE               FileHandle;
    UINTN                           WbufSize;
    CHAR16                          *Wbuf= (CHAR16*)L"This is test file\n";
    EFI_SHELL_PROTOCOL            *gEfiShellProtocol;

    Status = OpenShellProtocol(&gEfiShellProtocol);
    WbufSize = StrLen(Wbuf) * 2;    
    Status = gEfiShellProtocol->CreateFile((CONST CHAR16*)var, 0, &FileHandle); 
    Status = gEfiShellProtocol->WriteFile(FileHandle, &WbufSize, Wbuf);
    Status = gEfiShellProtocol->CloseFile(FileHandle); 
 
    return EFI_SUCCESS;

}

EFI_STATUS UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
        )
{
    EFI_FILE_PROTOCOL               *Root;
	EFI_STATUS  Status = 0;
    Status = GetFileIo(&Root);
    //Status = TestOpen();
    //Status = TestWrite(Root); 
    //Status = TestOpenDirectory(Root); 
    //Status = TestReadDirectory();
    //Status = TestReadFile();
    //ListDirectory(Root, ListFileInfo);
    //Status = TestAsyncOpen(Root);
    Status = TestShellFile();
    Status = TestVar(L">vTestVar");
    return Status;
}
