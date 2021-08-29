/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
extern "C" {
#pragma section(".CRTMP$XCA",long,read)
#pragma section(".CRTMP$XCZ",long,read)
#pragma section(".CRTMP$XIA",long,read)
#pragma section(".CRTMP$XIZ",long,read)

#pragma section(".CRTMA$XCA",long,read)
#pragma section(".CRTMA$XCZ",long,read)
#pragma section(".CRTMA$XIA",long,read)
#pragma section(".CRTMA$XIZ",long,read)

#pragma section(".CRTVT$XCA",long,read)
#pragma section(".CRTVT$XCZ",long,read)

#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCAA",long,read)
#pragma section(".CRT$XCB",long,read)
#pragma section(".CRT$XCC",long,read)
#pragma section(".CRT$XCZ",long,read)
#pragma section(".CRT$XDA",long,read)
#pragma section(".CRT$XDC",long,read)
#pragma section(".CRT$XDZ",long,read)
#pragma section(".CRT$XIA",long,read)
#pragma section(".CRT$XIAA",long,read)
#pragma section(".CRT$XIC",long,read)
#pragma section(".CRT$XID",long,read)
#pragma section(".CRT$XIY",long,read)
#pragma section(".CRT$XIZ",long,read)
#pragma section(".CRT$XLA",long,read)
#pragma section(".CRT$XLC",long,read)
#pragma section(".CRT$XLD",long,read)
#pragma section(".CRT$XLZ",long,read)
#pragma section(".CRT$XPA",long,read)
#pragma section(".CRT$XPX",long,read)
#pragma section(".CRT$XPXA",long,read)
#pragma section(".CRT$XPZ",long,read)
#pragma section(".CRT$XTA",long,read)
#pragma section(".CRT$XTB",long,read)
#pragma section(".CRT$XTX",long,read)
#pragma section(".CRT$XTZ",long,read)

#pragma section(".rdata$T",long,read)
#pragma section(".rtc$IAA",long,read)
#pragma section(".rtc$IZZ",long,read)
#pragma section(".rtc$TAA",long,read)
//#pragma section(".rtc$TZZ",long,read) 
}

typedef void (__cdecl *_PVFV)(void);
typedef int  (__cdecl *_PIFV)(void);
typedef void (__cdecl *_PVFI)(int); 
#define _CRTALLOC(x) __declspec(allocate(x))    
#undef NULL
#define NULL 0
_CRTALLOC(".CRT$XIA") _PIFV __xi_a[] = { NULL };
_CRTALLOC(".CRT$XIZ") _PIFV __xi_z[] = { NULL };
_CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { NULL };
_CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { NULL };
_CRTALLOC(".CRT$XPA") _PVFV __xp_a[] = { NULL };
_CRTALLOC(".CRT$XPZ") _PVFV __xp_z[] = { NULL };
_CRTALLOC(".CRT$XTA") _PVFV __xt_a[] = { NULL };
_CRTALLOC(".CRT$XTZ") _PVFV __xt_z[] = { NULL };
#pragma comment(linker, "/merge:.CRT=.rdata")

_PVFV *atexits  = NULL;
int num_atexit = 0;
// int max_atexit =-1;


void static _g_finit()
{
	for(int i =num_atexit-1; i>= 0; i--){
		if ( atexits[i] != NULL )
			(*atexits[i])();		
	}
}
#pragma section(".CRT$XPYZ", long ,read)
_CRTALLOC(".CRT$XPYZ") _PVFV __xp_finitz[] = { _g_finit };


template<typename _PVFV>
void _my_initterm (
        _PVFV * pfbegin,
        _PVFV * pfend
        )
{
       /*
         * walk the table of function pointers from the bottom up, until
         * the end is encountered.  Do not skip the first entry.  The initial
         * value of pfbegin points to the first valid entry.  Do not try to
         * execute what pfend points to.  Only entries before pfend are valid.
         */
        while ( pfbegin < pfend )
        {
            /*
             * if current table entry is non-NULL, call thru it.
             */
            if ( *pfbegin != NULL )
                (**pfbegin)();
            ++pfbegin;
        }
}

void __g_cpp_init()
{
	_my_initterm(__xc_a,__xc_z); 
	_my_initterm(__xi_a,__xi_z); 
}

void __g_cpp_finish()
{
	_my_initterm(__xp_a,__xp_z); 
	_my_initterm(__xt_a,__xt_z); 
} 

