#include <Uefi.h>
EFI_STATUS UefiMain ( IN EFI_HANDLE  ImageHandle,IN EFI_SYSTEM_TABLE  *SystemTable)  // Èë¿Úº¯Êý
{
   SystemTable -> ConOut-> OutputString(SystemTable->ConOut, L"HelloWorld\n"); 
   return EFI_SUCCESS;
}


/*
// head file
#include<Uefi.h>
// type func_name(handle, ST)
EFI_STATUS UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable){
// ST - ConOut - OutputString (this, info)
SystemTable->ConOut->OutputString(SystemTable->ConOut,L"Hello world\n");


	return EFI_SUCCESS;
}*/