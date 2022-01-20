/** @file
 **/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextInEx.h>


/** example  
 *
 */
EFI_STATUS testTimeOut()
{
    EFI_STATUS Status;
    UINTN EventIndex = 0;
    EFI_EVENT Events[2] = {0}; 
    Events[0] = gST->ConIn->WaitForKey;
    Status = gBS->CreateEvent(EVT_TIMER  , TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &Events[1]);
    Status = gBS->SetTimer(Events[1],TimerPeriodic , 10 * 1000 * 1000);
    Status = gBS->WaitForEvent (2, Events, &EventIndex);
    if(EFI_SUCCESS == Status)
    {
        if(EventIndex == 0){
            //����������
        }else if(EventIndex == 1){
            // 1 �����޼��̲���
            Print(L"You don't input any key in 1 seconds!" );
        }
    }
    Status = gBS->CloseEvent(Events[1]);
    return Status;
}

/** example 1
 *
 */
void WaitKey()
{
    EFI_STATUS   Status = 0;
    UINTN        Index=0;
    EFI_INPUT_KEY  Key;

    Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key); 
}

/** example 2
 *
 */
INTN TimeOut()
{
    return 0;
}

EFI_STATUS TestTimer()
{
    EFI_STATUS  Status;
    EFI_EVENT myEvent;
    UINTN       index=0;
    UINTN       repeats=0;

    Print(L"Test EVT_TIMER  ");
    Status = gBS->CreateEvent(EVT_TIMER  , TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &myEvent);
    if(EFI_ERROR(Status)){
        Print(L"Fail EVT_TIMER | EVT_RUNTIME %r", Status);
        return Status;
    }
    Status = gBS->SetTimer(myEvent,TimerPeriodic , 15 * 1000 * 1000);
    while(1)
    {
        Status = gBS->WaitForEvent(1, &myEvent, &index);
        // do something
        TimeOut();
        Print(L"Time out!");
        if(repeats ++ == 10){
            break;
        }
    }
    Status = gBS->CloseEvent(myEvent);
    return EFI_SUCCESS;
}

/* exmpale 3
 *
 */
VOID myEventNoify (
        IN EFI_EVENT                Event,
        IN VOID                     *Context
        )
{
    static UINTN times = 0;
    Print(L"myEventNotif Wait %d\n", times);
    times ++;
    if(times >5)
        gBS->SignalEvent(Event);
}

EFI_STATUS TestNotify()
{
    EFI_STATUS  Status;
    UINTN       index=0;
    EFI_EVENT myEvent;

    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_NOTIFY, (EFI_EVENT_NOTIFY)myEventNoify , (VOID*)NULL, &myEvent);
    Status = gBS->WaitForEvent(1, &myEvent, &index);
    Status = gBS->CloseEvent(myEvent);
    return EFI_SUCCESS;
}

/* example 4
 *
 */
VOID
myEventNoify30 (
        IN EFI_EVENT                Event,
        IN VOID                     *Context
        )
{
    static UINTN times = 0;
    Print(L"%s\nmyEventNotif signal%d\n", Context, times);
    times ++;
}

EFI_STATUS TestEventSingal()
{
    EFI_STATUS  Status;
    EFI_EVENT myEvent;
    Print(L"Test EVT_TIMER | EVT_NOTIFY_SIGNAL");
    // ����Timer�¼��������ô�������
    Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)myEventNoify30, (VOID*)L"Hello! Time Out!", &myEvent);
    // ����Timer�ȴ�ʱ��Ϊ10�룬����Ϊѭ���ȴ�
    Status = gBS->SetTimer(myEvent,TimerPeriodic , 10 * 1000 * 1000);
    WaitKey();
    Status = gBS->CloseEvent(myEvent);
    return EFI_SUCCESS;
}

/* examle 5
 *
 */
EFI_STATUS 
testMouseSimple()
{
    EFI_STATUS  Status;
    EFI_SIMPLE_POINTER_PROTOCOL* mouse = 0;
    EFI_SIMPLE_POINTER_STATE     State;
    EFI_EVENT events[2]; // = {0, gST->ConIn->WaitForKey};
    //��ʾ���
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    //�ҳ�����豸
    Status = gBS->LocateProtocol(
            &gEfiSimplePointerProtocolGuid,
            NULL,
            (VOID**)&mouse
            );
    // ��������豸
    Status = mouse->Reset(mouse, TRUE);
    // ������¼��ŵ��ȴ��¼�����
    events[0] = mouse->WaitForInput;
    // �������¼��ŵ��ȴ�����
    events[1] = gST->ConIn->WaitForKey;
    while(1)
    {
        EFI_INPUT_KEY	   Key;
        UINTN index;
        // �ȴ�events�е���һ�¼�����  2���¼��������+���� indexΪ�����¼�������
        Status = gBS->WaitForEvent(2, events, &index);
        if(index == 0){
            // ��ȡ���״̬�����
            Status = mouse->GetState(mouse, &State);
            Print(L"X:%d Y:%d Z:%d L:%d R:%d\n",
                State.RelativeMovementX,
                State.RelativeMovementY,
                State.RelativeMovementZ,
                State.LeftButton,
                State.RightButton
                );
        } else{            
            Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
            // ����q�����˳�
            if (Key.UnicodeChar == 'q')
                break;
        }
    }
    return EFI_SUCCESS;
}


EFI_STATUS UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
{
    EFI_STATUS Status;
    Status = testTimeOut();
    Status = TestTimer(); 
    Status = TestNotify();
    Status = TestEventSingal();
    Status = testMouseSimple();
	return Status;
}
