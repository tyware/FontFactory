#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        typedef enum 
        {
            PID_Unicode = 0,
            PID_Macintosh,
            PID_Reserved,
            PID_Microsoft,
            PID_Custom,
        }PLATFORM_ID;

        typedef enum 
        {
            PUEID_10 = 0,
            PUEID_11,
            PUEID_ISO_10646,
            PUEID_20_BMP, // cmap subtable format 0, 4, 6
            PUEID_30_FULL, // cmap subtable format 0, 4, 6, 10, 12
            PUEID_UVS, // cmap subtable format 14
            PUEID_FULL, // cmap subtable format 0, 4, 6, 10, 12, 13
        }PLATFORM_UNICODE_ENCORDING_ID;

        typedef enum 
        {
            PWEID_Symbol = 0,
            PWEID_Unicode_BMP,
            PWEID_Unicode_ShiftJIS,
            PWEID_Unicode_PRC,
            PWEID_Unicode_Big5,
            PWEID_Unicode_Wansung,
            PWEID_Unicode_Johab = 6,
            PWEID_Unicode_Unicode_UCS4 = 10,
        }PLATFORM_WINDOWS_ENCODING_ID;

        typedef enum 
        {
            PISOEID_ASCII = 0,
            PISOEID_ISO_10646,
            PISOEID_ISO_8859_1,
        }PLATFORM_ISO_ENCODING_ID;

        typedef enum 
        {
            PMACEID_Roman = 0,
            PMACEID_Japanese,
            PMACEID_Traditional_Chinese,
            PMACEID_Korean,
            PMACEID_Arabic,
            PMACEID_Hebrew,
            PMACEID_Greek,
            PMACEID_Russian,
            PMACEID_RSymbol,
            PMACEID_Devanagari,
            PMACEID_Gurmukhi,
            PMACEID_Gujarati,
            PMACEID_Oriya,
            PMACEID_Bengali,
            PMACEID_Tamil,
            PMACEID_Telugu,
            PMACEID_Kannada,
            PMACEID_Malayalam,
            PMACEID_Sinhalese,
            PMACEID_Burmese,
            PMACEID_Khmer,
            PMACEID_Thai,
            PMACEID_Laotian,
            PMACEID_Georgian,
            PMACEID_Armenian,
            PMACEID_Simplified_Chinese,
            PMACEID_Tibetan,
            PMACEID_Mongolian,
            PMACEID_Geez,
            PMACEID_Slavic,
            PMACEID_Vietnamese,
            PMACEID_Sindhi,
            PMACEID_Reserved,
        }PLATFORM_MAC_ENCODING_ID;

        typedef enum
        {
            NID_Copyright = 0,
            NID_FontFamily,
            NID_FontSubFamily,
            NID_UniqueFontID,
            NID_FullFontName,
            NID_Version,
            NID_PSFontName,
            NID_Trademark,
            NID_ManufacturerName,
            NID_Designer,
            NID_Description,
            NID_URLVendor,
            NID_URLDesigner,
            NID_LicenseDescription,
            NID_LicenseInfoURL,
            NID_Reserved1,
            NID_TypographicFamilyName,
            NID_TypographicSubFamilyName,
            NID_CompatibleFull,
            NID_SampleText,
            NID_PS_CIDFindFontName,
            NID_WWSFamilyName,
            NID_WWSSubFamilyName,
            NID_LightBkPalette,
            NID_DarkBkPaletter,
            NID_VariationPSNamePrefix,
        }NAME_ID;

        class CTableName
        {
        public:
            class CNameRecord
            {
            public:
                CNameRecord():pString(0){};
                ~CNameRecord()
                {
                    if(pString) delete[] pString;
                };

            public:
                UShort platformID;
                UShort platformSpecificID; // encoding id
                UShort languageID;
                UShort nameID;
                UShort length;
                UShort offset;
                char *pString;
            };

        public:
            CTableName(void);
            ~CTableName(void);
            void Load(void* ptr, Long offset);

        public:
            UShort format;
            UShort count;
            UShort stringOffset;
            CNameRecord *pNameRecord;
            //Char *pNameString;
        };

    };

};

