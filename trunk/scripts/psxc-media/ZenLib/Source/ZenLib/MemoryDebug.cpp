// ZenLib::MemoryDebug - To debug memory leaks
// Copyright (C) 2002-2006 Jerome Martinez, Zen@MediaArea.net
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#if defined(ZENLIB_DEBUG)
//---------------------------------------------------------------------------
#include "ZenLib/MemoryDebug.h"
#include <iomanip>
#include <sstream>
using namespace std;
//---------------------------------------------------------------------------

namespace ZenLib
{

//***************************************************************************
// Constructors/destructor
//***************************************************************************

MemoryDebug::MemoryDebug()
{
}

MemoryDebug::~MemoryDebug()
{
    if (!m_Blocks.empty())
        ReportLeaks();
}

//***************************************************************************
// Instance
//***************************************************************************

MemoryDebug& MemoryDebug::Instance()
{
    static MemoryDebug Inst;
    return Inst;
}

//***************************************************************************
// Reports
//***************************************************************************

void MemoryDebug::ReportLeaks()
{
    std::ofstream      m_File ("Debug_MemoryLeak.txt");        // Fichier de sortie

    // Détail des fuites
    std::size_t TotalSize = 0;
    for (TBlockMap::iterator i = m_Blocks.begin(); i != m_Blocks.end(); ++i)
    {
        // Ajout de la taille du bloc au cumul
        TotalSize += i->second.Size;

        // Inscription dans le fichier des informations sur le bloc courant
        m_File << "-> 0x" << std::hex << i->first << std::dec
               << " | "   << std::setw(7) << std::setfill(' ') << static_cast<int>(i->second.Size) << " bytes"
               << " | "   << i->second.File.c_str() << " (" << i->second.Line << ")" << std::endl;
    }

    // Affichage du cumul des fuites
    m_File << std::endl << std::endl << "-- "
           << static_cast<int>(m_Blocks.size()) << " non-released blocs, "
           << static_cast<int>(TotalSize)       << " bytes --"
           << std::endl;
}

//***************************************************************************
// Memory management
//***************************************************************************

void* MemoryDebug::Allocate(std::size_t Size, const char* File, int Line, bool Array)
{
    // Allocation de la mémoire
    void* Ptr = malloc(Size);

    // Ajout du bloc à la liste des blocs alloués
    TBlock NewBlock;
    NewBlock.Size  = Size;
    NewBlock.File  = File;
    NewBlock.Line  = Line;
    NewBlock.Array = Array;
    m_Blocks[Ptr]  = NewBlock;
    return Ptr;
}

void MemoryDebug::Free(void* Ptr, bool Array)
{
    // Recherche de l'adresse dans les blocs alloués
    TBlockMap::iterator It = m_Blocks.find(Ptr);

    // Si le bloc n'a pas été alloué, on génère une erreur
    if (It == m_Blocks.end())
    {
        // En fait ça arrive souvent, du fait que le delete surchargé est pris en compte même là où on n'inclue pas DebugNew.h,
        // mais pas la macro pour le new
        // Dans ce cas on détruit le bloc et on quitte immédiatement
        free(Ptr);
        return;
    }

    // Si le type d'allocation ne correspond pas, on génère une erreur
    if (It->second.Array != Array)
    {
        //throw CBadDelete(Ptr, It->second.File.c_str(), It->second.Line, !Array);
    }

    // Finalement, si tout va bien, on supprime le bloc et on loggiz tout ça
    m_Blocks.erase(It);
    m_DeleteStack.pop();

    // Libération de la mémoire
    free(Ptr);
}

void MemoryDebug::NextDelete(const char* File, int Line)
{
    TBlock Delete;
    Delete.File = File;
    Delete.Line = Line;

    m_DeleteStack.push(Delete);
}

} //NameSpace

#endif // defined(ZENLIB_DEBUG)
