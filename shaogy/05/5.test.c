/*************************************************************************
	> File Name: 5.test.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Sat 12 Mar 2022 10:15:56 AM CST
 ************************************************************************/

// entry point func
typedef EFI_STATUS(EFIAPI *EFI_IMAGE_ENTRY_POINT)(
    IN      EFI_HANDLE              ImageHandle,
    IN      EIF_SYSTEM_TABLE        *SystemTable
);

// code eg.5.1
typedef struct{
    /* 1.Table Header */
    EFI_TABLE_HEADER            Hdr; //
    /* 2.FW info */
    CHAR16                      *FirmwareVendor;
    UINT32                      FirmwareRevision;
    /* 3.Std in/out/err */
    EFI_HANDLE                  ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;      //input : from ConsoleInHandle read char 
    EFI_HANDLE                  ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUPUT_PROTOCOL *ConOut;     // output: output char to ConsoleOutHandle
    EFI_HANDLE                  StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUPUT_PROTOCOL *StdErr;     // output err info to ConsoleOutHandle
    /* 4.Boot Service */
    EFI_BOOT_SERVICES           *BootServices;
    /* 5.RunTime Service */
    EFI_RUNTIME_SERVICES        *RunTimeServices;
    /* 6.Sys Config Table */
    UINTN                       NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE     *ConfigurationTable; // is arry , and arry[i] is table
}EFI_SYSTEM_TABLE;

// code eg.5.2
typedef struct{
    UINT64          Signature;
    UINT32          Revision;
    UINT32          HeaderSize; // sizeof(EFI_SYSTEM_TABLE)
    UINT32          CRC32;      // 1. clear 2. compute CRC32  3. write 
    UINT32          Reserved;
}EFI_TABLE_HEADER;

// code eg.5.3
typedef struct{
    EFI_GUID    VendorGuid;
    VOID        *VendorTable;
}EFI_CONFIGURATION_TABLE;


// code eg.5.4
{
    EFI_HANDLE          gImageHandle = NULL;
    EFI_SYSTEM_TABLE    *gST = NULL;
    EFI_BOOT_SERVICES   *gBS = NULL;

    EFI_STATUS EFIAPI UefiBootServicesTableLibConstructor(
        IN      EFI_HANDLE      ImageHandle,
        IN      EFI_SYSTEM_TABLE    *SystemTable
        )
    {
        gImageHandle = ImageHandle;
        gST          = SystemTable;
        gBS          = SystemTable->BootServices;

        return EFI_SUCCESS;
    }
}

// Event: CreateEvent CloseEvent CreateEventEx SignalEvent WaitForEvent CheckEvent
// Timer: SetTimer
// TPL : RaiseTPL  RestoreTPL

// Memory: AllocatePool FreePool GetMemoryMap AllocatePages FreePages

// Protocol Management: InstallProtocolInterface UninstallProtocolInterface 
//                      ReinstallProtocolInterface RegisterProtocolNotify
//                      InstallMultipleProtocolInterfaces
//                      UninstallMultipleProtocolInterfaces
//                      (install/uninstall Protocol to/from device)
//                      (register notify for special Protocol, when install Protocol, this
//                      notify trigger)

// Protocol Use: OpenProtocol HandleProtocol LocateProtocol 
//               LocateHandleBuffer LocateHandle 
//               OpenProtocolInformation ProtocolsPerHandle
//               CloseProtocol LocateDevicePath


// Driver Management: ConnectController (install dirver to special device controller)
//                    DisconnectController(uninstall Driver from special device controller)
//

// Image : LoadImage (load .efi to memory and create Image)
//         SatrtImage (start Image: call Image EntryPoint)
//         Exit UnloadImage
//

// ExitBootServices
//

// Other: 




// code eg.5.5
typedef struct{
    EFI_TABLE_HEADER    Header;

    // TPL
    EFI_RAISE_TPL       RaiseTPL;
    EFI_RESTORE_TPL     RestoreTPL;

    // Memory
    EFI_ALLOCATE_POOL   AllocatePool;
    EFI_FREE_POOL       FreePool;
    EFI_GET_MEMORY_MAP  GetMemoryMap;
    EFI_ALLOCATE_PAGES  AllocatePages;
    EFI_FREE_PAGES      FreePages;

    // Event & Timer
    EFI_CREATE_EVENT    CreateEvent;
    EFI_CHECK_EVENT     CheckEvent;
    EFI_SIGNAL_EVENT    SignalEvent;
    EFI_WAIT_FOR_EVENT  WaitForEvent;
    EFI_CLOSE_EVENT     CloseEvent;
    EFI_SET_TIMER       SetTimer;

    // Protocol
    EFI_INSTALL_PROTOCOL_INTERFACE  InstallProtocolInterface;
    EFI_REINSTALL_PROTOCOL_INTERFACE    ReinstallProtocolInterface;
    EFI_UNINSTALL_PROTOCPL_INTERFACE    UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL                 HandleProtocol;
    VOID                                *Reserved;
    EFI_REGISTER_PROTOCOL_NOTIFY        RegisterProtocolNotify;
    EFI_LOCATE_HANDLE                   LocateHandle;
    EFI_LOCATE_DEVICE_PATH              LocateDevicePath;
    EFI_INSTALL_CONFIGURATION_TABLE     InstallConfigurationTable;


    // Image 
    EFI_IAMGE_LOAD  LoadImage;
    EFI_IMAGE_START StartImage;
    EFI_EXIT        Exit;
    EFI_IMAGE_UNLOAD    UnloadImage;
    EFI_EXIT_BOOT_SERVICES ExitBootServices;

    // Other
    EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
    EFI_STALL Stall;
    EFI_SET_WATCHDOGS_TIMER         SetWatchdogTimer;

    // Driver
    EFI_CONNECT_CONTROLLER      ConnectController;
    EFI_DISCONNECT_CONTROLLER   DisconnectController;

    // Protocol
    EFI_OPEN_PROTOCOL               OpenProtocol;
    EFI_CLOSE_PROTOCOL              CloseProtocol;
    EFI_OPEN_PROTOCOL_INFORMATION   OpenProtocolInformation;
    EFI_PROTOCOLS_PER_HANDLE        ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER        LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL             LocateProtocol;
    EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACE     InstallMultipleProtocolInterfaces;
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACE   UninstallMultipleProtocolInterfaces;
    EFI_CALCULATE_CRC32             CalculateCrc32;
    EFI_COPY_MEM                    CopyMem;
    EFI_SET_MEM                     SetMem;
    EFI_CREATE_EVENT_EX             CreateEventEx;
}EFI_BOOT_SERVICES;

// AllocatePool/FreePool
typedef EFI_STATUS(EFIAPI *EFI_ALLOCATE_POOL)(
    IN      EFI_MEMORY_TYPE         PoolType,  // Memory type
    IN      UINTN                   Size,      // Memory Size
    OUT     VOID                    **Buffer   // Memory start Address
);
typedef EFI_STATUS(EFIAPI *EFI_FREE_POOL)(
    IN      VOID        *Buffer
);

typedef enum{
    EfiReservedMemoryType=0,    // not Use
    EfiLoaderCode,              // allocate to os loader's code
    EfiLoaderData,              // allocate to os loader's data
    EfiBootServicesCode,        // allocate to bs's code
    EfiBootServicesData,        // allocate to bs's data
    EfiRuntimeServicesCode,     // allocate to rs's code
    EfiRuntimeServicesData,     // allocate to rs's data
    EfiConventionalMemory,      // can allocate Memory
    EfiUnusableMemory,          // this block mem have error, can't use
    EfiACPIReclaimMemory,       // save ACPI table
    EfiACPIMemoryNVS,           // fw use
    EfiMemoryMappedIO,          // Memory map IO, rs can use
    EfiMemoryMappedIOPortSpace, // 
    EfiPalCode,                 // save for fw
    EfiMaxMemoryType
}EFI_MEMORY_TYPE;

// test 5-3 use AllocatePool/FreePool
{
    UINTN BufferSize = 1024;
    CHAR16 *Buf = NULL;
    EFI_STATUS Status;

    Status = gBS->AllocatePool(EfiBootServicesCode, BufferSize, &Buf);
    // ...
    Status = gBS->FreePool(Buf);
}

// AllocatePages/FreePages
typedef EFI_STATUS(EFIAPI *EFI_ALLOCATE_PAGES)(
    IN        EFI_ALLOCATE_TYPE       Type, //allocate type
    IN        EFI_MEMORY_TYPE         MemoryType, // memory type (same as AllocatePool)
    IN        UINTN                   Pages, // allocate the number of page
    IN OUT    EFI_PHYSICAL_ADDRESS    *Memory // IN : phyaddress or null ;OUT: virtual add
);

typedef EFI_STATUS(EFIAPI *EFI_FREE_POOL)(
    IN      EFI_PHYSICAL_ADDRESS    Memory,
    IN      UINTN           Pages // how many Pages will be free
);

typedef enum{
    AllocateAnyPages,  // allocate any can use page
    AllocateMaxAddress, // allocated page's end add must < special Address
    AllocateAddress, // allocate special phy address's page
    MaxAllocateType
}EFI_ALLOCATE_TYPE;

// test 5-4 AllocateAnyPages
{
    EFI_STATUS TestAllocateAnyPages(){
        EFI_STATUS Status = 0;
        EFI_PHYSICAL_ADDRESS        pages;
        Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesCode, 3, &pages);
        Print(L"AllocatePages: %r  %x\n", Status, pages);

        if(Status == 0){
            // success allocate pages
            CHAR16 *str = (CHAR16 *)pages;  

            Status = gBS->FreePages(pages, 3);  // free pages
        }else{
            // failed allocate pages

        }

        return Status;

    }
}

// test 5-5 Allocate special Address's 3 pages
{
    EFI_STATUS TestAllocateAddress(){
        EFI_STATUS      Status = 0;
        EFI_PHYSICAL_ADDRESS    pages = 1024 * 1024 * 10;

        Status = gBS->AllocatePages(AllocateAddress, EfiBootServicesCode, 3, &pages);
        Print(L"AllocatePages: %r  %x\n", Status, pages);

        if(Status == 0){
            // success allocate pages

            CHAR16 *str = (CHAR16 *)pages;  

            // ...
            Status = gBS->FreePages(pages, 3);  // free pages
        }else{
            // failed allocate pages

        }

        return Status;


    }
}


// GetMemoryMap: get all mem map in system
typedef EFI_STATUS(EFIAPI *EFI_GET_MEMORY_MAP)(
    IN  OUT     UINTN   *MemoryMapSize,
    IN  OUT     EFI_MEMORY_DESCRIPTOR   *MemoryMap, // output buffer
    OUT         UINTN       *MapKey,                // current Memorym map's key
    OUT         UINTN       *DescriptorSize,        // EFI_MEMORY_DESCRIPTOR's size
    OUT         UINT32      *DescriptorVersion      // EFI_MEMORY_DESCRIPTOR's Version
);

typedef struct{
    UINT32      Type;
    EFI_PHYSICAL_ADDRESS        PhysicalStart;
    EFI_VIRTUAL_ADDRESS         virtualStart;
    UINT64                      NumberOfPages;
    UINT64                      Attribute;
}EFI_MEMORY_DESCRIPTOR;



// RunTimeServices
// 1. time service: read/set system time. read/set wake time (from sleep to wake)
// 2. read/set system var
// 3. virtual Mem Service
// 4. Other: ResetSystem ... 


// code eg.5.13
typedef struct{
    EFI_TABLE_HEADER            Header;

    // 1. Time Service
    EFI_GET_TIME                GetTime;
    EFI_SET_TIME                SetTime;
    EFI_GET_WAKEUP_TIME         GetWakeupTime;
    EFI_SET_WAKEUP_TIME         SetWakeupTime;

    // 2. virtual Mem 
    EFI_SET_VIRTUAL_ADDRESS_MAP     SetVirtualAddressMap;
    EFI_CONVERT_POINTER             ConvertPointer;

    // 3. UEFI System var 
    EFI_GET_VARIABLE            GetVariable;
    EFI_GET_NEXT_VARIABLE_NAME  GetNextVariableName;
    EFI_SET_VARIABLE            SetVariable;

    // 4. Other 
    EFI_GET_NEXT_HIGH_MONO_COUNT    GetNextHighMonoCount;
    EFI_RESET_SYSTEM                ResetSystem;
    EFI_UPDATE_CAPSULE              UpdateCapsule;
    EFI_QUERY_CAPSULE_CAPABILITIES  QueryCapsuleCapabilities;
    EFI_QUERY_VARIABLE_INFO         QueryVeriableInfo;
}

// code eg.5.14
typedef EFI_STATUS(EFIAPI *EFI_GET_TIME)(
    OUT         EFI_TIME        *Time,      // current time 
    OUT         EFI_TIME_CAPABILITIES       *Capabilities // Optional; CLK HW xing neng
);
typedef EFI_STATUS(EFIAPI *EFI_SET_TIME)(IN     EFI_TIME        *Time);


// code eg.5.15
typedef struct{
    UINT16          Year;
    UINT8           Month;
    UINT8           Day;
    UINT8           Hour;
    UINT8           Minute;
    UINT8           Second;
    UINT8           Pad1;
    UINT32          Nanosecond;
    INT16           TimeZone;
    UINT8           Daylight;
    UINT8           Pad2;
}EFI_TIME;

// code eg.5.16
typedef struct{
    UINT32          Resoultion; // RTC-Real time Clock
    UINT32          Accuracy;   // 1ppm = 1E-6 s
    BOOLEAN         SetsToZero; // True = clear 
}EFI_TIME_CAPABILITIES;


// code eg.5.17  GetWakeupTime: return current wake_up_timer state
typedef EFI_STATUS(EFIAPI *EFI_GET_WAKEUP_TIME)(
    OUT         BOOLEAN         *Enabled, // Enabled/Disabled
    OUT         BOOLEAN         *Pending, // timer is Pending state?
    OUT         EFI_TIME        *Time     // timer set 
);

typedef EFI_STATUS(EFIAPI *EFI_SET_WAKEUP_TIME)(
    IN      BOOLEAN         Enabled,
    IN      EFI_TIME        *Time  //opetional
);


// GetVariable
// SetVariable
// GetNextVariableName
//
// code eg.5.18

typedef EFI_STATUS(EFIAPI *EFI_GET_VARIABLE)(
    IN      CHAR16      *VariableName,
    IN      EFI_GUID    *VendorGuid,
    OUT     UINT32      *Attributes, OPTIOANL
    IN OUT  UINTN       *DataSize,
    OUT     VOID        *Data
);

// SetVariable = Create + Update + Del
//  if var don't exit, then Create; if var exits and can change, then update;
//  if want to del, func 1: DataSize = 0 and 
//  Attributes != EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS || EFI_BARIABLE_APPEND_WRITE ||EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
//  func 2: Attributes = 0
//
//  code eg.5.20
typedef EFI_STATUS(EFIAPI *EFI_SET_VARIABLE)(
    IN  CHAR16      *VariableName,  // 
    IN  EFI_GUID    *VendorGuid,
    IN  UINT32      *Attributes,    // 
    IN  UINTN       DataSize,       //
    IN  VOID        *Data
);


// code eg.5.21 gRT->GetNextVariableName()
typedef EFI_STATUS(EFIAPI *EFI_GET_NEXT_VARIABLE_NAME)(

    IN  OUT     UINTN   *VariableNameSize, // 
    IN  OUT     CHAR16  *VariableName,      // IN: last var name, OUT: next var name
    IN  OUT     EFI_GUID    *VendorGuid     // IN: last var guid, OUT: next var guid
);


