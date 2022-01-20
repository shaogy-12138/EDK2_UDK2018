#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
    EFI_STATUS
UefiMain (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"Test ExitBootServices\n");
    {
        UINTN MemMapSize = 0;
        EFI_MEMORY_DESCRIPTOR* MemMap = 0;
        UINTN MapKey = 0;
        UINTN DesSize = 0;
        UINT32 DesVersion = 0;
        CHAR16* vendor = SystemTable->FirmwareVendor;
        //����̼���Ӧ�̵�����
        SystemTable -> ConOut-> OutputString(SystemTable->ConOut, vendor);
        _asm int 3;
        // ȡ�õ�ǰ��MapKey
        gBS->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DesSize, &DesVersion);
        // ����Boot Services
        gBS->ExitBootServices(ImageHandle, MapKey);
        _asm int 3;
        ASSERT(SystemTable -> BootServices == NULL); 

        //SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"Crash!\n"); 
        while(DesSize >0){}
    }
    return (EFI_STATUS)-1;
}

