/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "Network.h"
#include "KeyBoard.h"

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

typedef struct MyUDP4Socket{
		EFI_HANDLE                     m_SocketHandle;                   
		EFI_UDP4_PROTOCOL*             m_pUdp4Protocol;
		EFI_UDP4_CONFIG_DATA*          m_pUdp4ConfigData;
		EFI_UDP4_TRANSMIT_DATA*        m_TransData;
		EFI_UDP4_RECEIVE_DATA*         m_RecvData; 
		// EFI_UDP4_CLOSE_TOKEN           CloseToken;
		EFI_UDP4_COMPLETION_TOKEN              SendToken, RecvToken;
        // int stub;   //robin add for debug
}MYUDP4SOCKET;

static MYTCP4SOCKET* TCP4SocketFd[32];
static MYUDP4SOCKET *UDP4SocketFd[32];
//=================================================================TCP4 Function Begin===========================================
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
    Print(L"Init: CurSocket=%p TCP4SocketFd[index]=%p\n", CurSocket,TCP4SocketFd[index]);
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
    CurSocket->m_pTcp4ConfigData->TimeToLive = 16;    
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.RemoteAddress.Addr) = Ip32;
    CurSocket->m_pTcp4ConfigData->AccessPoint.RemotePort = Port;
    *(UINT32*)(CurSocket->m_pTcp4ConfigData->AccessPoint.SubnetMask.Addr) = (255 | 255 << 8 | 255 << 16 | 0 << 24) ;

    CurSocket->m_pTcp4ConfigData->AccessPoint.UseDefaultAddress = TRUE;

    /// if UseDefaultAddress is FALSE， config StationAddress 
    CurSocket->m_pTcp4ConfigData->AccessPoint.StationPort = 61558;
    CurSocket->m_pTcp4ConfigData->AccessPoint.ActiveFlag = TRUE;
    CurSocket->m_pTcp4ConfigData->ControlOption = NULL;
    Status = CurSocket->m_pTcp4Protocol ->Configure(CurSocket->m_pTcp4Protocol, CurSocket->m_pTcp4ConfigData);    
    return Status;
}

EFI_STATUS SendTCP4Socket(UINTN index, CHAR8* Data, UINTN Lenth)
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
    CurSocket->m_TransData->DataLength = (UINT32)Lenth;
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
    Print(L"Send: WaitForEvent, %r\n", Status);
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
    Print(L"Recv: WaitForEvent, %r\n", Status);
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
    Print(L"Connect: WaitForEvent, %r\n", Status);
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
    if(CurSocket->m_TransData){
	    FreePool(CurSocket->m_TransData);
	    CurSocket->m_TransData = NULL;
    }
    if(CurSocket->m_RecvData){
	    FreePool(CurSocket->m_RecvData);
	    CurSocket->m_RecvData = NULL;
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

//=================================================================TCP4 Function End===========================================
// stub funciton
VOID UdpNopNoify (  IN EFI_EVENT  Event,  IN VOID *Context  )
{
}

VOID  Udp4SendNotify(IN EFI_EVENT  Event,  IN VOID *Context)
{
    // MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

    // Print(L"Tcp4SendNotify: stub=%x\n", (int)CurSocket->stub);
    // Print(L"Tcp4SendNotify: Context=%p\n", Context);
}
VOID  Udp4RecvNotify(IN EFI_EVENT  Event,  IN VOID *Context)
{
    // MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);
    Print(L"Udp4RecvNotify is called.\n");
    // Print(L"Tcp4RecvNotify: stub=%x\n", (int)CurSocket->stub);
    // Print(L"Tcp4RecvNotify: Context=%p\n", Context);
}

UINTN CreateUDP4Socket(VOID)
{
    EFI_STATUS                           Status;
    EFI_SERVICE_BINDING_PROTOCOL*  pUdpServiceBinding;
    MYUDP4SOCKET *CurSocket = NULL;
    INTN i;
    INTN MyFd = -1;

    for (i = 0; i < 32; i++)
    {
        if(UDP4SocketFd[i]==NULL)
        {
            CurSocket=(MYUDP4SOCKET *) AllocatePool(sizeof(MYUDP4SOCKET));
            UDP4SocketFd[i] = CurSocket;
            MyFd = i;
            break;
        }
    }

    if(CurSocket==NULL)
        return MyFd;
    
    gBS->SetMem((void*)CurSocket, 0, sizeof(MYUDP4SOCKET));        
    CurSocket->m_SocketHandle              = NULL;
    Status = gBS->LocateProtocol ( &gEfiUdp4ServiceBindingProtocolGuid,
        NULL,
        (VOID **)&pUdpServiceBinding );
    if(EFI_ERROR(Status))
        return Status;

    Status = pUdpServiceBinding->CreateChild ( pUdpServiceBinding,
        &CurSocket->m_SocketHandle );
    if(EFI_ERROR(Status))
        return Status;

    Status = gBS->OpenProtocol ( CurSocket->m_SocketHandle,
        &gEfiUdp4ProtocolGuid,
        (VOID **)&CurSocket->m_pUdp4Protocol,
        gImageHandle,
        CurSocket->m_SocketHandle,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL );
        
    if(EFI_ERROR(Status))
         return Status;
    InitUdp4SocketFd(MyFd);

    return MyFd;
}


EFI_STATUS InitUdp4SocketFd(INTN index)
{
    EFI_STATUS                           Status;
    MYUDP4SOCKET *CurSocket = UDP4SocketFd[index];

    //1 Create Configure data
    CurSocket->m_pUdp4ConfigData = (EFI_UDP4_CONFIG_DATA*)AllocatePool(sizeof(EFI_UDP4_CONFIG_DATA));
    // CurSocket->stub = 0x1212;
    //2 Create Connect Event
    // // CurSocket->ConnectToken.CompletionToken.Status = EFI_ABORTED;
    // Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->ConnectToken, &CurSocket->ConnectToken.CompletionToken.Event );
    // if(EFI_ERROR(Status)) return Status;  

    //3 Create Transmit Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Udp4SendNotify , (VOID*)CurSocket, &CurSocket->SendToken.Event);
    Print(L"Init: CurSocket=%p UDP4SocketFd[index]=%p\n", CurSocket,UDP4SocketFd[index]);
    if(EFI_ERROR(Status)) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Send Event fail!\n\r");
        return Status;     
    }
    // CurSocket->SendToken.CompletionToken.Status  =EFI_ABORTED; 
    CurSocket->m_TransData = (EFI_UDP4_TRANSMIT_DATA*)AllocatePool(sizeof(EFI_UDP4_TRANSMIT_DATA));

    //4 Create Recv Event
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Udp4RecvNotify , (VOID*)CurSocket, &CurSocket->RecvToken.Event);
    if(EFI_ERROR(Status)) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Recv Event fail!\n\r");
        return Status;
    }   
    // CurSocket->RecvToken.CompletionToken.Status  =EFI_ABORTED;
    CurSocket->m_RecvData = (EFI_UDP4_RECEIVE_DATA*) AllocatePool(sizeof(EFI_UDP4_RECEIVE_DATA));;
    
    //5 Create Close Event
    // CurSocket->CloseToken.CompletionToken.Status = EFI_ABORTED;
    // Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->CloseToken, &CurSocket->CloseToken.CompletionToken.Event );
    // if(EFI_ERROR(Status))
    // {
    //     gST->ConOut->OutputString(gST->ConOut,L"Init: Create Close Event fail!\n\r");
    //     return Status;
    // }
    return Status;
}

EFI_STATUS ConfigUDP4Socket(UINTN index, UINT32 Ip32, UINT16 Port)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYUDP4SOCKET *CurSocket = UDP4SocketFd[index];

    if(CurSocket->m_pUdp4ConfigData == NULL)
        return Status;
    CurSocket->m_pUdp4ConfigData->AcceptBroadcast = TRUE;  //Accept broadcast UDP packets
    CurSocket->m_pUdp4ConfigData->AcceptPromiscuous = TRUE; //Accetp UDP packets that are send to any address
    CurSocket->m_pUdp4ConfigData->AcceptAnyPort = FALSE;
    CurSocket->m_pUdp4ConfigData->AllowDuplicatePort = FALSE;
    CurSocket->m_pUdp4ConfigData->TypeOfService = 0;
    CurSocket->m_pUdp4ConfigData->TimeToLive = 16;
    CurSocket->m_pUdp4ConfigData->UseDefaultAddress = TRUE;
    *(UINTN*)(CurSocket->m_pUdp4ConfigData->RemoteAddress.Addr ) = Ip32;
    CurSocket->m_pUdp4ConfigData->RemotePort = Port;
    // *(UINT32*)(CurSocket->m_pTcp4ConfigData->AccessPoint.SubnetMask.Addr) = (255 | 255 << 8 | 255 << 16 | 0 << 24) ;

    // /// if UseDefaultAddress is FALSE， config StationAddress 
    // CurSocket->m_pTcp4ConfigData->AccessPoint.StationPort = 61558;
    // CurSocket->m_pTcp4ConfigData->AccessPoint.ActiveFlag = TRUE;
    // CurSocket->m_pTcp4ConfigData->ControlOption = NULL;
    Status = CurSocket->m_pUdp4Protocol ->Configure(CurSocket->m_pUdp4Protocol, CurSocket->m_pUdp4ConfigData);    
    return Status;
}


EFI_STATUS SendUDP4Socket(UINTN index, CHAR8* Data, UINTN Length)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYUDP4SOCKET *CurSocket = UDP4SocketFd[index];
    UINTN waitIndex = 0;

    if(CurSocket->m_pUdp4Protocol == NULL) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Send: m_Tcp4Protocol is NULL\n\r");
        return Status;  
    }
    CurSocket->m_TransData->UdpSessionData = NULL;
    CurSocket->m_TransData->GatewayAddress = NULL;
    CurSocket->m_TransData->DataLength = (UINT32)Length;
    CurSocket->m_TransData->FragmentCount = 1;
    CurSocket->m_TransData->FragmentTable[0].FragmentLength = (UINT32)Length;
    CurSocket->m_TransData->FragmentTable[0].FragmentBuffer = Data;
    CurSocket->SendToken.Packet.TxData = CurSocket->m_TransData;
    Status = CurSocket->m_pUdp4Protocol->Transmit(CurSocket->m_pUdp4Protocol, &CurSocket->SendToken);

    // CurSocket->m_TransData->Push = TRUE;
    // CurSocket->m_TransData->Urgent = TRUE;
    // CurSocket->m_TransData->DataLength = (UINT32)Lenth;
    // CurSocket->m_TransData->FragmentCount = 1;
    // CurSocket->m_TransData->FragmentTable[0].FragmentLength =CurSocket->m_TransData->DataLength;
    // CurSocket->m_TransData->FragmentTable[0].FragmentBuffer =Data;
    // CurSocket->SendToken.Packet.TxData=  CurSocket->m_TransData;
    // Status = CurSocket->m_pTcp4Protocol -> Transmit(CurSocket->m_pTcp4Protocol, &CurSocket->SendToken);

    if(EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut,L"Send: Transmit fail!\n\r");
        return Status;
    }
        
        
	Status = gBS->WaitForEvent(1, &(CurSocket->SendToken.Event), &waitIndex);
    Print(L"Send: WaitForEvent, %r\n", Status);
    return CurSocket->SendToken.Status;
}

EFI_STATUS RecvUDP4Socket(IN UINTN index, IN CHAR8* Buffer1, IN CHAR8* Buffer2,IN UINTN Length, OUT UINTN *recvLength, UINT32 SrcIp32, UINT16 SrcPort)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYUDP4SOCKET *CurSocket = UDP4SocketFd[index];
    UINTN waitIndex = 0;

    if(CurSocket->m_pUdp4Protocol == NULL) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Recv: m_Udp4Protocol is NULL\n\r");
        return Status;  
    }


    CurSocket->m_RecvData->DataLength = (UINT32)Length*2;
    CurSocket->m_RecvData->FragmentCount = 2;
    CurSocket->m_RecvData->FragmentTable[0].FragmentLength = (UINT32)Length ;
    CurSocket->m_RecvData->FragmentTable[0].FragmentBuffer = (void*)Buffer1;
    CurSocket->m_RecvData->FragmentTable[1].FragmentLength = (UINT32)Length ;
    CurSocket->m_RecvData->FragmentTable[1].FragmentBuffer = (void*)Buffer2;
    
    *(UINTN*)(CurSocket->m_RecvData->UdpSession.SourceAddress.Addr ) = SrcIp32;
    CurSocket->m_RecvData->UdpSession.SourcePort = SrcPort;
    CurSocket->RecvToken.Packet.RxData=  CurSocket->m_RecvData;
    Status = gBS->WaitForEvent(1, &(CurSocket->RecvToken.Event), &waitIndex);
    Print(L"Recv: WaitForEvent, %r\n", Status);

    Status = CurSocket->m_pUdp4Protocol -> Receive(CurSocket->m_pUdp4Protocol, &CurSocket->RecvToken);
    if(EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut,L"Recv: Receive fail!\n\r");
        return Status;
    }
    
    *recvLength = CurSocket->m_RecvData->DataLength;
   
    return CurSocket->RecvToken.Status;
}

INTN DestroyUDP4Socket(UINTN index)
{
    EFI_STATUS Status;
    MYUDP4SOCKET *CurSocket = UDP4SocketFd[index];
    
    //Print(L"DestroyUDP4Socket 1 ");
    //WaitKey();
    if(CurSocket->m_SocketHandle){
        EFI_SERVICE_BINDING_PROTOCOL*  pUdpServiceBinding;
        Status = gBS->LocateProtocol ( &gEfiUdp4ServiceBindingProtocolGuid,
                NULL, (VOID **)&pUdpServiceBinding );
/*        Print(L"DestroyUDP4Socket:LocateProtocol %r ", Status);
        WaitKey();  */      
        Status = pUdpServiceBinding->DestroyChild ( pUdpServiceBinding,
                CurSocket->m_SocketHandle );
    }
    // Print(L"DestroyUDP4Socket 2 ");
    // WaitKey();
    // if(CurSocket->ConnectToken.CompletionToken.Event)
    //     gBS->CloseEvent(CurSocket->ConnectToken.CompletionToken.Event);    
    if(CurSocket->SendToken.Event)
        gBS->CloseEvent(CurSocket->SendToken.Event);    
    if(CurSocket->RecvToken.Event)
        gBS->CloseEvent(CurSocket->RecvToken.Event);
    // Print(L"DestroyUDP4Socket 3 ");
    // WaitKey();
    if(CurSocket->m_pUdp4ConfigData){
	    FreePool(CurSocket->m_pUdp4ConfigData);
    }
    // Print(L"DestroyUDP4Socket 4 ");
    // WaitKey();
    if(CurSocket->m_TransData){
	    FreePool(CurSocket->m_TransData);
	    CurSocket->m_TransData = NULL;
    }
    // Print(L"DestroyUDP4Socket 5 ");
    // WaitKey();
    if(CurSocket->m_RecvData){
	    FreePool(CurSocket->m_RecvData);
	    CurSocket->m_RecvData = NULL;
    }
    // Print(L"DestroyUDP4Socket 6 ");
    // WaitKey();
    return 0;
}

INTN CloseUDP4Socket(UINTN index)
{
    // EFI_STATUS Status;
    MYUDP4SOCKET *CurSocket = UDP4SocketFd[index];
	// Status = CurSocket -> m_pUdp4Protocol -> Close(CurSocket->m_pTcp4Protocol, &CurSocket->CloseToken);

	// Status = CurSocket->m_pUdp4Protocol ->Configure(CurSocket->m_pUdp4Protocol, NULL);  
	DestroyUDP4Socket(index);
	FreePool(CurSocket);
	UDP4SocketFd[index] = NULL;

	return 0;
}