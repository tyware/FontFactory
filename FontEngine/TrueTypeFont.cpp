#include "stdafx.h"
#include "TrueTypeFont.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTrueTypeFont::CTrueTypeFont()
            :tableEntries(0)
        {
        }

        CTrueTypeFont::~CTrueTypeFont()
        {
            if(fp) fclose(fp);
            if(tableEntries) delete[] tableEntries;
        }

        UShort CTrueTypeFont::ToUShort(Byte *buf)
        {
            return buf[1] + (buf[0] << 8);
        }

        Short CTrueTypeFont::ToShort(Byte *buf)
        {
            return buf[1] + (buf[0] << 8);
        }

        ULong CTrueTypeFont::ToULong(Byte *buf)
        {
            return buf[3] + (buf[2] << 8) + (buf[1] << 16) + (buf[0] << 24);
        }

        Long CTrueTypeFont::ToLong(Byte *buf)
        {
            return buf[3] + (buf[2] << 8) + (buf[1] << 16) + (buf[0] << 24);
        }

        float CTrueTypeFont::FixedToFloat(Byte *buf)
        {
            Long v = ToLong(buf);
            Short mantissa = (Short)(v >> 16);
            float res = (v & 0xFFFF) / 65536.0f;
            if((mantissa & 0x8000)) res = mantissa - res; // negative
            else res = mantissa - res; // positive
            return res;
        }

        float CTrueTypeFont::F2DOT14ToFloat(Byte *buf)
        {
            Short v = ToShort(buf);
            float res = ((v >> 14) & 1) + (v & 0x3FFF) / 16384.0f;
            if(v >> 15) res = -res;
            return res;
        }

        Byte CTrueTypeFont::ReadByte()
        {
            char buf[1];
            size_t byts = ReadBytes(buf, 1);
            return (Byte)buf[0];
        }
        Char CTrueTypeFont::ReadChar()
        {
            char buf[1];
            size_t byts = ReadBytes(buf, 1);
            return buf[0];
        }
        UShort CTrueTypeFont::ReadUShort()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToUShort((Byte*)buf);
        }
        Short CTrueTypeFont::ReadShort()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToShort((Byte*)buf);
        }
        ULong CTrueTypeFont::ReadULong()
        {
            char buf[4];
            size_t byts = ReadBytes(buf, 4);
            return ToULong((Byte*)buf);
        }
        Long CTrueTypeFont::ReadLong()
        {
            char buf[4];
            size_t byts = ReadBytes(buf, 4);
            return ToLong((Byte*)buf);
        }
        UShort CTrueTypeFont::ReadFUWord()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToUShort((Byte*)buf);
        }
        Short CTrueTypeFont::ReadFWord()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToShort((Byte*)buf);
        }
        float CTrueTypeFont::ReadFixed()
        {
            char buf[4];
            size_t byts = ReadBytes(buf, 4);
            return FixedToFloat((Byte*)buf);
        }
        float CTrueTypeFont::ReadF2Dot14()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return F2DOT14ToFloat((Byte*)buf);
        }


        BOOL CTrueTypeFont::LoadFontFile(wchar_t *fileName)
        {
            if(!CBaseFont::LoadFontFile(fileName)) return false;

            fseek(fp, 0, SEEK_SET);

            // Table Directory //
            sfntVersion = ReadFixed();
            numTables = ReadUShort();
            searchRange = ReadUShort();
            entrySelector = ReadUShort();
            rangeShift = ReadUShort();

            // Read table entries //
            if(tableEntries) delete[] tableEntries;
            tableEntries = NULL;
            if(numTables > 0)
                tableEntries = new CTableEntry[numTables];
            for(UShort i = 0; i< numTables; i++)
            {
                ReadTables(&tableEntries[i]);
            }

            // Read table //
            for(UShort i = 0; i< numTables; i++)
            {
                switch(tableEntries[i].tag)
                {
                case TAG_cmap:
                    ReadCMap(tableEntries[i].offset, tableEntries[i].length);
                    break;
                case TAG_glyf:
                    break;
                case TAG_head:
                    break;
                case TAG_hhea:
                    break;
                case TAG_hmtx:
                    break;
                case TAG_loca:
                    break;
                case TAG_maxp:
                    break;
                case TAG_name:
                    break;
                case TAG_post:
                    break;
                case TAG_os2:
                    ReadOS2(tableEntries[i].offset);
                    break;
                case TAG_cvt:
                    break;
                case TAG_ebdt:
                    break;
                case TAG_eblc:
                    break;
                case TAG_ebsc:
                    break;
                case TAG_fpgm:
                    break;
                case TAG_gasp:
                    break;
                case TAG_hdmx:
                    break;
                case TAG_kern:
                    break;
                case TAG_ltsh:
                    break;
                case TAG_prep:
                    break;
                case TAG_pclt:
                    break;
                case TAG_vdmx:
                    break;
                case TAG_vhea:
                    break;
                case TAG_vmtx:
                    break;
                default:
                    break;
                }
            }

            return true;
        }


        BOOL CTrueTypeFont::ReadOS2(Long offset)
        {
            fseek(fp, offset, SEEK_SET);
            tableOS2.version = ReadUShort();
            tableOS2.xAvgCharWidth = ReadShort();
            tableOS2.usWeightClass = ReadUShort();
            tableOS2.usWidthClass = ReadUShort();
            tableOS2.fsType = ReadShort();
            tableOS2.ySubscriptXSize = ReadShort();
            tableOS2.ySubscriptYSize = ReadShort();
            tableOS2.ySubscriptXOffset = ReadShort();
            tableOS2.ySubscriptYOffset = ReadShort();
            tableOS2.ySuperscriptXSize = ReadShort();
            tableOS2.ySuperscriptYSize = ReadShort();
            tableOS2.ySuperscriptXOffset = ReadShort();
            tableOS2.ySuperscriptYOffset = ReadShort();
            tableOS2.yStrikeoutSize = ReadShort();
            tableOS2.yStrikeoutPosition = ReadShort();
            tableOS2.sFamilyClass = ReadShort();

            tableOS2.bFamilyType = ReadByte();
            tableOS2.bSerifStyle = ReadByte();
            tableOS2.bWeight = ReadByte();
            tableOS2.bProportion = ReadByte();
            tableOS2.bContrast = ReadByte();
            tableOS2.bStrokeVariation = ReadByte();
            tableOS2.bArmStyle = ReadByte();
            tableOS2.bLetterForm = ReadByte();
            tableOS2.bMidline = ReadByte();
            tableOS2.bXHeight = ReadByte();

            tableOS2.ulUnicodeRange1 = ReadULong();
            tableOS2.ulUnicodeRange2 = ReadULong();
            tableOS2.ulUnicodeRange3 = ReadULong();
            tableOS2.ulUnicodeRange4 = ReadULong();

            ReadBytes(tableOS2.achVendID, 4);

            tableOS2.fsSelection = ReadUShort();
            tableOS2.usFirstCharIndex = ReadUShort();
            tableOS2.usLastCharIndex = ReadUShort();
            tableOS2.sTypoAscender = ReadUShort();
            tableOS2.sTypoDescender = ReadUShort();
            tableOS2.sTypoLineGap = ReadUShort();
            tableOS2.usWinAscent = ReadUShort();
            tableOS2.usWinDescent = ReadUShort();
            tableOS2.ulCodePageRange1 = ReadUShort();
            tableOS2.ulCodePageRange2 = ReadUShort();

            return true;
        }

        BOOL CTrueTypeFont::ReadCMap(Long offset, Long length)
        {
            fseek(fp, offset, SEEK_SET);
            tableCMap.version = ReadUShort();
            tableCMap.numTables = ReadUShort();

            if(tableCMap.numTables > 0) tableCMap.subTables = new CTableCMap::CSubTable[tableCMap.numTables];

            for(UShort i = 0; i< tableCMap.numTables; i++)
            {
                tableCMap.subTables[i].platformID = ReadUShort();
                tableCMap.subTables[i].encodingID = ReadUShort();
                tableCMap.subTables[i].offset = ReadULong();
            }

            return true;
        }

        BOOL CTrueTypeFont::ReadTables(CTableEntry *pTable)
        {
            if(pTable == NULL) return false;
            char tag[5];
            tag[0] = ReadByte();
            tag[1] = ReadByte();
            tag[2] = ReadByte();
            tag[3] = ReadByte();
            tag[4] = 0;
            ReadULong(); // bypass checkSum
            pTable->offset = ReadULong();
            pTable->length = ReadULong();

            if(strcmp(tag, "cmap") == 0)
                pTable->tag = TAG_cmap;
            else if(strcmp(tag, "glyf") == 0)
                pTable->tag = TAG_glyf;
            else if(strcmp(tag, "head") == 0)
                pTable->tag = TAG_head;
            else if(strcmp(tag, "hhea") == 0)
                pTable->tag = TAG_hhea;
            else if(strcmp(tag, "hmtx") == 0)
                pTable->tag = TAG_hmtx;
            else if(strcmp(tag, "loca") == 0)
                pTable->tag = TAG_loca;
            else if(strcmp(tag, "maxp") == 0)
                pTable->tag = TAG_maxp;
            else if(strcmp(tag, "name") == 0)
                pTable->tag = TAG_name;
            else if(strcmp(tag, "post") == 0)
                pTable->tag = TAG_post;
            else if(strcmp(tag, "OS/2") == 0)
                pTable->tag = TAG_os2;
            else if(strcmp(tag, "cvt") == 0)
                pTable->tag = TAG_cvt;
            else if(strcmp(tag, "EBDT") == 0)
                pTable->tag = TAG_ebdt;
            else if(strcmp(tag, "EBLC") == 0)
                pTable->tag = TAG_eblc;
            else if(strcmp(tag, "EBSC") == 0)
                pTable->tag = TAG_ebsc;
            else if(strcmp(tag, "fpgm") == 0)
                pTable->tag = TAG_fpgm;
            else if(strcmp(tag, "gasp") == 0)
                pTable->tag = TAG_gasp;
            else if(strcmp(tag, "hdmx") == 0)
                pTable->tag = TAG_hdmx;
            else if(strcmp(tag, "kern") == 0)
                pTable->tag = TAG_kern;
            else if(strcmp(tag, "LTSH") == 0)
                pTable->tag = TAG_ltsh;
            else if(strcmp(tag, "prep") == 0)
                pTable->tag = TAG_prep;
            else if(strcmp(tag, "PCLT") == 0)
                pTable->tag = TAG_pclt;
            else if(strcmp(tag, "VDMX") == 0)
                pTable->tag = TAG_vdmx;
            else if(strcmp(tag, "vhea") == 0)
                pTable->tag = TAG_vhea;
            else if(strcmp(tag, "vmtx") == 0)
                pTable->tag = TAG_vmtx;

            return true;
        }
    };

};
