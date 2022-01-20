/*************************************************************************
	> File Name: MyTest/systemtable/gRT/RT.c
	> Author: 
	> Mail: 
	> Created Time: Fri 14 Jan 2022 02:52:30 PM CST
 ************************************************************************/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>


#define INIT_NAME_BUFFER_SIZE   128
#define INIT_DATA_BUFFER_SIZE   1024




EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *systemtable
    )
{
    EFI_STATUS                  Status;
    CHAR16                      *FoundVarName, *OldName;
    UINT32                      Atts;
    EFI_GUID                    FoundVarGuid;
    UINT8                       *DataBuffer;
    UINTN                       DataSize, NameBufferSize, NameSize, OldNameBufferSize, DataBufferSize;
    
    
    NameBufferSize = INIT_NAME_BUFFER_SIZE;
    DataBufferSize = INIT_DATA_BUFFER_SIZE;

    // allocate to varname , and must init to 0
    FoundVarName = AllocateZeroPool(NameBufferSize);
    if( FoundVarName == NULL ){
        return (EFI_OUT_OF_RESOURCES);
    }

    DataBuffer = AllocatePool(DataBufferSize);
    if(DataBuffer == NULL){
        FreePool(FoundVarName);
        return (EFI_OUT_OF_RESOURCES);
    }

    for( ; ; ){
        NameSize = NameBufferSize;
        Status = gRT->GetNextVariableName(&NameSize, FoundVarName, &FoundVarGuid);

        if(Status == EFI_BUFFER_TOO_SMALL){
            //
            // if buffer is not enough, then reallocate memory and run GetNextVariableName
            //
            OldName = FoundVarName;
            OldNameBufferSize = NameBufferSize;

            //
            // allocate enough Memory
            //
            NameBufferSize = (NameSize > NameBufferSize * 2) ? NameSize : (NameBufferSize * 2);
            FoundVarName = AllocateZeroPool(NameBufferSize);

            if(FoundVarName == NULL){
                Status = EFI_OUT_OF_RESOURCES;
                FreePool(OldName);
                break;
            }

            // 
            // COPY current varname from oldbuffer to newbuffer
            //
            CopyMem(FoundVarName, OldName, OldNameBufferSize);
            FreePool(OldName);
            NameSize = NameBufferSize;

            //
            // redo GetNextVariableName
            //
            Status = gRT->GetNextVariableName(&NameSize, FoundVarName, &FoundVarGuid);

        }
        if(Status == EFI_NOT_FOUND){
            break;
        }
        Print(L"%s\n", FoundVarName);
        DataSize = DataBufferSize;
        Status = gRT->GetVariable(FoundVarName, &FoundVarGuid, &Atts, &DataSize, DataBuffer);
        if(Status == EFI_BUFFER_TOO_SMALL){
            Print(L"EFI_BUFFER_TOO_SMALL\n");
        }
    }

    return Status;
}
