//display 256 color bmp pictures

#ifndef _PICTURES_H
#define _PICTURES_H
#include "Common.h"

#define MAX_HUFFMAN_CODE_LEN  16
#pragma pack(1)
typedef struct pcx_header_type
{
	UINT8 Manufacturer;
	UINT8 version;
	UINT8 Encoding;
	UINT8 BitsPerPixel;
	UINT16 xMin,yMin;
	UINT16 xMax,yMax;
	UINT16 Hres;
	UINT16 Vres;
	UINT8 ColorMap[48];
	UINT8 reserved;
	UINT8 ColorPlanes;
	UINT16 BytesPerLine;    //每行的字节数，必定为偶数
	UINT16 PaletteType;
	UINT8 padding[58];
} PCX_HEADER, *PCX_HEADER_PTR;




/* 类型定义 */
/* 编码表项类型定义 */
typedef struct
{
    int symbol; /* 符号 */
    int freq;   /* 频率 */
    int group;  /* 分组 */
    int depth;  /* 码长 */
    int code;   /* 码字 */
} HUFCODEITEM;

/* 编码器类型定义 */
typedef struct
{
    UINT8         huftab[MAX_HUFFMAN_CODE_LEN + 256]; /* 哈夫曼表 */
    int          first [MAX_HUFFMAN_CODE_LEN];       /* first    */
    int          index [MAX_HUFFMAN_CODE_LEN];       /* index    */
    HUFCODEITEM  codelist[256];/* 编码表 */
    void        *input;        /* input bit stream  */
    void        *output;       /* output bit stream */
} HUFCODEC;

// 内部类型定义
typedef struct {
    // width & height
    int       width;
    int       height;

    // quantization table
    int      *pqtab[16];

    // huffman codec ac
    HUFCODEC *phcac[16];

    // huffman codec dc
    HUFCODEC *phcdc[16];

    // components
    int comp_num;
    struct {
        int id;
        int samp_factor_v;
        int samp_factor_h;
        int qtab_idx;
        int htab_idx_ac;
        int htab_idx_dc;
    } comp_info[4];

    int   datalen;
    UINT8 *databuf;
} JFIF;

/* 内部类型定义 */
typedef struct {
    int   type;
    UINT32 bitbuf;
    int   bitnum;
    FILE *fp;
} FBITSTR;

typedef struct {
    int   type;
    UINT32 bitbuf;
    int   bitnum;
    UINT8 *membuf;
    int   memlen;
    int   curpos;
} MBITSTR;

/* BMP 对象的类型定义 */
typedef struct
{
    int   width;   /* 宽度 */
    int   height;  /* 高度 */
    int   stride;  /* 行字节数 */
    void *pdata;   /* 指向数据 */
} BMP_FORJPG;
typedef struct { 
    UINT16  bfType;
    UINT32  bfSize;
    UINT16  bfReserved1;
    UINT16  bfReserved2;
    UINT32  bfOffBits;
    UINT32  biSize;
    UINT32  biWidth;
    UINT32  biHeight;
    UINT16  biPlanes;
    UINT16  biBitCount;
    UINT32  biCompression;
    UINT32  biSizeImage;
    UINT32  biXPelsPerMeter;
    UINT32  biYPelsPerMeter;
    UINT32  biClrUsed;
    UINT32  biClrImportant;
} BMPFILEHEADER;
#pragma pack()

#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
VOID putBMP256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID putBMP256_fast(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID putBMP256_fun(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *PixelHeight,
     OUT UINTN     *PixelWidth
  );

EFI_STATUS decompressPCX24bits(IN EFI_FILE_PROTOCOL *file, OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer);
EFI_STATUS decompressPCX24bits_fast(IN EFI_FILE_PROTOCOL *file, OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer);
VOID putPCX256(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID putPCX256_fast(UINTN x,UINTN y,UINTN Width,UINTN Height,
              EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              UINT8 *Picture,
              UINT8 MaskColor);
VOID decompressPCX256(IN UINTN Width,IN UINTN Height,
              IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              IN UINT8 *Picture,
              IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer);
VOID decompressPCX256_special(IN UINTN Width,IN UINTN Height,
              IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorTable,
              IN UINT8 *Picture,
              IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
              IN UINT8 MaskColor);
              
EFI_STATUS ShowBMP256(CHAR16 *fileName,UINTN x,UINTN y);
EFI_STATUS ShowBMP24True(CHAR16 *fileName,UINTN x,UINTN y);
EFI_STATUS ShowPCX24True(CHAR16 *fileName,UINTN x,UINTN y);
EFI_STATUS ShowJPEG24True(char *fileName,UINTN x_,UINTN y_);

//============================ 移植自ffjipeg ===================
void *jfif_load(char *file);
void jfif_free(void *ctxt);
void init_dct_module(void);
void init_fdct_ftab(int *ftab, int *qtab);
static int ALIGN(int x, int y);
void *bitstr_open(int type, char *file, char *mode);
int bitstr_close(void *stream);
void *bitstr_open(int type, char *file, char *mode);
static void *mbitstr_open(void *buf, int len);
static void *fbitstr_open(char *file, char *mode);
int bitstr_close(void *stream);
static int mbitstr_close(void *stream);
static int fbitstr_close(void *stream);
int bitstr_get_bits(void *stream, int n);
int bitstr_getb(void *stream);
int bitstr_getc(void *stream);
static int mbitstr_getc(void *stream);
static int fbitstr_getc(void *stream);
void huffman_decode_init(HUFCODEC *phc);
int huffman_decode_step(HUFCODEC *phc);
static int category_decode(int code, int size);
void zigzag_decode(int *data);
void init_idct_ftab(int *ftab, int *qtab);
void idct2d8x8(int *data, int *ftab);
int bmp_create(BMP_FORJPG *pb, int w, int h);
void bmp_free(BMP_FORJPG *pb);
void yuv_to_rgb(int y, int u, int v, UINT8 *r, UINT8 *g, UINT8 *b);
int jfif_decode(void *ctxt, BMP_FORJPG *pb);
int bmp_save(BMP_FORJPG *pb, char *file);
#endif