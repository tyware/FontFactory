#include "stdafx.h"
#include "TableGlyf.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        /*CGlyphInstrs::CGlyphInstrs()
            :m_nContourStart(0), m_nPointStart(0), m_nPointEnd(0), nInstructions(0), pInstructions(0)
        {
        }
        CGlyphInstrs::CGlyphInstrs(UShort nInstrs)
            : m_nContourStart(0), m_nPointStart(0), m_nPointEnd(0), nInstructions(nInstrs)
        {
            pInstructions = new Byte[nInstrs + 1];
        }
        CGlyphInstrs::~CGlyphInstrs()
        {
            if (pInstructions) delete[] pInstructions;
        }*/

        CGlyphDef::CGlyphDef()
            : numberOfContours(0), pRef(0)
            , pInstructions(0), nInstructions(0)
        {
        }
        CGlyphDef::~CGlyphDef()
        {
            if (pInstructions) delete[] pInstructions;

            //for (size_t i = 0; i < instructions.size(); i++)
            //    delete instructions[i];
            //instructions.clear();
        }

        CGlyphOutlines* CGlyphDef::GetOutline()
        {
            if(numberOfContours >= 0) // simple glyph
                return &((CSimpleGlyph*)this)->outlines;
            else
            {
                // compound glyph
                // TODO
                return NULL;
            }
        }

        UShort CGlyphDef::GetInstructions(Byte **pInstr)
        {
            *pInstr = pInstructions;
            return nInstructions;
        }

        //UShort CGlyphDef::GetInstructions(int index, Byte **pInstr)
        //{
        //    if (index < 0 || index >= instructions.size())
        //    {
        //        *pInstr = nullptr;
        //        return 0;
        //    }
        //    *pInstr = instructions[index]->pInstructions;
        //    return instructions[index]->nInstructions;
        //}


        CSimpleGlyph::CSimpleGlyph()
        {
        };

        CSimpleGlyph::~CSimpleGlyph()
        {
            //if(pEndPtsOfContours) delete[] pEndPtsOfContours;
            //if(pCoordinates) delete[] pCoordinates;
        };

        CCompoundGlyph::CCompoundGlyph()
        {
        }
        CCompoundGlyph::~CCompoundGlyph()
        {
            //if(pCompoints) delete[] pCompoints;
            for(size_t i=0; i<components.size(); i++)
                delete components[i];
            components.clear();
        }

        CTableGlyf::CTableGlyf(void)
            :numGlyphs(0)
        {
        }


        CTableGlyf::~CTableGlyf(void)
        {
            size_t numGlyphs = glyphArray.size();
            for(size_t i=0; i<numGlyphs; i++)
                delete glyphArray[i];
            glyphArray.clear();
            glyphMap.clear();
        }

        void CTableGlyf::Load(void* ptr, Long offset, ULong *offsetArray)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            std::map<Long, CGlyphDef*>::iterator it;

#ifdef DEBUG_GLYF
            FILE *fp;
            char buf[128];
            fopen_s(&fp, "G:\\Test\\tmp1\\Glyf.txt", "w");
#endif

            for(Long i = 0; i<numGlyphs; i++)
            {
                if(offsetArray[i] == offsetArray[i+1])
                {
                    CSimpleGlyph *pGlyph = new CSimpleGlyph();
                    glyphArray.push_back(pGlyph);
                    glyphMap.insert(std::pair<Long, CGlyphDef*>(i, pGlyph));

                    //offsetToGlphyMap.insert(std::pair<Long, CGlyphDef*>(offsetArray[i], pGlyph));
                    pGlyph->numberOfContours = 0;
                    pGlyph->outlines.numContours = 0;
                    pGlyph->xMin = 0;
                    pGlyph->yMin = 0;
                    pGlyph->xMax = 0;
                    pGlyph->yMax = 0;
                    continue;
                }

                pFont->Seek(offset + offsetArray[i], SEEK_SET);

                Short numberOfContours = pFont->ReadShort();

#ifdef DEBUG_GLYF
                sprintf_s(buf, 128, "%03d(%02X): %02d, ", i, i, numberOfContours);
                fwrite(buf, 1, strlen(buf), fp);
#endif

                if(numberOfContours >= 0) // simple glyph
                {
#ifdef DEBUG_GLYF
                    fwrite("simp, ", 6, 1, fp);
#endif

                    CSimpleGlyph *pGlyph = new CSimpleGlyph();
                    glyphArray.push_back(pGlyph);
                    glyphMap.insert(std::pair<Long, CGlyphDef*>(i, pGlyph));

                    pGlyph->numberOfContours = numberOfContours;
                    pGlyph->xMin = pFont->ReadFWord();
                    pGlyph->yMin = pFont->ReadFWord();
                    pGlyph->xMax = pFont->ReadFWord();
                    pGlyph->yMax = pFont->ReadFWord();
                    pGlyph->outlines.numContours = pGlyph->numberOfContours;

                    if(pGlyph->numberOfContours == 0) continue;

                    pGlyph->outlines.pEndPtsOfContours = new unsigned int [numberOfContours];
                    for(Short j = 0; j < numberOfContours; j++)
                    {
                        pGlyph->outlines.pEndPtsOfContours[j] = pFont->ReadUShort();
#ifdef DEBUG_GLYF
                        sprintf_s(buf, 128, "%03d ", pGlyph->outlines.pEndPtsOfContours[j]);
                        fwrite(buf, 1, strlen(buf), fp);
#endif
                    }
#ifdef DEBUG_GLYF
                    fwrite("\n", 1, 1, fp);
#endif

                    UShort nInstructions = pFont->ReadUShort();
                    pGlyph->nInstructions = nInstructions;
                    pGlyph->pInstructions = new Byte[pGlyph->nInstructions];
                    pFont->ReadBytes((char*)pGlyph->pInstructions, nInstructions);
                    /*if (nInstructions > 0)
                    {
                        CGlyphInstrs *pGlypyInstrs = new CGlyphInstrs(nInstructions);
                        pFont->ReadBytes((char*)pGlypyInstrs->pInstructions, nInstructions);
                        pGlypyInstrs->m_nContourStart = 0;
                        pGlypyInstrs->m_nPointStart = 0;
                        pGlypyInstrs->m_nPointEnd = pGlyph->outlines.numPoints - 1;
                        pGlyph->instructions.push_back(pGlypyInstrs);
                    }*/

                    pGlyph->outlines.numPoints = pGlyph->outlines.pEndPtsOfContours[numberOfContours-1] + 1;
                    pGlyph->outlines.pCoordinates = new CGlyphCoordinate[pGlyph->outlines.numPoints];

                    Byte flag;
                    Long index = 0;
                    Byte *flags = new Byte[pGlyph->outlines.numPoints];
                    while(index < pGlyph->outlines.numPoints)
                    {
                        flag = pFont->ReadByte();
                        flags[index++] = flag;
                        if((flag & REPEAT_FLAG) != 0)
                        {
                            // repeat flag is set, next byte is number of additional times this set of flags is repeated
                            Byte n = pFont->ReadByte();
                            for(Byte j=0; j < n; j++)
                                flags[index++] = flag;
                        }
                    }

                    // onCurve flag
                    for(Long j = 0; j < pGlyph->outlines.numPoints; j++)
                    {
                        if((flags[j] & ON_CURVE_POINT) != 0)
                            pGlyph->outlines.pCoordinates[j].bOnCurve |=  PF_ONCURVE;
                    }

                    float curValue = 0;
                    // x-coordinates
                    for(Long j = 0; j < pGlyph->outlines.numPoints; j++)
                    {
                        if((flags[j] & X_SHORT_VECTOR) != 0) // 1-byte
                        {
                            pGlyph->outlines.pCoordinates[j].x = pFont->ReadByte();
                            if((flags[j] & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) == 0) // negative
                                pGlyph->outlines.pCoordinates[j].x = -pGlyph->outlines.pCoordinates[j].x;
                        }
                        else // short
                        {
                            if((flags[j] & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) == 0) // short
                            {
                                pGlyph->outlines.pCoordinates[j].x = pFont->ReadShort();
                            }
                            else // save as previous point
                                pGlyph->outlines.pCoordinates[j].x = 0;
                        }

                        pGlyph->outlines.pCoordinates[j].x += curValue;
                        curValue = pGlyph->outlines.pCoordinates[j].x;
                    }

                    // y-coordinates
                    curValue = 0;
                    for(Long j = 0; j < pGlyph->outlines.numPoints; j++)
                    {
                        if((flags[j] & Y_SHORT_VECTOR) != 0) // 1-byte
                        {
                            pGlyph->outlines.pCoordinates[j].y = pFont->ReadByte();
                            if((flags[j] & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) == 0) // negative
                                pGlyph->outlines.pCoordinates[j].y = -pGlyph->outlines.pCoordinates[j].y;
                        }
                        else // short
                        {
                            if((flags[j] & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) == 0) // short
                            {
                                pGlyph->outlines.pCoordinates[j].y = pFont->ReadShort();
                            }
                            else // save as previous point
                                pGlyph->outlines.pCoordinates[j].y = 0;
                        }

                        pGlyph->outlines.pCoordinates[j].y += curValue;
                        curValue = pGlyph->outlines.pCoordinates[j].y;
                    }

                    Long pos = pFont->Tell();
                    delete[] flags;

                }
                else // compound glyph
                {
#ifdef DEBUG_GLYF
                    fwrite("comp, ", 6, 1, fp);
                    fwrite("\n", 1, 1, fp);
#endif

                    CCompoundGlyph *pGlyph = new CCompoundGlyph();
                    glyphArray.push_back(pGlyph);
                    glyphMap.insert(std::pair<Long, CGlyphDef*>(i, pGlyph));
                    pGlyph->numberOfContours = numberOfContours;
                    pGlyph->xMin = pFont->ReadFWord();
                    pGlyph->yMin = pFont->ReadFWord();
                    pGlyph->xMax = pFont->ReadFWord();
                    pGlyph->yMax = pFont->ReadFWord();

                    UShort flags;
                    bool hasInstructions = false;

                    do
                    {
                        CCompoundGlyph::CComponent *pCC = new CCompoundGlyph::CComponent();
                        pCC->a = pCC->d = 1.0f;
                        pCC->b = pCC->c = pCC->e = pCC->f = 0;
                        pCC->isPoints = false;

                        pCC->flags = pFont->ReadUShort();
                        pCC->glyphid = pFont->ReadUShort();
                        if(pCC->flags & ARG_1_AND_2_ARE_WORDS)
                        {
                            pCC->arg1 = pFont->ReadShort();
                            pCC->arg2 = pFont->ReadShort();
                        }
                        else
                        {
                            pCC->arg1 = (char)pFont->ReadByte();
                            pCC->arg2 = (char)pFont->ReadByte();
                        }

                        if(pCC->flags & ARGS_ARE_XY_VALUES)
                        {
                            //pCC->e = pCC->arg1 / 16384.0f;
                            //pCC->f = pCC->arg2 / 16384.0f;
                            //if(pCC->flags & ROUND_XY_TO_GRID)
                            //{
                            //    pCC->e = floor(pCC->e + 0.5);
                            //    pCC->f = floor(pCC->f + 0.5);
                            //}
                        }
                        else
                            pCC->isPoints = true;

                        if(pCC->flags & WE_HAVE_A_SCALE)
                        {
                            pCC->a = pFont->ReadF2Dot14();
                            pCC->c = pCC->a;
                        }
                        else if (pCC->flags & WE_HAVE_AN_X_AND_Y_SCALE)
                        {
                            pCC->a = pFont->ReadF2Dot14();
                            pCC->d = pFont->ReadF2Dot14();
                        }
                        else if(pCC->flags & WE_HAVE_A_TWO_BY_TWO)
                        {
                            pCC->a = pFont->ReadF2Dot14();
                            pCC->b = pFont->ReadF2Dot14();
                            pCC->c = pFont->ReadF2Dot14();
                            pCC->d = pFont->ReadF2Dot14();
                        }

                        pGlyph->components.push_back(pCC);
                        flags = pCC->flags;

                        if(flags & WE_HAVE_INSTRUCTIONS) hasInstructions = true;
                    }
                    while(flags & MORE_COMPONENTS);

                    if(hasInstructions)
                    {
                        UShort nInstructions = pFont->ReadUShort();
                        pGlyph->nInstructions = nInstructions;
                        pGlyph->pInstructions = new Byte[pGlyph->nInstructions];
                        pFont->ReadBytes((char*)pGlyph->pInstructions, nInstructions);
                        //if (nInstructions > 0)
                        //{
                        //    CGlyphInstrs *pGlypyInstrs = new CGlyphInstrs(nInstructions);
                        //    pFont->ReadBytes((char*)pGlypyInstrs->pInstructions, nInstructions);
                        //    pGlypyInstrs->m_nContourStart = 0;
                        //    pGlypyInstrs->m_nPointStart = 0;
                        //    pGlypyInstrs->m_nPointEnd = - 1;
                        //    pGlyph->instructions.push_back(pGlypyInstrs);
                        //}
                    }
                }
            }
#ifdef DEBUG_GLYF
            fclose(fp);
#endif
        }

        CGlyphDef* CTableGlyf::GetGlyphObj(Long index)
        {
            //if(glyphMap[index]->numberOfContours == -1)
            //    return glyphMap[((CCompoundGlyph*)glyphMap[index])->components[1]->glyphid];
            if(index >=0 && index < glyphMap.size()) return glyphMap[index];
            else return NULL;
        }


    };

};

