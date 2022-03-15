/*************************************************************************
	> File Name: test.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Fri 11 Mar 2022 09:14:07 AM CST
 ************************************************************************/

#include<stdio.h>

// code eg.4.1
struct _EFI_BLOCK_IO_PROTOCOL{
    // member var
    UINT64                  Revision;
    EFI_BLOCK_IO_MEDIA      *Media;

    // member func
    EFI_BLOCK_RESET         Reset;
    EFI_BLOCK_READ          ReadBlocks;
    EFI_BLOCK_WRITE         WriteBlocks;
    EFI_BLOCK_FLUSH         FlushBlocks;
};

extern EFI_GUID gEfiBlockIoProtocolGuid;

// code eg.4.2
typedef EFI_STATUS(EFIAPI *EFI_BLOCK_READ)(
    IN EFI_BLOCK_IO_PROTOCOL *This,     //this pointer, point to context, who use this func, this = who.
    IN UINT32                MediaId,   //media id
    IN EFI_LBA               Lba,       //lba: logic block address, where will to read
    IN UINTN                 BufferSize, //read bytes number
    OUT VOID                 *Buffer    // read info to Buffer
);

//EFI_BLOCK_READ is a function pointer;
//return type is EFI_STATUS

//for example: BlockIo from lba = 0 read BlockSize data to Buffer
struct _EFI_BLOCK_IO_PROTOCOL *BlockIo;
BlockIo->ReadBlocks(Blocks,
                   BlockIo->Media->MediaId,
                   0,
                   BlockIo->Media->BlockSize,
                   (VOID *)Buffer);

// code eg.4.3
typedef VOID *EFI_HANDLE;
typedef struct{
    UINTN               Signature; //Handle lei bie
    LIST_ENTRY          AllHandles; //all IHANDLE consist ListTable a->b->c->d ...
    LIST_ENTRY          Protocols; // this handle's all Protocols, double dir ListTable
    UINTN               LocateRequest;
    UINT64              Key;
}IHANDLE;

typedef struct {
    UINTN           Signature;
    LIST_ENTRY      Link;
    IHANDLE         *Handle;
    LIST_ENTRY      ByProtocol;
    PROTOCL_ENTRY   *Protocols;
    VOID            *Interface;
    LIST_ENTRY      OpenList;
    UINTN           OpenListCount;
}PROTOCOL_INTERFACE;

typedef struct{
    UINTN           Signature;
    LIST_ENTRY      AllEntries;
    EFI_GUID        ProtocolID;
    LIST_ENTRY      Protocols;
    LIST_ENTRY      Notify;
}PROTOCL_ENTRY;

// get protocol GUID
IHANDLE *Handle;
GUID = Handle->Protocols->Protocol->ProtocolID;

// code eg.4.4
typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL){
    IN  EFI_HANDLE          Handle;
    IN  EFI_GUID            *Protocols;
    OUT VOID                **Interface, // OPTIONAL return will open Protocol obj
    IN  EFI_HANDLE          AgentHandle; // Open this Protocol's Image
    IN  EFI_HANDLE          ControllerHandle; // Use this Protocol's Controller
    IN  UINT32              Attributes; // Open Protocol's function
}

// Attributes
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL    0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL          0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL         0x00000004  // this attribute, Interface is OPTIONAL
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER   0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER             0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE             0x00000020

// 4-1: Open BlockIo
{
    extern EFI_BOOT_SERVICE             *gBS;
    EFI_HANDLE                          ImageHandle;
    EFI_DRIVER_BINDING_PROTOCOL         *This;
    IN EFI_HANDLE                       ControllerHandle;
    extern EFI_GUID                     gEfiBlockIoProtocolGuid;// define in *.dec file [Protocols] option , must be sheng ming in *.inf 's [Protocols]
    EFI_BLOCK_IO_PROTOCOL               *BlockIo;
    EFI_STATUS                          Status;

    // if use OpenProtocol in Application, ControllerHandle can set NULL
    Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEfiBlockIoProtocolGuid,
        &BlockIo,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
        );

    // if use OpenProtocol in Driver, ControllerHandle is Controller's Handle
    Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEfiBlockIoProtocolGuid,
        &BlockIo,
        This->DriverBindingHandle,
        ControllerHandle,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );
}

// code eg.4.5
typedef EFI_STATUS(EFIAPI *EFI_HANDLE_PROTOCOL)(
    IN  EFI_HANDLE      Handle,
    IN  EFI_GUID        *Protocols,
    OUT VOID            **Interface
);

// code eg.4.6
EFI_STATUS EFIAPI CoreHandleProtocol(
    IN EFI_HANDLE           Handle,
    IN EFI_GUID             *Protocols,
    OUT VOID                **Interface
    )
{
    return CoreOpenProtocol(
        Handle,
        Protocols,
        Interface,
        gDxeCoreImageHandle, //AgentHandle
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
        );
}


// code eg.4.7
typedef EFI_STATUS(EFIAPI * EFI_LOCATE_PROTOCOL)(
    IN  EFI_GUID        *Protocol,
    IN  VOID            *Registration, // OPTIONAL, the key which get from RegisterProtocolNotify()
    OUT VOID            **Interface // return the first match's Protocols obj in the system
);


// code eg.4.8
typedef EFI_STATUS(EFIAPI *EFI_LOCATE_HANDLE_BUFFER)(
    IN      EFI_LOCATE_SEARCH_TYPE      SearchType, // Search funcs
    IN      EFI_GUID                    *Protocol,  // special protocol
    IN      VOID                        *SearchKey, // OPTIONAL, PROTOCL_NOTIFY type
    IN OUT  UINTN                       *NoHandles, // return find Handle's number
    OUT     EFI_HANDLE                  *Buffer    // allocate Handle arr and return, Buffer save Handles. [Notes: this Buffer allocated by system, and released by caller]
);

// SearchType
typedef enum{
    AllHandles, // find all Handles in system
    ByRegisterNotify, // from RegisterProtocolNotify find match SearchKey's Handle
    ByProtocol // from system Handle db find support special Protocol's Handle
}EFI_LOCATE_SEARCH_TYPE;


typedef EFI_STATUS(EFIAPI *EFI_LOCATE_PROTOCOL)(
    IN      EFI_LOCATE_SEARCH_TYPE      SearchType,
    IN      EFI_GUID                    *Protocol, // OPTIONAL
    IN      VOID                        *SearchKey, //OPTIONAL
    IN OUT  VOID                        *BufferSize, // find Handles number
    OUT     EFI_HANDLE                  *Buffer  // return found Handle, [Notes: this Buffer allocated and released by caller]
);

// 4-3: USE LocateHandler find boot partion
EFI_STATUS DetectBootablePartition(void *Bootable){
    EFI_STATUS      Status;
    EFI_HANDLE      *ControllerHandles = NULL;
    UINTN           HandleIndex, NumHandles;

    // 1. find all install SimpleFileSystemProtocol's Handle
    Status = gBS->LocateHandlerBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL;
        &NumHandles,
        &ControllerHandles
        );

    if(EFI_ERROR(Status)){
        return Status;
    }

    // 2. scan all Handle
    for(HandleIndex = 0; HandleIndex < NumHandles; HandleIndex++){
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *SimpleFileSystem;
        // Open SimpleFileSystemProtocol which on the Handle
        Status = gBS->HandleProtocol(
            ControllerHandles[HandleIndex],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID **)&SimpleFileSystem;
            );

        // check efi\boot\bootx64.efi File
        // ... ...
    }

    // 3. release ControllerHandles used memory
    if(ControllerHandles != NULL){
        Status = gBS->FreePool(ControllerHandles);
    }

    return Status;
}


// code eg.4.10
typedef EFI_STATUS(EFIAPI *EFI_PROTOCOLS_PRE_HANDLE)(
    IN      EFI_HANDLE      Handle,             // the goal Handle
    OUT     EFI_GUID        ***ProtocolBuffer,  // save all Protocol GUID
    OUT     UINTN           *ProtocolBufferCount // mZ


// code eg.4.11
typedef EFI_STATUS(EFIAPI *EFI_OPEN_PROTOCOL_INFORMATION)(
    IN      EFI_HANDLE      Handle,         // special device handle 
    IN      EFI_GUID        *Protocol,      // special Search Protocol
    OUT     EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  **EntryBuffer, // open info through this buffer return, contain AgentHandle, ControllerHandle, open mode and open number  
    OUT     UINTN           *EntryCount // the number of EntryBuffer
);

// code eg.4.12
typedef struct(
    EFI_HANDLE  AgentHandle,
    EFI_HANDLE  ControllerHandle,
    UINT32      Attributes,
    UINT32      OpenCount,
)EFI_OPEN_PROTOCOL_INFORMATION_ENTRY;
// code eg.4.13
typedef EFI_STATUS(EFIAPI *EFI_CLOSE_PROTOCOL)(
    IN      EFI_HANDLE      Handle,
    IN      EFI_GUID        *Protocol,
    IN      EFI_HANDLE      AgentHandle,
    IN      EFI_HANDLE      ControllerHandle
);





