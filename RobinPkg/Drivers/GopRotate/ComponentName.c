/* ComponentName.c
Copyright (c) 2015, Aaron Pop
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
#include "GopRotate.h"

EFI_STATUS EFIAPI
GraphicsOutputRotateComponentNameGetDriverName
(
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName);

EFI_STATUS EFIAPI
GraphicsOutputRotateComponentNameGetControllerName
(
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName);

/// Component Name Protocol instantiation 
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL gGraphicsOutputRotateComponentName = 
{
  GraphicsOutputRotateComponentNameGetDriverName,
  GraphicsOutputRotateComponentNameGetControllerName,
  "eng"
};

/// Component Name 2 Protocol instantiation
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gGraphicsOutputRotateComponentName2 = 
{
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) GraphicsOutputRotateComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) GraphicsOutputRotateComponentNameGetControllerName,
  "en"
};

/// Unicode string table, used to match a language identifier to a human readable string
GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mGraphicsOutputRotateDriverNameTable[] = 
{
  {
    "eng;en;en-us",
    (CHAR16 *)L"Graphics Output Rotate Driver"
  },
  {
    NULL,
    NULL
  }
};

/**
    Return a human readable unicode string that identifies this driver

    @param  This            Pointer to this CompnentName protocol instance
    @param  Language        Pointer to an ascii string representing the language
                            to return the driver name in
    @param  DriverName      Pointer to fill out with the driver name string

    @retval EFI_INVALID_PARAMETER The Language string or the Driver name pointers are null
    @retval EFI_UNSUPPORTED       No driver string exists in the passed language string

**/
EFI_STATUS
EFIAPI
GraphicsOutputRotateComponentNameGetDriverName 
(
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName)
{
  return LookupUnicodeString2(  Language,
                                This->SupportedLanguages,
                                mGraphicsOutputRotateDriverNameTable,
                                DriverName,
                                (BOOLEAN)(This == &gGraphicsOutputRotateComponentName));
}

/**
    Return a human readable string for the controller name passed 

    @param  This                    Pointer to the component name instance
    @param  ControllerHandle        The handle of the controller to get a name for
    @param  ChildHandle             Handle of the child device
    @param  Language                Pointer to an ascii string representing the language
                                    to return the controller name
    @param  ControllerName          Place to return the pointer to the unicode string

    @retval EFI_INVALID_PARAMETER   ControllerHandle, ChildHandale, Language or ControllerName
                                    pointers are NULL or invalid
    @retval EFI_UNSUPPORTED         The driver does not have a name string associated with it
**/
EFI_STATUS EFIAPI
GraphicsOutputRotateComponentNameGetControllerName 
(
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName)
{
  return EFI_UNSUPPORTED;
}
