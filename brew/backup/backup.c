
/* backup.c
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
 
#include "AEEAppGen.h"
#include "backup.bid"
#include "backup.h"

static boolean EnumFiles_HandleEvent(EnumFilesApp *pApp, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean EnumFiles_InitAppData(EnumFilesApp *pApp);
static boolean EnumFiles_CopyFileInfoToSD(EnumFilesApp *pApp, FileInfo *pInfo);
static boolean EnumFiles_CopyFileToSD(EnumFilesApp *pApp, const char *szFile);

int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
{
    EnumFilesApp* pApp = NULL;
    *ppObj = NULL;
		
    if(!AEEApplet_New( sizeof(EnumFilesApp),             // Size of our private class
                     ClsId,                              // Our class ID
                     pIShell,                            // Shell interface
                     pMod,                               // Module instance
                     (IApplet**)ppObj,                   // Return object
                     (AEEHANDLER)EnumFiles_HandleEvent,  // Our event handler
                     NULL))                              // No special "cleanup" function
        return(ENOMEMORY);
	
    pApp = (EnumFilesApp*) *ppObj;

    if (!EnumFiles_InitAppData( pApp )) 
    {
        return EFAILED;
    }
    
    return (AEE_SUCCESS);
}

#define BUF_SIZE    512

static boolean EnumFiles_CopyFileToSD(EnumFilesApp *pApp, const char *szFile)
{
    IFile *iFileSrc = NULL, *iFileDst = NULL;
    char szDst[MAX_PATH];
    uint32 readBytes = 0, writeBytes = 0, totalRBytes = 0, totalWBytes = 0;
    unsigned char buf[BUF_SIZE];
    unsigned char msg[256];
    boolean ret = TRUE;
    int err = 0;
    
    if(!szFile) {
        DBGPRINTF("[-] szFile = NULL!");
        ret = FALSE;
        goto exit;
    }
    
    MEMSET(szDst, 0, sizeof(szDst));
    MEMSET(msg, 0, sizeof(msg));
    
    STRCPY(szDst, COPY_TO);
    STRCAT(szDst, (const char *)(szFile + 3));
    
    SPRINTF(msg, "---------------------------------\n");
    IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
    
    SPRINTF(msg, "[EnumFiles_CopyFileToSD] Src: %s\n[EnumFiles_CopyFileToSD] Dst: %s\n", szFile, szDst);
    IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
    
    DBGPRINTF("[+] Copying '%s'", szFile);
    DBGPRINTF("[+] szDst: '%s'", szDst);
    
    if(IFILEMGR_Test(pApp->m_pIFileMgr, szDst) == SUCCESS) {
        
        //err = IFILEMGR_GetLastError(pApp->m_pIFileMgr);
        
        DBGPRINTF("[-] File exists. Skipping.");
        
        SPRINTF(msg, "[EnumFiles_CopyFileToSD] File exists. Skipping.\n");
        IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
        
        ret = FALSE;
        goto exit; 
    }
    
    iFileSrc = IFILEMGR_OpenFile(pApp->m_pIFileMgr, szFile, _OFM_READ);
    if(!iFileSrc) {
       
        err = IFILEMGR_GetLastError(pApp->m_pIFileMgr);
        
        DBGPRINTF("[-] Failed to open source file. %X", err);
        
        SPRINTF(msg, "[EnumFiles_CopyFileToSD] Failed to open source file. %X\n", err);      
        IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
        
        ret = FALSE;
        goto exit;   
    }
    
    iFileDst = IFILEMGR_OpenFile(pApp->m_pIFileMgr, szDst, _OFM_CREATE);
    if(!iFileDst) {
        err = IFILEMGR_GetLastError(pApp->m_pIFileMgr);
        DBGPRINTF("[-] Failed to open destiny file. %X", err);
        
        SPRINTF(msg, "[EnumFiles_CopyFileToSD] Failed to open destiny file. %X\n", err);
        IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
        
        ret = FALSE;
        goto exit;
    }
    
    while((readBytes = IFILE_Read(iFileSrc, buf, BUF_SIZE)) > 0) {
        
        if((writeBytes = IFILE_Write(iFileDst, buf, readBytes)) == 0) {
            DBGPRINTF("[-] Failed to write file. Write returned 0.");
            ret = FALSE;
            goto exit;
        }
            
        totalWBytes += writeBytes; 
        totalRBytes += readBytes;
        
        //DBGPRINTF("TR: %d | TW: %d", totalRBytes, totalWBytes);        
    }
    
    if(totalRBytes != totalWBytes) {
        DBGPRINTF("[-] Read and written bytes are not equal!");
        ret = FALSE;
        goto exit;
    }
       
    DBGPRINTF("[+] Done. %d bytes", totalRBytes);
    
    SPRINTF(msg, "[EnumFiles_CopyFileToSD] Done. %d bytes.\n", totalRBytes);
    IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
    
    pApp->m_nFileCount++;

exit:
    
    if(iFileSrc != NULL) IFILE_Release(iFileSrc);
    if(iFileDst != NULL) IFILE_Release(iFileDst);
    
    return ret;    
}

static boolean EnumFiles_CopyFileInfoToSD(EnumFilesApp *pApp, FileInfo *pInfo)
{
    return EnumFiles_CopyFileToSD(pApp, pInfo->szName);
}

static boolean EnumFiles_Iterate(EnumFilesApp *pApp, const char* szInitialPath)
{
    unsigned char szPath[MAX_PATH];
    int dwOutLen = MAX_PATH;
    uint32 dwActualRights = 0;
    FileInfo pInfo;
    Entry *pEntries = NULL;
    uint32 nEntryCount = 0, i;
    unsigned char msg[256];
    
#if (1)
    if(IFILEMGR_ResolvePath(pApp->m_pIFileMgr, szInitialPath, szPath, &dwOutLen) != SUCCESS) {
        DBGPRINTF("[-] Failed to resolve path: %s", szInitialPath);
        return FALSE;
    }
#else    
    STRCPY(szPath, szInitialPath);
#endif
    
    if(IFILEMGR_CheckPathAccess(pApp->m_pIFileMgr, szPath, AEEFP_READ, &dwActualRights) != SUCCESS) {
        DBGPRINTF("[-] Access denied: %s", szPath);
        return FALSE;       
    }
    
    pEntries = MALLOC(sizeof(Entry) * MAX_ITEMS);
    
    if(!pEntries) {
        DBGPRINTF("[-] Failed to alloc entries memory.");
        return FALSE;
    }
    
    DBGPRINTF("[+] Iteration of '%s'", szPath);
    
    SPRINTF(msg, "[EnumFiles_Iterate] Iteration of '%s'\n", szPath);
    IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
    
    // Enumerate directories
    if( IFILEMGR_EnumInit(pApp->m_pIFileMgr, szPath, TRUE) == SUCCESS)
    {   
        while(IFILEMGR_EnumNext(pApp->m_pIFileMgr, &pInfo))
        {
            pEntries[nEntryCount].iType = TYPE_DIR;
            STRCPY(pEntries[nEntryCount].szName, pInfo.szName);
            
            nEntryCount++;
            
            if(nEntryCount >= MAX_ITEMS) {
                DBGPRINTF("[-] nEntryCount reached MAX_ITEMS.");
                return FALSE;
            }
            
            //DBGPRINTF("- %s/", pInfo.szName);
            //if(!EnumFiles_Iterate(pApp, pInfo.szName)) {
            //    DBGPRINTF("Iterate failed for: %s", pInfo.szName);
            //}
        }
    } else {
        DBGPRINTF("[-] Can't enum or no dir. Code: %d", IFILEMGR_GetLastError(pApp->m_pIFileMgr));
    }
    
    // Enumerate files
    if( IFILEMGR_EnumInit(pApp->m_pIFileMgr, szPath, FALSE) == SUCCESS)
    {
        while(IFILEMGR_EnumNext(pApp->m_pIFileMgr, &pInfo))
        {
            pEntries[nEntryCount].iType = TYPE_FILE;
            STRCPY(pEntries[nEntryCount].szName, pInfo.szName);
            
            nEntryCount++;
            
            if(nEntryCount >= MAX_ITEMS) {
                DBGPRINTF("[-] nEntryCount reached MAX_ITEMS.");
                return FALSE;
            }
            //DBGPRINTF("   - %s", pInfo.szName);
            //EnumFiles_CopyFileInfoToSD(pApp, &pInfo);
        }
    } else {
        DBGPRINTF("[-] Can't enum or no file. Code: %d", IFILEMGR_GetLastError(pApp->m_pIFileMgr));
    }
    
    DBGPRINTF("[+] Found %d items on [%s].", nEntryCount, szPath);
    
    SPRINTF(msg, "[EnumFiles_Iterate] Found %d items on [%s].\n", nEntryCount, szPath);
    IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
    
    for(i = 0; i < nEntryCount; i++) {
        if(pEntries[i].iType == TYPE_DIR) {
            EnumFiles_Iterate(pApp, pEntries[i].szName);
        } else {
            EnumFiles_CopyFileToSD(pApp, pEntries[i].szName);
            SPRINTF(msg, "[Entry] %s\n", pEntries[i].szName);
            IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
        }        
    }
    
    DBGPRINTF("[+] Exiting [%s]", szPath);
    
    SPRINTF(msg, "[EnumFiles_Iterate] Exiting [%s].\n", szPath);
    IFILE_Write(pApp->m_pLog, msg, STRLEN(msg));
    
    FREE(pEntries);
    
    return TRUE;
}

static boolean EnumFiles_InitAppData(EnumFilesApp *pApp)
{
    pApp->m_nFileCount = 0;
    
	if( ISHELL_CreateInstance( pApp->a.m_pIShell, AEECLSID_FILEMGR, (void **)(&pApp->m_pIFileMgr) ) != SUCCESS )
	{
        DBGPRINTF("[-] Failed to instantiate IFileMgr.");
		return FALSE;
	}
    
    pApp->m_pLog = IFILEMGR_OpenFile(pApp->m_pIFileMgr, LOG_FILE, _OFM_CREATE);
    if(!pApp->m_pLog) {
        DBGPRINTF("[-] Couldn't create log file.");
        DBGPRINTF("[-] %s", LOG_FILE);
        return FALSE;
    }
    
    // ZEEBOIDDATA
    if(EnumFiles_Iterate(pApp, ZEEBOIDS_PATH)) {
        DBGPRINTF("[+] Done. %d files copied.", pApp->m_nFileCount);
    } else {
        DBGPRINTF("[-] Failed to copy requested files.");
    }    
    
    // MIF
    if(EnumFiles_Iterate(pApp, AEEFS_MIF_DIR)) {
        DBGPRINTF("[+] Done. %d files copied.", pApp->m_nFileCount);
    } else {
        DBGPRINTF("[-] Failed to copy requested files.");
    }
    
    // MOD
    if(EnumFiles_Iterate(pApp, AEEFS_MOD_DIR)) {
        DBGPRINTF("[+] Done. %d files copied.", pApp->m_nFileCount);
    } else {
        DBGPRINTF("[-] Failed to copy requested files.");
    }
    
    // SYS
    if(EnumFiles_Iterate(pApp, AEEFS_SYS_DIR)) {
        DBGPRINTF("[+] Done. %d files copied.", pApp->m_nFileCount);
    } else {
        DBGPRINTF("[-] Failed to copy requested files.");
    }
    
    IFILE_Release(pApp->m_pLog);
    IFILEMGR_Release(pApp->m_pIFileMgr);
    
    return TRUE;
}

static boolean EnumFiles_HandleEvent(EnumFilesApp *pApp, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    AECHAR szText[] = {'D','o','n','e','!', '\0'};
            
    switch (eCode) {
    case EVT_APP_START:
    
         IDISPLAY_DrawText(pApp->a.m_pIDisplay, // Display instance
                           AEE_FONT_BOLD,       // Use BOLD font
                           szText,              // Text - Normally comes from resource
                           -1,                  // -1 = Use full string length
                           0,                   // Ignored - IDF_ALIGN_CENTER
                           0,                   // Ignored - IDF_ALIGN_MIDDLE
                           NULL,                // No clipping
                           IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
         IDISPLAY_Update (pApp->a.m_pIDisplay);
         
        return(TRUE);

    case EVT_APP_STOP:
        return(TRUE);

    default:
        break;
   }
   
   return(FALSE);
}
