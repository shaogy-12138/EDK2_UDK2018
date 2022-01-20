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
	events[1] = gST->ConIn->WaitForKey;

	while(1)
	{
		EFI_INPUT_KEY	   Key;
		UINTN index;
		// �ȴ�events�е���һ�¼�����
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
	return testMouseSimple(); 
}
