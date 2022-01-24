/*************************************************************************
	> File Name: Event.c
	> Author: Jason.shao
	> Mail: 
	> Created Time: Mon Jan 17 14:50:40 2022
 ************************************************************************/


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SimplePointer.h>



VOID myEventNotify30(IN EFI_EVENT Event, IN VOID *Context){
    static UINTN times = 0;
    Print(L"%s \n myEventNotify signal %d\n", Context, times);
    times ++;
}

EFI_STATUS TestTimer(){

    EFI_STATUS          Status;
    EFI_EVENT           myEvent;
    UINTN               index = 0;
    Print(L"Test EVT_TIMER | EVT_NOTIFY_SIGNAL");


    // 1. CREATE TIMER EVENT
    Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, 
                              TPL_CALLBACK, 
                              (EFI_EVENT_NOTIFY)myEventNotify30, 
                              (VOID*)L"Hello, Time out!", 
                              &myEvent);
    
    // 2. SET TIMER ATTRIBUTE
    Status = gBS->SetTimer(myEvent, TimerPeriodic, 10 * 1000 * 1000);
   // WaitKey();
    Status = gBS->WaitForEvent(1, &myEvent, &index);


    Status = gBS->CloseEvent(myEvent);
    return Status;
}
VOID myEventNotify(
    IN EFI_EVENT EVENT,
    IN VOID *Context
    )
{
    
    static UINTN times = 0;
    int i = 1000;
    while(i--);
    Print(L"myEventNotify wait %d\n", times);
    times ++;
    if(times > 5){
        gBS->SignalEvent(EVENT);
    }
    
}

EFI_STATUS TestNotify()
{
    EFI_STATUS  Status;
    UINTN       index = 0;
    EFI_EVENT   myEvent;

    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT,
                             TPL_NOTIFY,
                             (EFI_EVENT_NOTIFY)myEventNotify,
                             (VOID *)NULL,
                             &myEvent);

    if(Status){  }
    Status = gBS->WaitForEvent(1, &myEvent, &index);
    return EFI_SUCCESS;
}

EFI_STATUS TestMouseSimple(){
    EFI_STATUS          Status;
    EFI_SIMPLE_POINTER_PROTOCOL* mouse = 0;
    EFI_SIMPLE_POINTER_STATE     State;
    EFI_EVENT           events[2]; //{0, gBS->ConIn->WaitForKey}

    Print(L">>Debug2: start func\n");
    // show cursor
    gST->ConOut->EnableCursor(gST->ConOut, TRUE);
    
    Print(L">>Debug3: find mouse device\n");
    // find mouse device
    Status = gBS->LocateProtocol(&gEfiSimplePointerProtocolGuid, NULL, (VOID **)&mouse);

    Print(L">>Debug4: reset mouse device\n");
    //reset mouse device
    Status = mouse->Reset(mouse, TRUE);
    
    events[0] = mouse->WaitForInput;
    // input keyEvent to events[1]
    events[1] = gST->ConIn->WaitForKey;

    while(1){
        EFI_INPUT_KEY           Key;
        UINTN                   index;

        // wait events occur
        Status = gBS->WaitForEvent(2, events, &index);
        if(Status){  }
        
        if(index == 0){
            // get and output mouse state
            Status = mouse->GetState(mouse, &State);
            Print(L"X:%d Y:%d Z:%d L:%d R:%d\n",State.RelativeMovementX,State.RelativeMovementY,State.RelativeMovementZ,State.LeftButton, State.RightButton);
        }else{
            Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
            // press 'q', exit
            if(Key.UnicodeChar == 'q')
                break;
            Print(L"%c", Key.UnicodeChar);
        }
    }

    return EFI_SUCCESS;

}




EFI_STATUS  
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS Status;
    Print(L">>Debug1: input func\n");
    Status = TestMouseSimple();
    //Status = TestNotify();
   // Status = TestTimer();
    return Status;
}
