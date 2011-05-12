
#include "AEEAppGen.h"
#include "intro.bid"
#include "intro.h"
#include "intro.brh"

static boolean Intro_HandleEvent(IntroApp *pApp, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Intro_InitAppData(IntroApp *pApp);
static void Intro_Update(void* pData);
static void Intro_ReleaseObj(void ** ppObj);
static void Intro_StartAudio(IntroApp *pApp);
static void Intro_FreeAppData(IntroApp *pApp);

int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
{
    IntroApp* pApp = NULL;
    *ppObj = NULL;
		
    if(!AEEApplet_New( sizeof(IntroApp),
                     ClsId,
                     pIShell,
                     pMod,
                     (IApplet**)ppObj,
                     (AEEHANDLER)Intro_HandleEvent,
                     NULL))
        return(ENOMEMORY);
	
    pApp = (IntroApp*) *ppObj;

    if (!Intro_InitAppData( pApp )) 
    {
        return EFAILED;
    }
    
    return (AEE_SUCCESS);
}

static boolean Intro_InitAppData(IntroApp *pApp)
{
    AEEDeviceInfo* pDeviceInfo;
       
    pDeviceInfo = (AEEDeviceInfo*) MALLOC( sizeof( AEEDeviceInfo ) );
    if( !pDeviceInfo )
        return FALSE;

    ISHELL_GetDeviceInfo( pApp->a.m_pIShell, pDeviceInfo );
    pApp->m_nScreenColorDepth   = pDeviceInfo->nColorDepth;
    pApp->m_nScreenWidth        = pDeviceInfo->cxScreen;
    pApp->m_nScreenHeight       = pDeviceInfo->cyScreen;
    FREE( pDeviceInfo );
    
    if((pApp->m_pLogo = ISHELL_LoadResImage(pApp->a.m_pIShell, INTRO_RES_FILE, IDI_LOGO)) == NULL ||
       (pApp->m_pMsg = ISHELL_LoadResImage(pApp->a.m_pIShell, INTRO_RES_FILE, IDI_MSG)) == NULL ||
       (pApp->m_pFooter = ISHELL_LoadResImage(pApp->a.m_pIShell, INTRO_RES_FILE, IDI_FOOTER)) == NULL) {
        DBGPRINTF("Failed to load images.");
        return FALSE;
    }
    
    SETAEERECT( &pApp->m_rViewport, 0, 0,
        pApp->m_nScreenWidth,
        pApp->m_nScreenHeight );
    
    IDISPLAY_GetDeviceBitmap(pApp->a.m_pIDisplay, &pApp->m_pBitmap);
    if(!pApp->m_pBitmap)
        return FALSE;
    
    pApp->m_nCounter = 0;
                
    Intro_StartAudio(pApp);
    
    IDISPLAY_ClearScreen( pApp->a.m_pIDisplay );
    IDISPLAY_Update( pApp->a.m_pIDisplay );
    
    ISHELL_SetTimer( pApp->a.m_pIShell, REFRESH_RATE, (PFNNOTIFY)Intro_Update, pApp );
    
    return TRUE;
}

static void Sound_Callback(void *pUser, AEESoundPlayerCmd eCBType,
    AEESoundPlayerStatus eStatus, uint32 dwParam)
{
    IntroApp *pApp = (IntroApp *)pUser;
    AEESoundPlayerCmdData *pData = (AEESoundPlayerCmdData *)dwParam;
    
    switch(eStatus)
    {
        case AEE_SOUNDPLAYER_DONE:
            ISOUNDPLAYER_Play(pApp->pISoundPlayer);
            break;
    }
}

static void Intro_StartAudio(IntroApp *pApp)
{
    ISHELL_CreateInstance(pApp->a.m_pIShell,
        AEECLSID_SOUNDPLAYER, (void **)&pApp->pISoundPlayer);
        
    ISOUNDPLAYER_Set(pApp->pISoundPlayer,
        SDT_FILE, "data/intro.mp3");
    
    ISOUNDPLAYER_RegisterNotify(pApp->pISoundPlayer, 
        Sound_Callback, (void *)pApp);
    
    ISOUNDPLAYER_Play(pApp->pISoundPlayer);
}

static void Intro_Update( void* pData )
{
    uint32 time_elapsed = ISHELL_GetUpTimeMS(pApp->a.m_pIShell);
    IntroApp *pApp = (IntroApp *)pData;
    AEEImageInfo ii;
        
    pApp->m_nMsgY = MSG_START_Y + (sine[pApp->m_nCounter++ % 360] >> 1);
    
    if( pApp->m_pBitmap != NULL ) 
    {
        IBITMAP_FillRect( pApp->m_pBitmap, &pApp->m_rViewport,
            IBITMAP_RGBToNative(pApp->m_pBitmap, RGB_BLACK), AEE_RO_COPY );
    }
    
	if(pApp->m_pLogo != NULL)
	{
		IIMAGE_GetInfo(pApp->m_pLogo, &ii);
		IIMAGE_Draw(pApp->m_pLogo, (pApp->m_nScreenWidth - ii.cx)/2, 20);
	}
    
    if(pApp->m_pMsg != NULL)
	{
		IIMAGE_GetInfo(pApp->m_pMsg, &ii);
		IIMAGE_Draw(pApp->m_pMsg, (pApp->m_nScreenWidth - ii.cx)/2, pApp->m_nMsgY);
	}
    
    if(pApp->m_pFooter != NULL)
	{
		IIMAGE_GetInfo(pApp->m_pFooter, &ii);
		IIMAGE_Draw(pApp->m_pFooter, (pApp->m_nScreenWidth - ii.cx)/2, 440);
	}
        
    IDISPLAY_Update( pApp->a.m_pIDisplay );
    
    DBGPRINTF("Intro_Update = %d ms", ISHELL_GetUpTimeMS(pApp->a.m_pIShell) - time_elapsed);
    
    ISHELL_SetTimer( pApp->a.m_pIShell, REFRESH_RATE, (PFNNOTIFY) Intro_Update, pApp );   
}

static void Intro_FreeAppData( IntroApp* pApp )
{
    Intro_ReleaseObj( (void**) &pApp->m_pBitmap );
    Intro_ReleaseObj( (void**) &pApp->m_pLogo );
    Intro_ReleaseObj( (void**) &pApp->m_pMsg );
    Intro_ReleaseObj( (void**) &pApp->m_pFooter );
}

static void Intro_ReleaseObj(void ** ppObj)
{
  if ( ppObj && *ppObj ) 
  {
    (void) IBASE_Release( ( (IBase *) *ppObj ) );
    *ppObj = NULL;
  }
}

static boolean Intro_HandleEvent(IntroApp *pApp, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    switch (eCode) {
        case EVT_APP_START:
            return(TRUE);

        case EVT_APP_STOP:
            Intro_FreeAppData(pApp);
            return(TRUE);

        default:
            break;
   }
   
   return(FALSE);
}
