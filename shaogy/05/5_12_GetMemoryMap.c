/*************************************************************************
	> File Name: 5_12_GetMemoryMap.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Mon 14 Mar 2022 01:52:17 PM CST
 ************************************************************************/

#include<Uefi.h>
#include<Library/UefiBootServicesTableLib.h>
#include<stdio.h>

EFI_STATUS
EFIAPI
UefiMain(
    IN      EFI_HANDLE      ImageHandle,
    IN      EFI_SYSTEM_TABLE *SystemTable
    )
{

    EFI_STATUS Status = 0;

    UINTN       MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR   *MemoryMap = 0;
    UINTN   MapKey = 0, DescriptorSize = 0, DescriptorVersion = 0, i = 0;
    EFI_MEMORY_DESCRIPTOR   *MMap = 0;

    // 1. get buffer size
    Status = gBS->GetMemoryMap(
        &MemoryMapSize,
        MemoryMap,
        &MapKey,
        &DescriptorSize,
        &DescriptorVersion
        );

    if(Status != EFI_BUFFER_TOO_SMALL){
        return Status;
    }

    // 2. allocate Memory 
    Status = gBS->AllocatePool(EfiBootServicesData, MemoryMapSize, &MemoryMap);

    // 3. call GetMemoryMap
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    // sizeof(MemoryMap[i]) = DescriptorSize
    // DescriptorSize != sizeof(EFI_MEMORY_DESCRIPTOR)
    for(i = 0; i < MemoryMapSize / DescriptorSize; i++){
        MMap = (EFI_MEMORY_DESCRIPTOR *) (((CHAR8 *)MemoryMap) + i * DescriptorSize);
        Print(L"MemoryMap  %4d   %10d :", MMap[0].Type, MMap[0].NumberOfPages);
        Print(L"%101x<->", MMap[0].PhysicalStart);
        Print(L"%101x<->", MMap[0].VirtualStart);
        printf("MemoryMap  %4d   %10d :", MMap[0].Type, MMap[0].NumberOfPages);
        printf("%101x<->", MMap[0].PhysicalStart);
        printf("%101x<->", MMap[0].VirtualStart);
    }

    Status = gBS->FreePool(MemoryMap);

    return Status;
}
