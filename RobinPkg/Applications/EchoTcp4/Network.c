/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Network.h"


typedef struct MyTCP4Socket{
		EFI_HANDLE                     m_SocketHandle;                   
		EFI_TCP4_PROTOCOL*             m_pTcp4Protocol;
		EFI_TCP4_CONFIG_DATA*          m_pTcp4ConfigData;
		EFI_TCP4_TRANSMIT_DATA*        m_TransData;
		EFI_TCP4_RECEIVE_DATA*         m_RecvData; 
		EFI_TCP4_CONNECTION_TOKEN      ConnectToken;
		EFI_TCP4_CLOSE_TOKEN           CloseToken;
		EFI_TCP4_IO_TOKEN              SendToken, RecvToken;
        // int stub;   //robin add for debug
}MYTCP4SOCKET;

static MYTCP4SOCKET* TCP4SocketFd[32];

// stub funciton
VOID NopNoify (  IN EFI_EVENT  Event,  IN VOID *Context  )
{
}

VOID  Tcp4SendNotify(IN EFI_EVENT  Event,  IN VOID *Context)
{
    // MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

    // Print(L"Tcp4SendNotify: stub=%x\n", (int)CurSocket->stub);
    // Print(L"Tcp4SendNotify: Context=%p\n", Context);
}
VOID  Tcp4RecvNotify(IN EFI_EVENT  Event,  IN VOID *Context)
{
    // MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

    // Print(L"Tcp4RecvNotify: stub=%x\n", (int)CurSocket->stub);
    // Print(L"Tcp4RecvNotify: Context=%p\n", Context);
}
EFI_STATUS InitTcp4SocketFd(INTN index)
{
    EFI_STATUS                           Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];

    //1 Create Configure data
    CurSocket->m_pTcp4ConfigData = (EFI_TCP4_CONFIG_DATA*)AllocatePool(sizeof(EFI_TCP4_CONFIG_DATA));
    // CurSocket->stub = 0x1212;
    //2 Create Connect Event
    // CurSocket->ConnectToken.CompletionToken.Status = EFI_ABORTED;
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->ConnectToken, &CurSocket->ConnectToken.CompletionToken.Event );
    if(EFI_ERROR(Status)) return Status;  

    //3 Create Transmit Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Tcp4SendNotify , (VOID*)CurSocket, &CurSocket->SendToken.CompletionToken.Event);
    // Print(L"Init: CurSocket=%p TCP4SocketFd[index]=%p\n", CurSocket,TCP4SocketFd[index]);
    if(EFI_ERROR(Status)) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Send Event fail!\n\r");
        return Status;     
    }
    // CurSocket->SendToken.CompletionToken.Status  =EFI_ABORTED; 
    CurSocket->m_TransData = (EFI_TCP4_TRANSMIT_DATA*)AllocatePool(sizeof(EFI_TCP4_TRANSMIT_DATA));

    //4 Create Recv Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Tcp4RecvNotify , (VOID*)CurSocket, &CurSocket->RecvToken.CompletionToken.Event);
    // CurSocket->RecvToken.CompletionToken.Status  =EFI_ABORTED;
    CurSocket->m_RecvData = (EFI_TCP4_RECEIVE_DATA*) AllocatePool(sizeof(EFI_TCP4_RECEIVE_DATA));;
    if(EFI_ERROR(Status)) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Recv Event fail!\n\r");
        return Status;
    }   
    //5 Create Close Event
    // CurSocket->CloseToken.CompletionToken.Status = EFI_ABORTED;
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->CloseToken, &CurSocket->CloseToken.CompletionToken.Event );
    if(EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Close Event fail!\n\r");
        return Status;
    }
    return Status;
}

UINTN CreateTCP4Socket(VOID)
{
    EFI_STATUS                           Status;
    EFI_SERVICE_BINDING_PROTOCOL*  pTcpServiceBinding;
    MYTCP4SOCKET *CurSocket = NULL;
    INTN i;
    INTN MyFd = -1;

    for (i = 0; i < 32; i++)
    {
        if(TCP4SocketFd[i]==NULL)
        {
            CurSocket=(MYTCP4SOCKET *) AllocatePool(sizeof(MYTCP4SOCKET));
            TCP4SocketFd[i] = CurSocket;
            MyFd = i;
            break;
        }
    }

    if(CurSocket==NULL)
        return MyFd;
    
    gBS->SetMem((void*)CurSocket, 0, sizeof(MYTCP4SOCKET));        
    CurSocket->m_SocketHandle              = NULL;
    Status = gBS->LocateProtocol ( &gEfiTcp4ServiceBindingProtocolGuid,
        NULL,
        (VOID **)&pTcpServiceBinding );
    if(EFI_ERROR(Status))
        return Status;

    Status = pTcpServiceBinding->CreateChild ( pTcpServiceBinding,
        &CurSocket->m_SocketHandle );
    if(EFI_ERROR(Status))
        return Status;

    Status = gBS->OpenProtocol ( CurSocket->m_SocketHandle,
        &gEfiTcp4ProtocolGuid,
        (VOID **)&CurSocket->m_pTcp4Protocol,
        gImageHandle,
        CurSocket->m_SocketHandle,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL );
        
    if(EFI_ERROR(Status))
         return Status;
    InitTcp4SocketFd(MyFd);

    return MyFd;
}


EFI_STATUS ConfigTCP4Socket(UINTN index, UINT32 Ip32, UINT16 Port)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];

    if(CurSocket->m_pTcp4ConfigData == NULL)
        return Status;
    CurSocket->m_pTcp4ConfigData->TypeOfService = 0;
    CurSocket->m_pTcp4ConfigData->TimeToLive = 64;    
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.RemoteAddress.Addr) = Ip32;
    CurSocket->m_pTcp4ConfigData->AccessPoint.RemotePort = Port;
    *(UINT32*)(CurSocket->m_pTcp4ConfigData->AccessPoint.SubnetMask.Addr) = (255 | 255 << 8 | 255 << 16 | 0 << 24) ;

    CurSocket->m_pTcp4ConfigData->AccessPoint.UseDefaultAddress = TRUE;

    /// if UseDefaultAddress is FALSE， config StationAddress 
    CurSocket->m_pTcp4ConfigData->AccessPoint.StationPort = 60000;
    CurSocket->m_pTcp4ConfigData->AccessPoint.ActiveFlag = TRUE;
    CurSocket->m_pTcp4ConfigData->ControlOption = NULL;
    Status = CurSocket->m_pTcp4Protocol ->Configure(CurSocket->m_pTcp4Protocol, CurSocket->m_pTcp4ConfigData);    
    return Status;
}

EFI_STATUS SendTCP4Socket(UINTN index, CHAR8* Data, UINTN Length)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];
    UINTN waitIndex = 0;

    if(CurSocket->m_pTcp4Protocol == NULL) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Send: m_Tcp4Protocol is NULL\n\r");
        return Status;  
    }
    CurSocket->m_TransData->Push = TRUE;
    CurSocket->m_TransData->Urgent = TRUE;
    CurSocket->m_TransData->DataLength = (UINT32)Length;
    CurSocket->m_TransData->FragmentCount = 1;
    CurSocket->m_TransData->FragmentTable[0].FragmentLength =CurSocket->m_TransData->DataLength;
    CurSocket->m_TransData->FragmentTable[0].FragmentBuffer =Data;
    CurSocket->SendToken.Packet.TxData=  CurSocket->m_TransData;
    Status = CurSocket->m_pTcp4Protocol -> Transmit(CurSocket->m_pTcp4Protocol, &CurSocket->SendToken);

    if(EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut,L"Send: Transmit fail!\n\r");
        return Status;
    }
        
        
	Status = gBS->WaitForEvent(1, &(CurSocket->SendToken.CompletionToken.Event), &waitIndex);
    // Print(L"Send: WaitForEvent, %r\n", Status);
    return CurSocket->SendToken.CompletionToken.Status;
}

EFI_STATUS RecvTCP4Socket(IN UINTN index, IN CHAR8* Buffer, IN UINTN Length, OUT UINTN *recvLength)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];
    UINTN waitIndex = 0;

    if(CurSocket->m_pTcp4Protocol == NULL) return Status;

    CurSocket->m_RecvData->UrgentFlag = TRUE;
    CurSocket->m_RecvData->DataLength = (UINT32)Length;
    CurSocket->m_RecvData->FragmentCount = 1;
    CurSocket->m_RecvData->FragmentTable[0].FragmentLength = CurSocket->m_RecvData->DataLength ;
    CurSocket->m_RecvData->FragmentTable[0].FragmentBuffer = (void*)Buffer;
    CurSocket->RecvToken.Packet.RxData=  CurSocket->m_RecvData;
    Status = CurSocket->m_pTcp4Protocol -> Receive(CurSocket->m_pTcp4Protocol, &CurSocket->RecvToken);
    if(EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut,L"Recv: Receive fail!\n\r");
        return Status;
    }
    Status = gBS->WaitForEvent(1, &(CurSocket->RecvToken.CompletionToken.Event), &waitIndex);
    // Print(L"Recv: WaitForEvent, %r\n", Status);
    *recvLength = CurSocket->m_RecvData->DataLength;
   
    return CurSocket->RecvToken.CompletionToken.Status;
}

EFI_STATUS ConnectTCP4Socket(UINTN index, UINT32 Ip32, UINT16 Port)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];
    UINTN waitIndex=0;

    ConfigTCP4Socket(index, Ip32, Port);

    if(CurSocket->m_pTcp4Protocol == NULL) return Status; 
    Status = CurSocket->m_pTcp4Protocol -> Connect(CurSocket->m_pTcp4Protocol, &CurSocket->ConnectToken);
    if(EFI_ERROR(Status))
        return Status;

    Status = gBS->WaitForEvent(1, &(CurSocket->ConnectToken.CompletionToken.Event), &waitIndex);
    // Print(L"Connect: WaitForEvent, %r\n", Status);
    // if( !EFI_ERROR(Status)){
    //     gST->ConOut->OutputString(gST->ConOut,L"Connect: WaitForEvent fail!\n\r");
    //     Status = CurSocket->ConnectToken.CompletionToken.Status;
    // }
    return Status;
}

INTN DestroyTCP4Socket(UINTN index)
{
    EFI_STATUS Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];
    
    if(CurSocket->m_SocketHandle){
        EFI_SERVICE_BINDING_PROTOCOL*  pTcpServiceBinding;
        Status = gBS->LocateProtocol ( &gEfiTcp4ServiceBindingProtocolGuid,
                NULL, (VOID **)&pTcpServiceBinding );
        Status = pTcpServiceBinding->DestroyChild ( pTcpServiceBinding,
                CurSocket->m_SocketHandle );
    }
    if(CurSocket->ConnectToken.CompletionToken.Event)
        gBS->CloseEvent(CurSocket->ConnectToken.CompletionToken.Event);    
    if(CurSocket->SendToken.CompletionToken.Event)
        gBS->CloseEvent(CurSocket->SendToken.CompletionToken.Event);    
    if(CurSocket->RecvToken.CompletionToken.Event)
        gBS->CloseEvent(CurSocket->RecvToken.CompletionToken.Event);
    
    if(CurSocket->m_pTcp4ConfigData){
	    FreePool(CurSocket->m_pTcp4ConfigData);
    }
    if(CurSocket->SendToken.Packet.TxData){
	    FreePool(CurSocket->SendToken.Packet.TxData);
	    CurSocket->SendToken.Packet.TxData = NULL;
    }
    if(CurSocket->RecvToken.Packet.RxData){
	    FreePool(CurSocket->RecvToken.Packet.RxData);
	    CurSocket->RecvToken.Packet.RxData = NULL;
    }
    return 0;
}

EFI_STATUS CloseTCP4Socket(UINTN index)
{
    EFI_STATUS Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd[index];
	Status = CurSocket -> m_pTcp4Protocol -> Close(CurSocket->m_pTcp4Protocol, &CurSocket->CloseToken);
	
	DestroyTCP4Socket(index);
	FreePool(CurSocket);
	TCP4SocketFd[index] = NULL;

	return Status;
}
