/*************************************************************************
	> File Name: 4_4_read_GPT.c
	> Author: Jason.shao
	> Mail: shaogy12138@163.com 
	> Created Time: Fri 11 Mar 2022 02:58:10 PM CST
 ************************************************************************/


#include<Uefi.h>
#include<Base.h>
#include<Library/UefiLib.h>
#include<Library/BaseLib.h>
#include<Library/BaseMemoryLib.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/printLib.h>
#include<Protocol/DiskIo.h>
#include<Protocol/BlockIo.h>
#include<Protocol/DevicePath.h>
#include<Protocol/DevicePathToText.h>
#include<Uefi/UefiGpt.h>
#include<Library/DevicePathLib.h>



EFI_STATUS EFIAPI UefiMain(
    IN EFI_HANDLE   Handle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS  Status;
    UINTN       HandleIndex, HandleCount;
    EFI_HANDLE  *DiskControllerHandles = NULL;
    EFI_DISK_IO_PROTOCOL    *DiskIo;

    // 1. search device which support EFI_DISK_IO_PROTOCOL
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiDiskIoProtocolGuid,
        NULL,
        &HandleCount,
        &DiskControllerHandles
        );

    if(!EFI_ERROR(Status)){
        CHAR8 gptHeaderSector[512];
        EFI_PARTITION_TABLE_HEADER * gptHeader = (EFI_PARTITION_TABLE_HEADER *)gptHeaderSector;
        for(HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++){
            /* open EFI_DISK_IO_PROTOCOL */
            Status = gBS->HandleProtocol(
                DiskControllerHandles[HandleIndex],
                &gEfiDiskIoProtocolGuid,
                (VOID **)&DiskIo
                );
            if(!EFI_STATUS(Status)){
                EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
                EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *Device2TextProtocol = 0;
                CHAR16 *TextPath = 0;

                // 1.open EFI_DEVICE_PATH_PROTOCOL
                Status = gBS->OpenProtocol(
                    DiskControllerHandles[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DiskDevicePath,
                    ImageHandle,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
                if(!EFI_ERROR(Status)){
                    if(Device2TextProtocol == 0){
                        Status = gBS->LocateProtocol(
                            &gEfiDevicePathToTextProtocolGuid,
                            NULL,
                            (VOID **)&Device2TextProtocol
                            );
                        
                    }//end if

                // 2. use EFI_DEVICE_PATH_PROTOCOL get Text DEVICE path
                TextPath = Device2TextProtocol->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
                Print(L"%s\n", TextPath);
                if(TextPath) gBS->FreePool(TextPath);

                // 3. close EFI_DEVICE_PATH_PROTOCOL
                Status = gBS->CloseProtocol(
                    DiskControllerHandles[HandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    ImageHandle,
                    NULL
                    );
                }// end if
            }// end if
        {
            EFI_BLOCK_IO_PROTOCOL *BlockIo = *(EFI_BLOCK_IO_PROTOCOL **)(DiskIo + 1);
            EFI_BLOCK_IO_MEDIA *Media = Block->Media;

            /* read First sector*/
            Status = DiskIo->ReadDisk(DiskIo, Media->MediaId, 512, 512, gptHeader);


            /* check GPT flag */
            if((!EFI_ERROR(Status)) && (gptHeader->Header.Signature == 0x5452415020494645)){
                UINT32  CRCsum, gptHeaderCRCsum = (gptHeader->Header.CRC32);
                gptHeader -> Header.CRC32 = 0;
                gBS->CalculaterCrc32(gptHeader, (gptHeader->Header.HeaderSize), &(CRCsum));

                if(gptHeaderCRCsum == CRCsum){
                    // find GPT HEADER
                }//end if

            }// end if
        }
        }// end for
        gBS->FreePool(DiskControllerHandles);

    }//end if
}
