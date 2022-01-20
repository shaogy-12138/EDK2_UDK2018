/*************************************************************************
	> File Name: MyTest/BS/Mem/BSMem.c
	> Author: 
	> Mail: 
	> Created Time: Wed 12 Jan 2022 11:09:06 AM CST
 ************************************************************************/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>



EFI_STATUS TestMMap(){
    
    EFI_STATUS              Status = 0;
    EFI_MEMORY_DESCRIPTOR   *MemoryMap = 0;
    UINTN                   MemoryMapSize = 0;
    UINTN                   MapKey = 0;
    UINTN                  DescriptorSize = 0;
    UINT32                   DescriptorVersion = 0;
    UINTN                   i;
    EFI_MEMORY_DESCRIPTOR   *MMap = 0;

    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    if(Status != EFI_BUFFER_TOO_SMALL){
        return Status;
    }
    
    Status = gBS->AllocatePool(EfiBootServicesData, MemoryMapSize, &MemoryMap);
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    for( i = 0; i < MemoryMapSize / DescriptorSize; i++ ){
        MMap = (EFI_MEMORY_DESCRIPTOR *)(((CHAR8 *)MemoryMap)+ i * DescriptorSize);
        Print(L"MemoryMap %4d %10d: ", MMap[0].Type, MMap[0].NumberOfPages);

        Print(L"%101x <->", MMap[0].PhysicalStart);
        Print(L"%101x <->", MMap[0].VirtualStart);
        Print(L"\n");
    }


    Print(L" MemoryMapSize = %d\n MapKey = %d\n DescriptorSize = %d\n DescriptorVersion = %d\n",MemoryMapSize, MapKey, DescriptorSize, DescriptorVersion);

    Status = gBS->FreePool(MemoryMap);


    return Status;

}




EFI_STATUS TestAllocateAddress()
{
    
    EFI_STATUS              Status = 0;
    EFI_PHYSICAL_ADDRESS    memory = 1024 * 1024;
    UINTN                   pages  = 3;
    
    Print(L"debug 1 ...\n");
    Status = gBS->AllocatePages(AllocateAddress, EfiBootServicesData, pages, &memory);
    Print(L"AllocatePages: %s %x\n", Status, memory);
    Print(L"debug 2 ...\n");
    

    if(Status == 0){
        CHAR16 *str = 0;
        str = (CHAR16 *)memory;
        str[0] = 'S';
        str[1] = 'G';
        str[2] = 'Y';
        str[3] = 0  ;

        Print(L"Test str: %s\n", str);
        Status = gBS->FreePages(memory, pages);
    }else{
        Print(L" allocate failed \n"); 
    }

    return Status;
}

EFI_STATUS TestAllocateAnyPages()
{
    
    EFI_STATUS              Status = 0;
    EFI_PHYSICAL_ADDRESS    memory;
    Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 3, &memory);
    Print(L"AllocatePages: %r %x\n", Status, memory);

    if(Status == 0){
        CHAR16 *str;
        str = (CHAR16 *)memory;
        str[0] = 'S';
        str[1] = 'G';
        str[2] = 'Y';
        str[3] = 0  ;

        Print(L"Test str: %s\n", str);
        Status = gBS->FreePages(memory, 3);
    }else{
        Print(L" allocate failed \n"); 
    }

    return Status;
}


EFI_STATUS 
EFIAPI
UefiMain(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{

    EFI_STATUS Status = 0;
//    Status = TestAllocateAddress();
    Status = TestAllocateAnyPages();
    Status = TestMMap();
    return Status;

}
