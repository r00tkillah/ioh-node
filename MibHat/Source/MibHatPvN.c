/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
#include <AccessFunctions.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application device includes */
#include "MibHat.h"
#include "MibHatPvN.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Debug */
#define MIB_HAT_LIGHTS_DBG TRUE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibHatPvN *psMibHatPvN;  /* MIB data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibHatPvN_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibHatPvN_vInit(thJIP_Mib        hMibHatPvNInit,
			       tsMibHatPvN *psMibHatPvNInit)
{
    /* Debug */
    DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\nMibHatPvN_vInit() {%d}", sizeof(tsMibHatPvN));

    /* Valid data pointer ? */
    if (psMibHatPvNInit != (tsMibHatPvN *) NULL)
    {
        PDM_teStatus   ePdmStatus;

        /* Take copy of pointer to data */
        psMibHatPvN = psMibHatPvNInit;
        /* Take a copy of the MIB handle */
        psMibHatPvN->hMib = hMibHatPvNInit;

	/* default state if no data is read from PDM */
	psMibHatPvN->sPerm.u32Gold = 0;
	psMibHatPvN->sPerm.u32BeenHit = 0;
	psMibHatPvN->sPerm.u32ShotsFired = 0;
	strcpy(psMibHatPvN->sPerm.status, "Default Status");

        /* Load Hat mib data */
        ePdmStatus = PDM_eLoadRecord(&psMibHatPvN->sDesc,
				     (uint16)(MIB_ID_HAT_LIGHTS & 0xFFFF),
                                     (void *) &psMibHatPvN->sPerm,
                                     sizeof(psMibHatPvN->sPerm),
                                     FALSE);
	
	/* Initialise HW from PDM data */
	/* FIXME! Implement */
    }
}

/****************************************************************************
 *
 * NAME: MibHatPvN_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibHatPvN_vRegister(void)
{
    teJIP_Status eStatus;

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibHatPvN->hMib);
    /* Debug */
    DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\nMibHatPvN_vRegister()");
    DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\n\teJIP_RegisterMib(HatPvN)=%d", eStatus);

    /* Make sure permament data is saved */
	psMibHatPvN->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibHatPvN_vSecond
 *
 * DESCRIPTION:
 * Called once per second
 *
 ****************************************************************************/
PUBLIC void MibHatPvN_vSecond(void)
{
    /* Need to save record ? */
    if (psMibHatPvN->bSaveRecord)
    {
        /* Clear flag */
        psMibHatPvN->bSaveRecord = FALSE;
        /* Make sure permament data is saved */
        PDM_vSaveRecord(&psMibHatPvN->sDesc);
        /* Debug */
        DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\nMibHatPvN_vSecond()");
        DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\n\tPDM_vSaveRecord(MibHatPvN)");
    }
}

/****************************************************************************
 *
 * NAME: MibHatPvN_eSetGold
 *
 * DESCRIPTION:
 * Sets Gold variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatPvN_eSetGold(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

    /* Write new value */
    /* FIMXE: actually do something */

    /* Read back final value */
    psMibHatPvN->sPerm.u32Gold = u32Val;
    /* Notify any traps */
    vJIP_NotifyChanged(psMibHatPvN->hMib, VAR_IX_HAT_PVN_GOLD);
    
    /* Make sure permament data is saved */
    psMibHatPvN->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibHatPvN_eSetBeenHit
 *
 * DESCRIPTION:
 * Sets BeenHit variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatPvN_eSetBeenHit(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

    /* Write new value */
    /* FIMXE: actually do something */

    /* Read back final value */
    psMibHatPvN->sPerm.u32BeenHit = u32Val;
    /* Notify any traps */
    vJIP_NotifyChanged(psMibHatPvN->hMib, VAR_IX_HAT_PVN_BEENHIT);
    
    /* Make sure permament data is saved */
    psMibHatPvN->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibHatPvN_eSetShotsFired
 *
 * DESCRIPTION:
 * Sets ShotsFired variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatPvN_eSetShotsFired(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

    /* Write new value */
    /* FIMXE: actually do something */

    /* Read back final value */
    psMibHatPvN->sPerm.u32ShotsFired = u32Val;
    /* Notify any traps */
    vJIP_NotifyChanged(psMibHatPvN->hMib, VAR_IX_HAT_PVN_SHOTSFIRED);
    
    /* Make sure permament data is saved */
    psMibHatPvN->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibHatPvN_eSetStatus
 *
 * DESCRIPTION:
 * Sets Status variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatPvN_eSetStatus(const char *pcVal, uint8 u8Len, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

    /* Write new value */
    /* FIMXE: actually do something */

    strncpy(psMibHatPvN->sPerm.status, pcVal, HAT_BUF_LEN);
    /* Notify any traps */
    vJIP_NotifyChanged(psMibHatPvN->hMib, VAR_IX_HAT_PVN_STATUS);
    
    /* Make sure permament data is saved */
    psMibHatPvN->bSaveRecord = TRUE;

    return eReturn;
}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
