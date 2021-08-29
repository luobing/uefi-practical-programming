//Smbus.h:
//Code by luobing  
#ifndef MYSMBUS_H
#define MYSMBUS_H

#include "Common.h"

//*************************** Function Declaration ***************************************
UINT8 ReadSmbusByte(IN UINT8 SlaveAddress,IN UINT8 RegisterIndex);
VOID WriteSmbusByte(IN UINT8 SlaveAddress,IN UINT8 RegisterIndex,IN UINT8 Value);



#endif