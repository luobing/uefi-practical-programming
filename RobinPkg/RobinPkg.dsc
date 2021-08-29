## @file
#   Robin's Development Kit 
#
#   See the comments in the [LibraryClasses.IA32] and [BuildOptions] sections
#   for important information about configuring this package for your
#   environment.

##

[Defines]
  PLATFORM_NAME                  = RobinPkg
  PLATFORM_GUID                  = 0348cade-8a6e-4ee5-b773-1b2ccbda3e06
  PLATFORM_VERSION               = 0.01
  DSC_SPECIFICATION              = 0x00010006
  OUTPUT_DIRECTORY               = Build/RobinPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64|MIPS64EL|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

#
#  Debug output control
#
  DEFINE DEBUG_ENABLE_OUTPUT      = FALSE       # Set to TRUE to enable debug output
  DEFINE DEBUG_PRINT_ERROR_LEVEL  = 0x80000040  # Flags to control amount of debug output
  DEFINE DEBUG_PROPERTY_MASK      = 0

[PcdsFeatureFlag]

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|$(DEBUG_PROPERTY_MASK)
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|$(DEBUG_PRINT_ERROR_LEVEL)

[PcdsFixedAtBuild.IPF]

[LibraryClasses]
  #
  # Entry Point Libraries
  #
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  #
  # Common Libraries
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  !if $(DEBUG_ENABLE_OUTPUT)
    DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
    DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  !else   ## DEBUG_ENABLE_OUTPUT
    DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  !endif  ## DEBUG_ENABLE_OUTPUT
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf  
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf

  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf

  #lbdebug
	DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components]

#### Sample Applications and Drivers.
  
  RobinPkg/Applications/CppMain/CppMain.inf            		#chap03
  RobinPkg/Applications/TestProtocol/TestProtocol.inf  		#chap04
  RobinPkg/Applications/pixelCHS/pixelCHS.inf          		#chap04
  RobinPkg/Applications/HiiCHS/HiiCHS.inf              		#chap04
  RobinPkg/Applications/ShowBMP/ShowBMP.inf            		#chap05
  RobinPkg/Applications/ShowPCX/ShowPCX.inf            		#chap05
  RobinPkg/Applications/ShowJPEG/ShowJPEG.inf          		#chap05
  RobinPkg/Applications/HiiPicture/HiiPicture.inf      		#chap05
  RobinPkg/Applications/ImageEffect/ImageEffect.inf    		#chap05
  RobinPkg/Applications/RngEvent/RngEvent.inf          		#chap06
  RobinPkg/Applications/GuiBaseSample/GuiBaseSample.inf		#chap06
  RobinPkg/Applications/MyGuiFrame/MyGuiFrame.inf      		#chap06
  RobinPkg/Applications/UefiGuiLite/UefiGuiLite.inf    		#chap06
  RobinPkg/Applications/ListPCIMsg/ListPCIMsg.inf      		#chap07
  RobinPkg/Applications/ListSPD/ListSPD.inf            		#chap07
  RobinPkg/Applications/RwUart/RwUart.inf              		#chap07
  RobinPkg/Drivers/ServiceDrv/ServiceDrv.inf              #chap08
  RobinPkg/Applications/TestServiceDrv/TestServiceDrv.inf #chap08
  RobinPkg/Drivers/GopRotate/GopRotate.inf                #chap08
  RobinPkg/Applications/TestGopRotate/TestGopRotate.inf   #chap08
  RobinPkg/Drivers/MyOprom/MyOprom.inf										#chap08
  RobinPkg/Applications/ListUSB/ListUSB.inf               #chap09
  RobinPkg/Applications/HelloHid/HelloHid.inf             #chap09
  RobinPkg/Applications/EchoTcp4/EchoTcp4.inf          		#chap10
  RobinPkg/Applications/EchoUdp4/EchoUdp4.inf          		#chap10
  RobinPkg/Applications/stdEchoTcp4/stdEchoTcp4.inf    		#chap10
  RobinPkg/Applications/stdEchoUdp4/stdEchoUdp4.inf    		#chap10
  RobinPkg/Applications/stdEchoTcp6/stdEchoTcp6.inf    		#chap10
  RobinPkg/Applications/stdEchoUdp6/stdEchoUdp6.inf    		#chap10

 

  



##############################################################################
#
# Specify whether we are running in an emulation environment, or not.
# Define EMULATE if we are, else keep the DEFINE commented out.
#
# DEFINE  EMULATE = 1

##############################################################################
#
#  Include Boilerplate text required for building with the Standard Libraries.
#
##############################################################################
!include StdLib/StdLib.inc

