## @file
#
#  Copyright (c) 2018 Loongson Technology Corporation Limited (www.loongson.cn).
#  All intellectual property rights(Copyright, Patent and Trademark) reserved.
#
#  Any violations of copyright or other intellectual property rights of the Loongson Technology
#  Corporation Limited will be held accountable in accordance with the law,
#  if you (or any of your subsidiaries, corporate affiliates or agents) initiate
#  directly or indirectly any Intellectual Property Assertion or Intellectual Property Litigation:
#  (i) against Loongson Technology Corporation Limited or any of its subsidiaries or corporate affiliates,
#  (ii) against any party if such Intellectual Property Assertion or Intellectual Property Litigation arises
#  in whole or in part from any software, technology, product or service of Loongson Technology Corporation
#  Limited or any of its subsidiaries or corporate affiliates, or (iii) against any party relating to the Software.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
#  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION).
#
##

#!/bin/bash

#######################################################################
#
# 1: make BaseTools and copy BuildEnv.sh build_rule.txt tools_def.txt to
#   Conf directory
# 2: set environment path
# 3: source edksetup.sh BaseTools
# 4: make souce code
# 5: generate *efi file
# 6: generate *fd file
#
#######################################################################
export WORKSPACE=$PWD
export EDK_TOOLS_PATH=$PWD/BaseTools/
export GCC44_MIPS64EL_PREFIX=~/loongson/gcc-4.4.0-pmon/bin/mipsel-linux-
export PATH=$PATH:~/loongson/gcc-4.4.0-pmon/bin
export LD_LIBRARY_PATH=~/loongson/gcc-4.4.0-pmon/lib:$LD_LIBRARY_PATH
# source edksetup.sh BaseTools
#PLATFORMFILE=AppPkg/AppPkg.dsc
# PLATFORMFILE=MyPkg/MyPkg.dsc
# #COMPILER_VERSION=GCC44
# COMPILER_VERSION=GCC44

# #VERSION=DEBUG
# VERSION=RELEASE

# echo Runing Edk2 Build for Loongson...
# build -a MIPS64EL -p  $PLATFORMFILE -t $COMPILER_VERSION -b $VERSION -j log.txt

# exit $?
