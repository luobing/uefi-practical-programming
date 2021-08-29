#  **UEFI编程实践** 

## 介绍
笔者有一部分工作职责，是开发Option ROM。从Legacy BIOS时代，转到现在的UEFI BIOS，走了不少的弯路。为了更好地学习UEFI的知识，在业余期间，笔者以“UEFI开发探索”为名，撰写了UEFI开发的系列博客。

在此期间，认识了很多行业相关的朋友，在产品开发、市场推广方面，有过不少的探讨。

Legacy BIOS一直使用汇编语言编程，各家的架构不一，三十年来（从1981年IBM推出第一台个人电脑算起）一直是难度较高，分化比较严重的领域。特别是对我们这种第三方开发者来说，体会更为深刻。

UEFI的推出，改变了一切。以C语言为主的开发方式，降低了学习门槛，开发效率相对更高；优良的架构提供了很好的扩展性和安全性。从某种角度来看，UEFI已经是设计良好的嵌入式系统了，只不过它是连接硬件和现代操作系统的BIOS架构。

因此，在国产自主计算机从前几年开始批量出货，看到大部分BIOS采用了UEFI架构时，我一点都不感到奇怪。学习过程中，我萌生了将日常的实践经验记录下来，并集结成册的想法。希望能为UEFI的推广，特别是目前还处于早期发展阶段的国产计算机，贡献自己的一份力量。

经过了一年多的努力，奋斗的成果就是《UEFI编程实践》。

![UEFI编程实践-封面照](https://images.gitee.com/uploads/images/2021/0829/094740_cbbe1424_791211.png "gitee_cover.png")

## 仓库代码说明

本仓库中存放了《UEFI编程实践》的所有附书代码，内容如下。

*  :file_folder:  **/chap02**  第2章用到的工程文件，主要是Visual Studio和Windbg进行调试时使用。
*  :file_folder:  **/chap03**  第3章用到的工程文件，UEFI程序和库所使用的框架代码。
*  :file_folder:  **/chap04**  第4章用到的程序和工具，用于汉字提取。
*  :file_folder:  **/chap08**  第8章用到的工程文件，BlankDrv是Intel提供的驱动示例工程。
*  :open_file_folder:  **/chap09**  第9章用到的程序和工具。<br>
   :wrench: **UsbHID.exe**   _用于USB HID双向通信的上位机程序，运行于Windows系统_ <br>
   :bookmark_tabs: **STM32F4_UEFI**   _实现USB HID设备的代码，硬件为正点原子的F4探索者开发板。书中没有用到，仅做读者参考。_ <br>
   :bookmark_tabs: **YIE002STM32F1-UsbHID**   _实现USB HID设备的代码，硬件为YIE002开发板。书中用到的示例程序。_<br>
   :pushpin: _提示：本章两个嵌入式工程项目，是使用MDK-ARM 5.14.0.0开发的。_<br>
*  :open_file_folder:  **/chap10**  第10章用到的程序，主要用于网络通信。<br>
   :bookmark_tabs: **LinuxIPV6**  _Linux下进行IPV6通信的TCP和UDP代码，包括服务端和客户端程序。笔者在Ubuntu 18.04下开发测试。_<br>
   :bookmark_tabs: **WindowsIPV4**  _Windows下进行IPV4通信的TCP代码，包括服务端和客户端程序。_<br>
   :bookmark_tabs: **WindowsIPV6**  _Windows下进行IPV6通信的TCP和UDP代码，包括服务端和客户端程序。_<br>
   :pushpin: _提示：本章两个Windows项目，是使用Visual Studio 2015开发的。_<br>
*  :file_folder:  **/chap11**  第11章用到的文件，主要是龙芯下的汇编代码和批处理文件。
*  :file_folder:  **/chap12**  第12章用到的文件，UEFI的ARM编译环境用到的批处理文件。
*  :file_folder:  **/RobinPkg**  各章节的示例程序，包括UEFI应用和UEFI驱动，共计29个示例，均在此文件夹下。
*  :file_folder:  **/RobinPkg生成文件**   为方便实验，笔者将RobinPkg中的示例编译好了。分为IA32和X64两种，可以直接使用。
*  :page_facing_up:  [勘误.md](https://github.com/luobing/uefi-practical-programming/blob/main/%E5%8B%98%E8%AF%AF.md)  对《UEFI编程实践》的勘误记录 

## 使用教程

各章节的工程文件和工具（chapxx文件夹下），请参考书中的说明使用。本节主要说明RobinPkg中示例的编译方法。

RobinPkg中的代码，笔者是在以下环境下编译通过的：

 :bulb: 操作系统： Windows 10 (版本10.0.19042.1165)<br>
 :bulb: 编译工具： Visual Studio 2015, Python 2.7, IASL(20200528), NASM 2.15<br>
 :bulb: EDK2分支： EDK2 vUDK2018<br>
 :bulb: StdLib库： EADK 1.02<br>

### 1 搭建UEFI开发环境
请按照书中第2章的介绍，搭建UEFI的开发环境。书中为了保持代码的整洁，使用了批处理进行编译。

一般的情况下，可以将StdLib库中的所有文件，复制到EDK2的目录下进行使用。具体可以参考笔者的系列博客-UEFI开发探索。可参考的几篇：

>>[UEFI开发探索97 – EDK2模拟器搭建网络环境](https://blog.csdn.net/luobing4365/article/details/119848459)<br>
>>[UEFI开发探索62-环境搭建4（总结EDK2各种版本）](https://blog.csdn.net/luobing4365/article/details/107182353)<br>
>>[UEFI开发探索22 – 环境搭建3(使用UDK2018搭建)](https://blog.csdn.net/luobing4365/article/details/101018455)<br>
>>[UEFI开发探索03 – 环境搭建2](https://blog.csdn.net/luobing4365/article/details/100188051)<br>
>>[UEFI开发探索02 – 环境搭建1](https://blog.csdn.net/luobing4365/article/details/100187798)<br>

大致的步骤如下：<br>
:low_brightness: 1. 安装必要的开发工具，包括Visual Studio、Python、ASL和NASM编译器，并设置PATH路径；

:low_brightness: 2. 新建工作目录MyWorkspace，将必要的代码库下载到本地，主要是EDK2和StdLib库（也即EADK）：

```
Administrator@robin MINGW64 /c/MyWorkspace
$ git clone --branch vUDK2018 https://github.com/tianocore/edk2.git
$ git clone https://github.com/tianocore/edk2-libc.git
```

:low_brightness: 3. 更新子模块，编译BaseTools

```
Administrator@robin MINGW64 /c/MyWorkspace/edk2
$ git submodule update --init
```

打开VS2015 x86 Native Tools Command Prompt,进行编译：

`C:\MyWorkspace\edk2> edksetup.bat Rebuild`

编译完成后，BaseTools及其他工具会自动生成。

:low_brightness: 4. 将edk2-libc下的三个文件夹AppPkg、StdLib和StdLibPrivateInternalFiles，全部复制到edk2目录下，方便后续的编译。
测试是否搭建成功：

```
C:\MyWorkspace\edk2> edksetup.bat
C:\MyWorkspace\edk2> build -p AppPkg\AppPkg.dsc -a IA32 
```

如无错误提示，编译完成，即表示编译成功。

Github在国内使用，速度会比较慢，可以在Gitee上寻找对应的源进行下载。或者参考笔者另外一篇博客，将Github上的仓库导入到Gitee上，以加快下载速度：

 :zap: [使用gitee下载github项目](https://blog.csdn.net/luobing4365/article/details/105658274)

### 2 编译RobinPkg中的示例

把RobinPkg文件夹，复制到ekd2根目录下，就完成了编译的准备工作了。准备好的ekd2目录如下所示：

![输入图片说明](https://images.gitee.com/uploads/images/2021/0829/142318_b353a02b_791211.png "gitee_edk2_dir.png")

一般的示例，比如pixelCHS，可以直接编译，编译命令如下：

```
C:\MyWorkspace\edk2> edksetup.bat
C:\edk202011\edk2> build -p RobinPkg\RobinPkg.dsc -m RobinPkg\Applications\pixelCHS\pixelCHS.inf -a IA32 -t VS2015x86 -b DEBUG 
```

由于edk2下的Conf\target.txt中准备了缺省的编译参数，“-t”和“-a”在本例中也可以不指定。

与网络相关的示例，包括stdEchoTcp4、stdEchoUdp4等，使用了StdLib库中的网络函数。在编译此的时候，会报C4706的警告。警告是\StdLib\BsdSocketLib\ns_addr.c中的第83行语句引起的，这条语句在表达式内赋值，引发了C4706警告。

可以修改此语句，或者修改编译参数，关闭此警告。比如编译32位、目标为DEBUG的程序，可以修改Conf\tool_def.txt中的DEBUG_VS2015x86_IA32_CC_FLAGS宏定义，将其中的参数从
/W4”改为“/W3”即可。

修改完之后，即可通过编译。

## 其他说明
在编写本书的过程中，笔者尽量做到每句话都言之有据。但是学识有限，书中可能出现错误和不准确的介绍，恳请读者批评指正。如果发现本书的任何错误，或者有任何的建议，请发邮件至：

 :email: [uefi_explorer@163.com](uefi_explorer@163.com)

对于UEFI开发的探索，以及其他技术的探索，是永无止境的。平常的工作学习中，笔者会不定期的更新博客和专栏，有兴趣的读者，可以关注：

 :heart: [CSDN博客 https://blog.csdn.net/luobing4365](https://blog.csdn.net/luobing4365)

 :purple_heart: [知乎专栏 https://www.zhihu.com/column/c_1233025362843209728](https://www.zhihu.com/column/c_1233025362843209728)

 :blue_heart: [个人博客 http://yiiyee.cn/blog/author/luobing/](http://yiiyee.cn/blog/author/luobing/)
