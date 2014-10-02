/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* Stack includes */
#include <JIP.h>
#include <AccessFunctions.h>
/* Application common includes */
#include "MibHat.h"
#include "MibHatPvN.h"

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibHatPvN sMibHatPvN;

/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_HAT_PVN

#define DECLARE_MIB
#include "MibHatPvNDef.h"
JIP_START_DECLARE_MIB(HatPvNDef, HatPvN)
JIP_CALLBACK(Gold, MibHatPvN_eSetGold, vGetUint32, &sMibHatPvN.sPerm.u32Gold)
JIP_CALLBACK(BeenHit, MibHatPvN_eSetBeenHit, vGetUint32, &sMibHatPvN.sPerm.u32BeenHit)
JIP_CALLBACK(ShotsFired, MibHatPvN_eSetShotsFired, vGetUint32, &sMibHatPvN.sPerm.u32ShotsFired)
JIP_CALLBACK(Status, MibHatPvN_eSetStatus, vGetString, &sMibHatPvN.sPerm.status)
JIP_END_DECLARE_MIB(HatPvN, hHatPvN)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibHatPvN = &sHatPvNMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibHatPvN = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
