/*
 * =====================================================================================
 *
 *       Filename:  fplayer.c
 *
 *    Description:  Example of playing video with  EFI_FFDECODER_PROTOCOL 
 *
 *        Version:  1.0
 *        Created:  03/12/2012 09:13:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h> 
#include <Base.h> 
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
#ifdef __cplusplus
}
#endif
#include "ffdecoder.h"

EFI_STATUS LocateGraphicsOutput()
{
    EFI_STATUS Status = gBS->LocateProtocol(
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            (VOID **)&GraphicsOutput);
    if (EFI_ERROR(Status)) {
        Print(L"LocateProtocol %r\n", Status);
    }
    return Status;
}
void ShowFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    if(GraphicsOutput)
        GraphicsOutput->Blt(
                GraphicsOutput,
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)pFrame->data[0],
                EfiBltBufferToVideo,
                0,0,
                0,0,
                width, height,
                0
                );
}

int main(int argc, char *argv[])
{
    AVFrame         *pFrame; 
    EFI_GUID gEfiFFDecoderProtocolGUID = EFI_FFDECODER_PROTOCOL_GUID ;
    EFI_FFDECODER_PROTOCOL *FFDecoder; 
    UINT32 Width, Height;
    CHAR16* FileName = 0;

    // Locate the Protocol
    EFI_STATUS Status = gBS->LocateProtocol(
            &gEfiFFDecoderProtocolGUID ,
            NULL,
            (VOID **)&FFDecoder );
    if (EFI_ERROR(Status)) {
        Print(L"LocateProtocol %r\n", Status);
        return Status;
    }
    LocateGraphicsOutput();

    // Open Video
    Status =  gBS->AllocatePool(EfiLoaderData, AsciiStrLen(argv[1]) *2 + 2, (VOID**)&FileName );
    AsciiStrToUnicodeStr(argv[1], FileName);
    Status = FFDecoder -> OpenVideo( FFDecoder, FileName);
    (void) gBS->FreePool ( FileName);
    if (EFI_ERROR(Status)) {
        Print(L"Open %r\n", Status);
        return Status;
    }
    // Query Frame Size(Width height)
    Status = FFDecoder-> QueryFrameSize(FFDecoder, &Width, &Height);
    // Query Frame 
    while( !EFI_ERROR( FFDecoder-> QueryFrame(FFDecoder, &pFrame)))
    {
        ShowFrame(pFrame, Width, Height, 0);
    }
    // Close Video
    Status = FFDecoder -> CloseVideo(FFDecoder );
    return 0;
}
