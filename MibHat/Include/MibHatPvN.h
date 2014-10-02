#ifndef  MIBHATPVN_H_INCLUDED
#define  MIBHATPVN_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
#include <JIP.h>
#include <6LP.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Dio permament data */
typedef struct
{
  uint32 u32Gold;
  uint32 u32BeenHit;
  uint32 u32ShotsFired;
  char status[HAT_BUF_LEN];
} tsMibHatPvNPerm;

/* Bulb status temporary data */
typedef struct
{
  uint32 u32Gold;
  uint32 u32BeenHit;
  uint32 u32ShotsFired;
  char status[HAT_BUF_LEN];
} tsMibHatPvNTemp;

/* Network control mib */
typedef struct
{
	/* MIB handles */
	thJIP_Mib			hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor		sDesc;

	/* Data pointers */
	tsMibHatPvNPerm		sPerm;
	tsMibHatPvNTemp		sTemp;

	/* Other data */
	bool_t					bSaveRecord;

} tsMibHatPvN;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
/* Unpatched functions in MibPvn.c */
PUBLIC void MibHatPvN_vInit(thJIP_Mib hMibHatPvNInit,
			    tsMibHatPvN *psMibHatPvNInit);
PUBLIC void MibHatPvN_vRegister(void);
PUBLIC void MibHatPvN_vSecond(void);
PUBLIC teJIP_Status MibHatPvN_eSetGold	             (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status MibHatPvN_eSetBeenHit	     (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status MibHatPvN_eSetShotsFired	     (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status MibHatPvN_eSetStatus	     (const char *pcVal, uint8 u8Len, void *pvCbData);
#if defined __cplusplus
}
#endif

#endif  /* MIBHATPVN_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
