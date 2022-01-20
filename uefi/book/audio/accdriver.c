/*
 * =====================================================================================
 *
 *       Filename:  accdriver.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/2012 09:13:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */


#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h> 
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/DriverBinding.h>

#include "wintype.h"
#include "ac97reg.h"
#include "ichreg.h"

#include "audio.h"
#define true 1
#define false 0 

extern EFI_PCI_IO_PROTOCOL       *gAudioPciIo ;
extern EFI_HANDLE                 gAudioHandle ;

extern EFI_COMPONENT_NAME_PROTOCOL gAudioComponentName; 
extern EFI_COMPONENT_NAME2_PROTOCOL gAudioComponentName2; 
EFI_GUID gEfiAudioProtocolGUID = EFI_AUDIO_PROTOCOL_GUID; 

EFI_STATUS ReadCodecRegister (
		IN  AC97Register reg,
		OUT PWORD wData
		);

NTSTATUS WriteCodecRegister
(
    IN  AC97Register reg,
    IN  WORD wData,
    IN  WORD wMask
    );

void WriteBMControlRegister32
(
    IN  ULONG ulOffset,
    IN  ULONG ulValue
);

void WriteBMControlRegisterMask(
    IN  ULONG reg,
    IN  u8 wData,
    IN  u8 wMask
		);

ULONG ReadBMControlRegister32
(
    IN  ULONG ulOffset
);

EFI_STATUS WriteMaster(u32 addr, u8 data);

EFI_STATUS
EFIAPI
AC97DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
AC97DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
AC97DriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     ControllerHandle,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI AC97Reset(
        IN  EFI_AUDIO_PROTOCOL  *This
        );

EFI_STATUS
EFIAPI AC97Play(
        IN  EFI_AUDIO_PROTOCOL  *This,
	IN  UINT8* PcmData,
	IN  UINT32 Format,
	IN  UINTN Size
    );        

EFI_STATUS
EFIAPI AC97Volume(
        IN  EFI_AUDIO_PROTOCOL  *This,
	IN  INT32 Increase,
	IN OUT INT32 * NewVolume
	);



#define AUDIO_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('A', 'U', 'D', 'O')
#define EFI_AUDIO_PROTOCOL_REVISION  0x1
/**
  @member     Signature        The signature of the Protocol Context
  @member     FFAudio        The EFI_FFAUDIO_PROTOCOL   
 **/
typedef struct {
    UINTN                 Signature;
    EFI_AUDIO_PROTOCOL  Audio; 
    EFI_PCI_IO_PROTOCOL   *PciIo;
    BufferDescriptor Bdes[32]; 
} AUDIO_PRIVATE_DATA;

#define AUDIO_PRIVATE_DATA_FROM_THIS(a) CR (a, AUDIO_PRIVATE_DATA, Audio, AUDIO_PRIVATE_DATA_SIGNATURE)
//static AUDIO_PRIVATE_DATA gAudioPrivate;

//
// Driver binding protocol implementation for AC97 driver.
//
EFI_DRIVER_BINDING_PROTOCOL gAudioDriverBinding = {
  AC97DriverBindingSupported,
  AC97DriverBindingStart,
  AC97DriverBindingStop,
  0xa,
  NULL,
  NULL
};

//
// Template for Audio private data structure.
// The pointer to Audio protocol interface is assigned dynamically.
//
AUDIO_PRIVATE_DATA        gDiskIoPrivateDataTemplate = {
  AUDIO_PRIVATE_DATA_SIGNATURE,
  {
    EFI_AUDIO_PROTOCOL_REVISION,
    AC97Reset,
    AC97Play,
    AC97Volume,
    0
  },
  NULL,
  {0}
};

VOID
PlayEndEventNoify (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  AUDIO_PRIVATE_DATA    *Private = (AUDIO_PRIVATE_DATA   * )Context; 
  ULONG Status;

  (void)Private;
  gAudioPciIo = Private->PciIo; 
  Status = ReadBMControlRegister32(PO_SR);
  if(Status & (1<<3)){
      gBS->SignalEvent(Event);
      WriteBMControlRegisterMask(PO_SR           , 0,1<<3);
  }

}
/**
  Test to see if this driver supports ControllerHandle.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
AC97DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS            Status;
  PCI_TYPE00                        PciData;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  Status = gBS->OpenProtocol(
		  ControllerHandle,
		  &gEfiPciIoProtocolGuid, 
		  (VOID**)&PciIo,
		  This->DriverBindingHandle,
		  ControllerHandle,
		  EFI_OPEN_PROTOCOL_BY_DRIVER
			);
   if (EFI_ERROR (Status)) {
	   return Status;
   }

   Status = PciIo->Pci.Read (
		   PciIo,
		   EfiPciIoWidthUint32,
		   0,
		   sizeof (PciData) / sizeof (UINT32),
		   &PciData
		   );

   gBS->CloseProtocol (
        ControllerHandle,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        ControllerHandle
        );
   
   if (EFI_ERROR (Status)) {
	   return Status;
   }
   if (!(PciData.Hdr.ClassCode[2] == PCI_CLASS_MEDIA && PciData.Hdr.ClassCode[1] == PCI_CLASS_MEDIA_AUDIO && PciData.Hdr.ClassCode[0] == 0x00) ) {
	   return EFI_UNSUPPORTED;
   }
  return EFI_SUCCESS;
}

/**
  Start this driver on ControllerHandle by opening a PCI IO protocol and
  installing a Audio IO protocol on ControllerHandle.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
AC97DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS            Status;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  AUDIO_PRIVATE_DATA    *Private; 

  Status = gBS->OpenProtocol(
		  ControllerHandle,
		  &gEfiPciIoProtocolGuid, 
		  (VOID**)&PciIo,
		  This->DriverBindingHandle,
		  ControllerHandle,
		  EFI_OPEN_PROTOCOL_BY_DRIVER
			);
   if (EFI_ERROR (Status)) {
	   return Status;
   }
   //
   // Allocate a buffer to store the ATA_ATAPI_PASS_THRU_INSTANCE data structure
   //
   //Private = AllocateCopyPool (sizeof (AUDIO_PRIVATE_DATA),        &gDiskIoPrivateDataTemplate );
   Private = ( AUDIO_PRIVATE_DATA *)AllocatePages(1);
   if (Private == NULL) {
	   goto ErrorExit;
   }

   CopyMem(Private,  &gDiskIoPrivateDataTemplate, sizeof(AUDIO_PRIVATE_DATA));

   Private->PciIo = PciIo;
   Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_NOTIFY, (EFI_EVENT_NOTIFY)PlayEndEventNoify, (VOID*)Private, &Private->Audio.WaitForEndEvent);

   Status = gBS->InstallProtocolInterface (
                  &ControllerHandle,
                  &gEfiAudioProtocolGUID,
                  EFI_NATIVE_INTERFACE,
                  &Private->Audio
                  );


ErrorExit:
   if (EFI_ERROR (Status)) {

    if (Private != NULL) {
      //FreePool (Private);
      FreePages(Private, 1);
    }

    gBS->CloseProtocol (
          ControllerHandle,
          &gEfiPciIoProtocolGuid,
          This->DriverBindingHandle,
          ControllerHandle
          );
  }else{
   // Init Ac97
   AC97Reset(&Private->Audio);
  }
   return Status;
}

/**
  Stop this driver on ControllerHandle by removing Audio IO protocol and closing
  the PCI IO protocol on ControllerHandle.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
AC97DriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     ControllerHandle,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS            Status;
  AUDIO_PRIVATE_DATA    *Private; 
  EFI_AUDIO_PROTOCOL    *Audio;

  //
  // Get our context back.
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiAudioProtocolGUID,
                  (VOID **) &Audio,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Private = AUDIO_PRIVATE_DATA_FROM_THIS(This);

  Status = gBS->UninstallProtocolInterface (
                  ControllerHandle,
                  &gEfiAudioProtocolGUID,
                  &Private->Audio
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiPciIoProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle
                    );
  }

  if (!EFI_ERROR (Status)) {
    gBS->CloseEvent(Private->Audio.WaitForEndEvent);
    FreePool (Private);
  }

  return Status;
}

 
/**
  @param  This       Indicates a pointer to the calling context.

  @retval EFI_SUCCESS          . 
  @retval EFI_DEVICE_ERROR         . 

**/
EFI_STATUS
EFIAPI AC97Reset(
        IN  EFI_AUDIO_PROTOCOL  *This
        )
{
	NTSTATUS InitAC97 (void);
	EFI_STATUS            Status;
	AUDIO_PRIVATE_DATA* Private = AUDIO_PRIVATE_DATA_FROM_THIS(This);
	gAudioPciIo = Private->PciIo; 

	Status = InitAC97 ();
	return Status;
}

EFI_STATUS
EFIAPI AC97SetRate()
{
    // sample rate 44100 
		WriteCodecRegister (AC97REG_FRONT_SAMPLERATE, 44100, 0xFFFF);
		WriteCodecRegister (AC97REG_SURROUND_SAMPLERATE, 44100, 0xFFFF);
		WriteCodecRegister (AC97REG_LFE_SAMPLERATE, 44100, 0xFFFF);
        return 0;
}
/**
  @param  This       Indicates a pointer to the calling context.
  @param  PcmData    Pointer to PCM Data
  @param  Format     PCM Data Format
  @param  Size       How many bytes in PCM Data

  @retval EFI_SUCCESS          . 
  @retval EFI_DEVICE_ERROR         . 

**/
EFI_STATUS
EFIAPI AC97Play(
        IN  EFI_AUDIO_PROTOCOL  *This,
	IN  UINT8* PcmData,
	IN  UINT32 Format,
	IN  UINTN Size
        )
{
	EFI_STATUS            Status;
    const u16 FRAMES_PER_BLOCK = (65534) / 2;
	UINT8 i=0;
    UINTN FramesLeft = Size / 4;
	AUDIO_PRIVATE_DATA* Private = AUDIO_PRIVATE_DATA_FROM_THIS(This);
	gAudioPciIo = Private->PciIo; 

    // Run = 0
	WriteBMControlRegisterMask(PO_CR           , 0,1);

    //Print(L"Size:%d\n", Size);
	for( i=0; i< 32; i++, FramesLeft-=FRAMES_PER_BLOCK){
        if( i * FRAMES_PER_BLOCK >= Size/4) break;
		Private->Bdes[i].addr = (u32)(PcmData + FRAMES_PER_BLOCK * 4 * i);
        Private->Bdes[i].len = (u16)(FramesLeft < FRAMES_PER_BLOCK?FramesLeft*2:FRAMES_PER_BLOCK*2);
        Private->Bdes[i].BUP = 0;
        Private->Bdes[i].IOC = 0;
        //Print(L"Bdes[%d]; %0x len :%0x\n", i, Private->Bdes[i].addr, Private->Bdes[i].len*4);
	}
	Private->Bdes[i-1].BUP = 1;
	Private->Bdes[i-1].IOC = 0;
    Print(L"Bdes :%d; %0x len :%0x\n", i, Private->Bdes[0].addr, Private->Bdes[0].len);

	WriteBMControlRegister32(PO_BDBAR       ,  (u32)(long)Private->Bdes);
	WriteBMControlRegister(PO_LVI         ,  i-1);
    // Run = 1
	WriteBMControlRegisterMask(PO_CR           , 1,1);
    gBS->Stall(100);
    (void) Status;
    return EFI_SUCCESS;
}

/**
  @param  This       Indicates a pointer to the calling context.
  @param  Increase   How much should the volume change, 
                     +Number increase; -Number Decrease.
  @param  NewVolume   if *NewVolume >=0 , It will set the volume as *NewVolume;
                      if *NewVolume <0, the Volume will be changed by Increase, 
		      and *Newvolume returns the current Volume.

  @retval EFI_SUCCESS          . 
  @retval EFI_DEVICE_ERROR         . 

**/
EFI_STATUS
EFIAPI AC97Volume(
        IN  EFI_AUDIO_PROTOCOL  *This,
	IN  INT32 Increase,
	IN OUT INT32 * NewVolume
	)
{
	AUDIO_PRIVATE_DATA* Private = AUDIO_PRIVATE_DATA_FROM_THIS(This);
	gAudioPciIo = Private->PciIo; 

	if(*NewVolume >= 0){
		WORD data= (WORD) (* NewVolume);
		WriteCodecRegister (AC97REG_PCM_OUT_VOLUME, data, 0xFFFF);
	}else{
		WORD data= 0;
		ReadCodecRegister(AC97REG_PCM_OUT_VOLUME, &data); 
		data += (INT16) Increase;
		WriteCodecRegister (AC97REG_PCM_OUT_VOLUME, data, 0xFFFF);
		*NewVolume = (INT32)data;
	}
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitializeACC(
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    EFI_STATUS Status;
    //
    // Install driver model protocol(s).
    //
    Status = EfiLibInstallDriverBindingComponentName2 (
		    ImageHandle,
		    SystemTable,
		    &gAudioDriverBinding,
		    ImageHandle,
		    &gAudioComponentName,
		    &gAudioComponentName2
		    );
    //ASSERT_EFI_ERROR (Status);
    return Status;
}

#ifdef __cplusplus
}
#endif
