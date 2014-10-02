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
#include "MibHatLights.h"

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibHatLights sMibHatLights;

/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_HAT_LIGHTS

#define DECLARE_MIB
#include "MibHatLightsDef.h"
JIP_START_DECLARE_MIB(HatLightsDef, HatLights)
JIP_CALLBACK(Light0, MibHatLights_eSetLight0, vGetUint32, &sMibHatLights.sPerm.u32Light0)
JIP_CALLBACK(Light1, MibHatLights_eSetLight1, vGetUint32, &sMibHatLights.sPerm.u32Light1)
JIP_CALLBACK(Light2, MibHatLights_eSetLight2, vGetUint32, &sMibHatLights.sPerm.u32Light2)
JIP_CALLBACK(Light3, MibHatLights_eSetLight3, vGetUint32, &sMibHatLights.sPerm.u32Light3)
JIP_END_DECLARE_MIB(HatLights, hHatLights)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibHatLights = &sHatLightsMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibHatLights = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
