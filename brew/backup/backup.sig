
#ifndef ENUMFILES_H
#define ENUMFILES_H

//*****************************************************************************
// INCLUDE FILES
//*****************************************************************************

#include "AEEAppGen.h"
#include "AEEFile.h"
#include "AEEStdLib.h"

//*****************************************************************************
// DEFINITIONS
//*****************************************************************************

typedef struct _EnumFilesApp
{
    AEEApplet               a;
    IFileMgr*               m_pIFileMgr;
    
} EnumFilesApp;


#endif
