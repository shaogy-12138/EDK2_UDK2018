/**
  DO NOT EDIT
  FILE auto-generated
  Module name:
    AutoGen.h
  Abstract:       Auto-generated AutoGen.h for building module or library.
**/

#ifndef _AUTOGENH_6987936E_ED34_44db_AE97_1FA5E4ED2117
#define _AUTOGENH_6987936E_ED34_44db_AE97_1FA5E4ED2117

#ifdef __cplusplus
extern "C" {
#endif

#include <Base.h>
#include <Uefi.h>
#include <Library/PcdLib.h>

extern GUID  gEfiCallerIdGuid;
extern CHAR8 *gEfiCallerBaseName;

#define EFI_CALLER_ID_GUID \
  {0x6987936E, 0xED34, 0x44db, {0xAE, 0x97, 0x1F, 0xA5, 0xE4, 0xED, 0x21, 0x17}}

// Guids
extern EFI_GUID gEfiFileSystemInfoGuid;
extern EFI_GUID gEfiFileInfoGuid;
extern EFI_GUID gEfiMdePkgTokenSpaceGuid;
extern EFI_GUID gEfiVTUTF8Guid;
extern EFI_GUID gEfiVT100Guid;
extern EFI_GUID gEfiVT100PlusGuid;
extern EFI_GUID gEfiPcAnsiGuid;
extern EFI_GUID gEfiUartDevicePathGuid;
extern EFI_GUID gEfiSasDevicePathGuid;
extern EFI_GUID gEfiVirtualDiskGuid;
extern EFI_GUID gEfiVirtualCdGuid;
extern EFI_GUID gEfiPersistentVirtualDiskGuid;
extern EFI_GUID gEfiPersistentVirtualCdGuid;
extern EFI_GUID gEfiEventReadyToBootGuid;
extern EFI_GUID gEfiEventLegacyBootGuid;
extern EFI_GUID gEfiGlobalVariableGuid;
extern EFI_GUID gEdkiiIfrBitVarstoreGuid;
extern EFI_GUID gEfiShellEnvironment2ExtGuid;
extern EFI_GUID gEfiShellPkgTokenSpaceGuid;

// Protocols
extern EFI_GUID gEfiUnicodeCollation2ProtocolGuid;
extern EFI_GUID gEfiDevicePathProtocolGuid;
extern EFI_GUID gEfiLoadedImageProtocolGuid;
extern EFI_GUID gEfiSimpleFileSystemProtocolGuid;
extern EFI_GUID gEfiDevicePathToTextProtocolGuid;
extern EFI_GUID gEfiPlatformDriverOverrideProtocolGuid;
extern EFI_GUID gEfiBlockIoProtocolGuid;
extern EFI_GUID gEfiDriverBindingProtocolGuid;
extern EFI_GUID gEfiComponentNameProtocolGuid;
extern EFI_GUID gEfiDiskIoProtocolGuid;
extern EFI_GUID gEfiPciIoProtocolGuid;
extern EFI_GUID gEfiSimplePointerProtocolGuid;
extern EFI_GUID gEfiSimpleTextInputExProtocolGuid;
extern EFI_GUID gEfiShellProtocolGuid;
extern EFI_GUID gEfiShellParametersProtocolGuid;
extern EFI_GUID gEfiShellInterfaceGuid;
extern EFI_GUID gEfiSimpleTextInProtocolGuid;
extern EFI_GUID gEfiSimpleTextOutProtocolGuid;
extern EFI_GUID gEfiDebugPortProtocolGuid;
extern EFI_GUID gEfiGraphicsOutputProtocolGuid;
extern EFI_GUID gEfiHiiFontProtocolGuid;
extern EFI_GUID gEfiUgaDrawProtocolGuid;
extern EFI_GUID gEfiComponentName2ProtocolGuid;
extern EFI_GUID gEfiDriverConfigurationProtocolGuid;
extern EFI_GUID gEfiDriverConfiguration2ProtocolGuid;
extern EFI_GUID gEfiDriverDiagnosticsProtocolGuid;
extern EFI_GUID gEfiDriverDiagnostics2ProtocolGuid;
extern EFI_GUID gEfiFormBrowser2ProtocolGuid;
extern EFI_GUID gEfiShellEnvironment2Guid;
extern EFI_GUID gEfiHiiStringProtocolGuid;
extern EFI_GUID gEfiHiiImageProtocolGuid;
extern EFI_GUID gEfiHiiDatabaseProtocolGuid;
extern EFI_GUID gEfiHiiConfigRoutingProtocolGuid;

// Definition of SkuId Array
extern UINT64 _gPcd_SkuId_Array[];

// Definition of PCDs used in this module

#define _PCD_TOKEN_PcdUefiVariableDefaultPlatformLang  16U
#define _PCD_VALUE_PcdUefiVariableDefaultPlatformLang  _gPcd_FixedAtBuild_PcdUefiVariableDefaultPlatformLang
extern const UINT8 _gPcd_FixedAtBuild_PcdUefiVariableDefaultPlatformLang[6];
#define _PCD_GET_MODE_PTR_PcdUefiVariableDefaultPlatformLang  _gPcd_FixedAtBuild_PcdUefiVariableDefaultPlatformLang
#define _PCD_SIZE_PcdUefiVariableDefaultPlatformLang 6
#define _PCD_GET_MODE_SIZE_PcdUefiVariableDefaultPlatformLang  _PCD_SIZE_PcdUefiVariableDefaultPlatformLang 
//#define _PCD_SET_MODE_PTR_PcdUefiVariableDefaultPlatformLang  ASSERT(FALSE)  // It is not allowed to set value for a FIXED_AT_BUILD PCD

#define _PCD_TOKEN_PcdUefiVariableDefaultPlatformLangCodes  17U
#define _PCD_VALUE_PcdUefiVariableDefaultPlatformLangCodes  _gPcd_FixedAtBuild_PcdUefiVariableDefaultPlatformLangCodes
extern const UINT8 _gPcd_FixedAtBuild_PcdUefiVariableDefaultPlatformLangCodes[18];
#define _PCD_GET_MODE_PTR_PcdUefiVariableDefaultPlatformLangCodes  _gPcd_FixedAtBuild_PcdUefiVariableDefaultPlatformLangCodes
#define _PCD_SIZE_PcdUefiVariableDefaultPlatformLangCodes 18
#define _PCD_GET_MODE_SIZE_PcdUefiVariableDefaultPlatformLangCodes  _PCD_SIZE_PcdUefiVariableDefaultPlatformLangCodes 
//#define _PCD_SET_MODE_PTR_PcdUefiVariableDefaultPlatformLangCodes  ASSERT(FALSE)  // It is not allowed to set value for a FIXED_AT_BUILD PCD

// Definition of PCDs used in libraries is in AutoGen.c


EFI_STATUS
EFIAPI
ShellCEntryLib (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );





#ifdef __cplusplus
}
#endif

#endif
