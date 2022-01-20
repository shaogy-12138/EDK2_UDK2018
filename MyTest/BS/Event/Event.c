/*************************************************************************
	> File Name: Event.c
	> Author: Jason.shao
	> Mail: 
	> Created Time: Mon Jan 17 14:50:40 2022
 ************************************************************************/


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>



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

    Status = gBS->WaitForEvent(1, &myEvent, &index);
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
    //Status = TestNotify();
    Status = TestTimer();
    return Status;
}
