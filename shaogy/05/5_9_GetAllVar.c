/*************************************************************************
	> File Name: 5_8_GetVariable.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Tue 15 Mar 2022 09:37:19 AM CST
 ************************************************************************/

#include<Uefi.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/UefiRuntimeServicesTableLib.h>
#include<Library/MemoryAllocationLib.h>
#include<Library/BaseMemoryLib.h>
#include<Library/UefiLib.h>


#define INIT_NAME_BUFFER_SIZE       128
#define INIT_DATA_BUFFER_SIZE       1024

EFI_STATUS EFIAPI UefiMain(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
    )
{
    
    EFI_STATUS      Status = 0;
    CHAR16          *FoundVarName = NULL, *OldName = NULL;
    UINT32          Atts;
    EFI_GUID        FoundVarGuid;
    UINT8           *DataBuffer;
    UINTN           DataSize, NameBufferSize, NameSize, OldNameBufferSize, DataBufferSize;

    NameBufferSize = INIT_NAME_BUFFER_SIZE;
    DataBufferSize = INIT_DATA_BUFFER_SIZE;

    // 
    FoundVarName = AllocateZeroPool(NameBufferSize);
    if(FoundVarName == NULL){
        return (EFI_OUT_OF_RESOURCES);
    }

    DataBuffer = AllocatePool(DataBufferSize);
    if(DataBuffer == NULL){
        FreePool(FoundVarName);
        return (EFI_OUT_OF_RESOURCES);
    }

    for(;;){
        NameSize = NameBufferSize;
        Status = gRT->GetNextVariableName(&NameSize, FoundVarName, &FoundVarGuid);
        if(Status == EFI_BUFFER_TOO_SMALL){
            OldName = FoundVarName;
            OldNameBufferSize = NameBufferSize;

            NameBufferSize = (NameSize > NameBufferSize * 2) ? NameSize : NameBufferSize * 2;
            FoundVarName = AllocateZeroPool(NameBufferSize);
            if(FoundVarName == NULL){
                Status = EFI_OUT_OF_RESOURCES;
                FreePool(OldName);
                break;
            }
            CopyMem(FoundVarName, OldName, OldNameBufferSize);
            FreePool(OldName);

            NameSize = NameBufferSize;

            Status = gRT->GetNextVariableName(&NameSize, FoundVarName, &FoundVarGuid);
        }

        if(Status == EFI_NOT_FOUND){
            break;
        }

        Print(L"%s\n", FoundVarName);
        DataSize = DataBufferSize;
        Status = gRT->GetVariable(FoundVarName, &FoundVarGuid, &Atts, &DataSize, DataBuffer);
        if(Status == EFI_BUFFER_TOO_SMALL){
            Print(L"Current has error!\n");
        }
    }



    return Status;
}
