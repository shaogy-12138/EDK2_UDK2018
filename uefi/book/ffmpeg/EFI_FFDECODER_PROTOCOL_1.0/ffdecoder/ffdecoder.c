/*
 * =====================================================================================
 *
 *       Filename:  ffdecoder.c
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

// http://www.inb.uni-luebeck.de/~boehme/using_libavcodec.html
// avcodec_sample.cpp

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use
//
// g++ -o avcodec_sample avcodec_sample.cpp -lavformat -lavcodec -lz
//
// to build (assuming libavformat and libavcodec are correctly installed on
// your system).
//
// Run using
//
// avcodec_sample myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.

#ifdef __cplusplus
extern "C"{
#endif
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <Uefi.h> 
#include <Base.h> 
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <stdio.h>
#include <math.h>
#include "ffdecoder.h"
#define true 1
#define false 0 

#define FFDECODER_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('V', 'I', 'D', 'O')

/**
  @member     Signature        The signature of the Protocol Context
  @member     FFDecoder        The EFI_FFDECODER_PROTOCOL   
  @member     pFormatCtx       Video Format Context
  @member     videoStream      The index of Video Stream in all the streams.
  @member     pCodecCtx        Codec context
  @member     pFrame           The yuv Frame
  @member     pFrameRGBA       The RGBA Frame
  @member     buffer           Internal used
  @member     img_convert_ctx  The Context of converting from yuv to rgba.
 **/
typedef struct {
    UINTN                 Signature;
    EFI_FFDECODER_PROTOCOL  FFDecoder; 
    AVFormatContext *pFormatCtx;
    int             videoStream;
    AVCodecContext  *pCodecCtx;
    AVFrame         *pFrame; 
    AVFrame         *pFrameRGBA;
    uint8_t         *buffer;
    struct SwsContext  *img_convert_ctx ;

} FFDECODER_PRIVATE_DATA;

#define FFDECODER_PRIVATE_DATA_FROM_THIS(a) CR (a, FFDECODER_PRIVATE_DATA, FFDecoder, FFDECODER_PRIVATE_DATA_SIGNATURE)
static FFDECODER_PRIVATE_DATA gFFDecoderPrivate;

int GetNextFrame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, 
    int videoStream, AVFrame *pFrame)
{
    static AVPacket packet;
    static int      bytesRemaining=0;
    static uint8_t  *rawData;
    static int     fFirstTime=true;
    int             bytesDecoded;
    int             frameFinished;

    // First time we're called, set packet.data to NULL to indicate it
    // doesn't have to be freed
    if(fFirstTime)
    {
        fFirstTime=false;
        packet.data=NULL;
    }

    // Decode packets until we have decoded a complete frame
    while(true)
    {
        // Work on the current packet until we have decoded all of it
        while(bytesRemaining > 0)
        {
            // Decode the next chunk of data
            //bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, rawData, bytesRemaining);
            bytesDecoded=avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Was there an error?
            if(bytesDecoded < 0)
            {
                //fprintf(stderr, "Error while decoding frame\n");
                return false;
            }

            bytesRemaining-=bytesDecoded;

            // Did we finish the current frame? Then we can return
            if(frameFinished)
                return true;
        }

        // Read the next packet, skipping all packets that aren't for this
        // stream
        do
        {
            // Free old packet
            int ret = 0;
            if(packet.data!=NULL)
                av_free_packet(&packet);

            // Read new packet
            if((ret = av_read_packet(pFormatCtx, &packet))<0)
                goto loop_exit;
            if(ret == 32)
                goto loop_exit;
        } while(packet.stream_index!=videoStream);

        bytesRemaining=packet.size;
    }

loop_exit:

    // Decode the rest of the last frame
    //bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, rawData, bytesRemaining);
    bytesDecoded=avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,  &packet);

    // Free last packet
    if(packet.data!=NULL)
        av_free_packet(&packet);

    return frameFinished!=0;
}


#define SAFE_FREE(x) do{ if(x) av_free(x); x = 0; }while(0);
#define SAFE_CLOSE2(x,func) do{ if(x) func(x); x = 0; }while(0);

EFI_STATUS
CloseVideo(
        IN EFI_FFDECODER_PROTOCOL* This
        )
{
    FFDECODER_PRIVATE_DATA* Private;
    Private = FFDECODER_PRIVATE_DATA_FROM_THIS(This);
    (void) Private;

    // Free the RGB image
    SAFE_FREE(Private-> buffer);
    SAFE_FREE(Private-> pFrameRGBA);

    // Free the YUV frame
    SAFE_FREE(Private-> pFrame);

    // Close the codec
    SAFE_CLOSE2(Private-> pCodecCtx, avcodec_close);

    // Close the video file
    SAFE_CLOSE2(Private-> pFormatCtx, av_close_input_file);

    // Close Img Convert Context
    SAFE_CLOSE2(Private-> img_convert_ctx, sws_freeContext);

    if(Private != &gFFDecoderPrivate){
       (void) FreePool (Private);
    }

    return EFI_SUCCESS;
}

EFI_STATUS
OpenVideoInternal(
        IN EFI_FFDECODER_PROTOCOL* This,
        IN CHAR16* FileName
        )
{

    AVFormatContext *pFormatCtx = 0;
    int             videoStream = 0;
    AVCodecContext  *pCodecCtx = 0;
    AVFrame         *pFrame = 0; 
    AVFrame         *pFrameRGBA = 0;
    uint8_t         *buffer = 0;
    struct SwsContext  *img_convert_ctx = 0 ;

    int             numBytes, i = 0;
    AVCodec         *pCodec;
    CHAR8* FileName8 = NULL;
    EFI_STATUS Status = 0;

    FFDECODER_PRIVATE_DATA* Private;
    Private = FFDECODER_PRIVATE_DATA_FROM_THIS(This);
    (void) Private;

    //If the last is not closed, close it;
    if(Private-> buffer)
        CloseVideo(This);

    // Register all formats and codecs
    av_register_all();

    // Open video file
    {
        UINTN len = StrLen(FileName);
        UINTN i = 0;
        EFI_STATUS ret =  gBS->AllocatePool(EfiLoaderData, len+1, &FileName8 );

        for(i = 0; i< len ; i++)
            FileName8[i] = (CHAR8)FileName[i];
        FileName8[i] = 0;
    }

    if(av_open_input_file(&pFormatCtx, FileName8, NULL, 0, NULL)!=0){
       (void) gBS->FreePool ( FileName8);
       Status = EFI_NOT_FOUND;
       return Status;
       // Couldn't open file
    }
    (void) gBS->FreePool ( FileName8);
    FileName8 = NULL;

    // Retrieve stream information
    if(av_find_stream_info(pFormatCtx)<0){
       Status = EFI_NOT_READY ;
       goto FailedToOpen;
       // Couldn't find stream information
    }

    // Dump information about file onto standard error
    //dump_format(pFormatCtx, 0, argv[1], false);

    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++){
        //Print(L"type %d ",pFormatCtx->streams[i]->codec->codec_type  );
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;

        }
    }
    if(videoStream==-1){
       Status =EFI_NOT_READY ;
       goto FailedToOpen;
       // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL){
       Status = EFI_NOT_READY;
       goto FailedToOpen;
       // Codec not found
    }

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;

    // Open codec
    if(avcodec_open(pCodecCtx, pCodec)<0){
        // Could not open codec
        Status = EFI_NOT_READY;
        goto FailedToOpen;
    }

    // Hack to correct wrong frame rates that seem to be generated by some 
    // codecs
    //if(pCodecCtx->frame_rate>1000 && pCodecCtx->frame_rate_base==1)
    //    pCodecCtx->frame_rate_base=1000;

    // Allocate video frame
    pFrame=avcodec_alloc_frame();

    // Allocate an AVFrame structure
    pFrameRGBA=avcodec_alloc_frame();
    if(pFrameRGBA==NULL){
        Status = EFI_NOT_READY;
        goto FailedToOpen;
    }

    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGBA, pCodecCtx->width,
            pCodecCtx->height);
    buffer=(uint8_t*) av_malloc(numBytes);

    // Assign appropriate parts of buffer to image planes in pFrameRGBA
    avpicture_fill((AVPicture *)pFrameRGBA, buffer, PIX_FMT_RGBA,
            pCodecCtx->width, pCodecCtx->height);

    // 
    img_convert_ctx = sws_getContext( pCodecCtx->width, pCodecCtx->height,
            PIX_FMT_YUV420P, 
            pCodecCtx->width, pCodecCtx->height,
            PIX_FMT_RGBA, 
            SWS_SPLINE, NULL, NULL, NULL); 

    Private->pFormatCtx       =  pFormatCtx; 
    Private->videoStream      =  videoStream;
    Private->pCodecCtx        =  pCodecCtx;
    Private->pFrame           =  pFrame; 
    Private->pFrameRGBA       =  pFrameRGBA;
    Private->buffer           =  buffer;
    Private->img_convert_ctx  =  img_convert_ctx ;
    return 0;
FailedToOpen:
    // Release memory
    // Free the RGB image
    SAFE_FREE(buffer);
    SAFE_FREE(pFrameRGBA);

    // Free the YUV frame
    SAFE_FREE( pFrame);

    // Close the codec
    SAFE_CLOSE2(pCodecCtx, avcodec_close);

    // Close the video file
    SAFE_CLOSE2( pFormatCtx, av_close_input_file);

    // Close Img Convert Context
    SAFE_CLOSE2(img_convert_ctx, sws_freeContext);

    return Status;
}


EFI_STATUS
OpenVideo(
        IN EFI_FFDECODER_PROTOCOL* This,
        IN CHAR16* FileName
        )
{
    return OpenVideoInternal(This, FileName);
}

EFI_STATUS
OpenVideo2(
        IN EFI_FFDECODER_PROTOCOL* This,
        IN CHAR16* FileName,
        OUT EFI_FFDECODER_PROTOCOL** pVideoHandle
        )
{
    EFI_STATUS Status;
    EFI_FFDECODER_PROTOCOL* VideoHandle;
    FFDECODER_PRIVATE_DATA* Private = (FFDECODER_PRIVATE_DATA*)AllocateZeroPool(sizeof(FFDECODER_PRIVATE_DATA));
    if(Private == NULL) 
        return EFI_OUT_OF_RESOURCES; 
    VideoHandle = &(Private-> FFDecoder);
    Private-> Signature= FFDECODER_PRIVATE_DATA_SIGNATURE; 
    Private->FFDecoder.OpenVideo      = This->OpenVideo; 
    Private->FFDecoder.CloseVideo     = This->CloseVideo; 
    Private->FFDecoder.QueryFrame     = This->QueryFrame;  
    Private->FFDecoder.QueryFrameSize = This->QueryFrameSize; 
    Private->FFDecoder.OpenVideo2     = This->OpenVideo2; 
    Status = OpenVideoInternal(VideoHandle, FileName);
    if(EFI_ERROR(Status)){
        FreePool(VideoHandle);
        VideoHandle = NULL;
    }
    *pVideoHandle = VideoHandle;
    return Status;
}


EFI_STATUS
QueryFrame(
        IN  EFI_FFDECODER_PROTOCOL  *This,
        OUT AVFrame                 **ppFrame
        )
{
    int Ret = 0;
    FFDECODER_PRIVATE_DATA* Private;
    Private = FFDECODER_PRIVATE_DATA_FROM_THIS(This);
    (void) Private;
    if(Private-> pCodecCtx == NULL)
        return EFI_NO_MEDIA; 

    Ret = GetNextFrame(Private-> pFormatCtx, Private-> pCodecCtx, Private-> videoStream, Private-> pFrame);
    if(Ret){
        //Convert from yuv to rgba
        Ret = sws_scale(Private-> img_convert_ctx, Private-> pFrame->data, Private-> pFrame->linesize, 
                0, Private-> pCodecCtx->height, Private-> pFrameRGBA->data, Private-> pFrameRGBA->linesize); 
        *ppFrame = Private-> pFrameRGBA;
    }else {
        *ppFrame = 0;
        return EFI_END_OF_MEDIA;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
QueryFrameSize(
        IN  EFI_FFDECODER_PROTOCOL  *This,
        OUT UINT32                  *Width,
        OUT UINT32                  *Height
        )
{
    FFDECODER_PRIVATE_DATA* Private;
    Private = FFDECODER_PRIVATE_DATA_FROM_THIS(This);
    (void) Private;

    if(Private-> pCodecCtx){
        *Width = Private-> pCodecCtx->width;
        *Height = Private-> pCodecCtx->height;
        return EFI_SUCCESS;
    }else{
        return EFI_NO_MEDIA;
    }
}


EFI_GUID gEfiFFDecoderProtocolGUID = EFI_FFDECODER_PROTOCOL_GUID ;

INTN
EFIAPI
DriverInitMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  );


EFI_STATUS
EFIAPI
ShellLibConstructorWorker2 (
        void* aEfiShellProtocol,
        void* aEfiShellParametersProtocol,
        void* aEfiShellEnvironment2, 
        void* aEfiShellInterface
  );

EFI_STATUS
EFIAPI
InitFFdecoder (
        IN EFI_HANDLE        ImageHandle,
        IN EFI_SYSTEM_TABLE  *SystemTable
        )
{
    EFI_STATUS Status;
    FFDECODER_PRIVATE_DATA* Private = &gFFDecoderPrivate;
    //Print(L"%a: %d \n", __func__, __LINE__);
    ShellLibConstructorWorker2(NULL,NULL,NULL,NULL);
    (void) DriverInitMain(0, NULL);


    Private-> Signature= FFDECODER_PRIVATE_DATA_SIGNATURE; 
    Private->FFDecoder.OpenVideo = OpenVideo; 
    Private->FFDecoder.CloseVideo = CloseVideo; 
    Private->FFDecoder.QueryFrame= QueryFrame;  
    Private->FFDecoder.QueryFrameSize= QueryFrameSize; 
    Private->FFDecoder.OpenVideo2 = OpenVideo2; 

    Status = gBS->InstallProtocolInterface (
            &ImageHandle,
            &gEfiFFDecoderProtocolGUID ,
            EFI_NATIVE_INTERFACE,
            &Private->FFDecoder
            );

}

#ifdef __cplusplus
}
#endif
