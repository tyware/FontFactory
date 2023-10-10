#pragma once
//#define DEBUG_CMAP
//#define DEBUG_GLYF
//#define DEBUG_OUTLINES

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        typedef unsigned char Byte;
        typedef char Char;
        typedef unsigned short UShort;
        typedef short Short;
        typedef UINT32 ULong;
        typedef INT32 Long;
        typedef INT32 Fixed;
        typedef INT16 FWord;
        typedef UINT16 UFWord;
        typedef INT32 FUInt;
        typedef UINT32 UFUInt;
        typedef INT16 F2Dot14;

        typedef enum
        {
            TAG_none,
            TAG_cmap,
            TAG_glyf,
            TAG_head,
            TAG_hhea,
            TAG_hmtx,
            TAG_loca,
            TAG_maxp,
            TAG_name,
            TAG_post,
            TAG_os2,
            TAG_cvt,
            TAG_ebdt,
            TAG_eblc,
            TAG_ebsc,
            TAG_fpgm,
            TAG_gasp,
            TAG_hdmx,
            TAG_kern,
            TAG_ltsh,
            TAG_prep,
            TAG_pclt,
            TAG_vdmx,
            TAG_vhea,
            TAG_vmtx
        }TABLE_TAG;


    };

};
