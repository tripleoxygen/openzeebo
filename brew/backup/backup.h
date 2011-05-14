
/* backup.h
 * Zeebo Backup
 *
 * Copyright (C) 2011 OpenZeebo
 * Copyright (C) 2011 Triple Oxygen
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */ 

#ifndef ENUMFILES_H
#define ENUMFILES_H

#include "AEEAppGen.h"
#include "AEEFile.h"
#include "AEEStdLib.h"

#define VERSION "0.1"

#define MAX_PATH 256
#define MAX_ITEMS 500
#define ZEEBOIDS_PATH "fs:/zeeboiddata"
#define COPY_TO "fs:/card0"
#define LOG_FILE COPY_TO"/log.txt"

enum { TYPE_DIR, TYPE_FILE };

typedef struct _Entry 
{
    uint8 iType;
    char szName[MAX_FILE_NAME + 10];
} Entry;

typedef struct _EnumFilesApp
{
    AEEApplet               a;
    IFileMgr*               m_pIFileMgr;
    uint32                  m_nFileCount;
    IFile*                  m_pLog;
    
} EnumFilesApp;

#endif
