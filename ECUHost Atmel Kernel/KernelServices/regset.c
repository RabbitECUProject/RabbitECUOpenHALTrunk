/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      REGSET                                                 */
/* DESCRIPTION:        This code is functions for register setup              */
/*                                                                            */
/* FILE NAME:          REGSET.c                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "REGSET.h"


void REGSET_vStart(uint32* u32Stat)
{
	OS_xModuleStartOK(*u32Stat);
}

void REGSET_vRun(uint32* const u32Stat)
{
}

void REGSET_vTerminate(uint32* const u32Stat)
{

}


/************************************************
*
*   Function:     REGSET_vInitReg32
*
*   Inputs:       list - pointer to the null terminated list of 
*                 register:value:writemode triples.
*
*   Outputs:      All registers in list are set to value iaw 
*									write mode.
*
*   Returns:      None.
*
*   Description:  This routine navigates a list of 32 bit
*                 registers and set them to a default value
*                 and returns on NULL register pointer.
*
************************************************/
void REGSET_vInitReg32(const REGSET_tstReg32Val* ast)
{
  while(ast->reg != NULL)
  {
		switch (ast->writeMode)
		{
			case REGSET_enOverwrite:
				*ast->reg = ast->val;	
				break;
				 
			case REGSET_enOr:
				*ast->reg |= ast->val;
				break;
				 
			case REGSET_enAnd:
				*ast->reg &= ast->val;
				break;
				 
			case REGSET_enXor:
				*ast->reg ^= ast->val;
				break;
			
			default:
				break;
		}
    ast++; 
  }
}


/************************************************
*
*   Function:     REGSET_vInitReg16
*
*   Inputs:       list - pointer to the null terminated list of 
*                 register:value:writemode triples.
*
*   Outputs:      All registers in list are set to value iaw 
*									write mode.
*
*   Returns:      None.
*
*   Description:  This routine navigates a list of 16 bit
*                 registers and set them to a default value
*                 and returns on NULL register pointer.
*
************************************************/
void REGSET_vInitReg16(const REGSET_tstReg16Val* ast)
{
  while(ast->reg != NULL)
  {
		switch (ast->writeMode)
		{
			case REGSET_enOverwrite:
				*ast->reg = ast->val;	
				break;
				 
			case REGSET_enOr:
				*ast->reg |= ast->val;
				break;
				 
			case REGSET_enAnd:
				*ast->reg &= ast->val;
				break;
				 
			case REGSET_enXor:
				*ast->reg ^= ast->val;
				break;
			
			default:
				break;
		}
    ast++; 
  }
}

/************************************************
*
*   Function:     REGSET_vInitReg8
*
*   Inputs:       list - pointer to the null terminated list of 
*                 register:value:writemode triples.
*
*   Outputs:      All registers in list are set to value iaw 
*									write mode.
*
*   Returns:      None.
*
*   Description:  This routine navigates a list of 8 bit
*                 registers and set them to a default value
*                 and returns on NULL register pointer.
*
************************************************/
void REGSET_vInitReg8(const REGSET_tstReg8Val* ast)
{
  while(ast->reg != NULL)
  {
		switch (ast->writeMode)
		{
			case REGSET_enOverwrite:
				*ast->reg = ast->val;	
				break;
				 
			case REGSET_enOr:
				*ast->reg |= ast->val;
				break;
				 
			case REGSET_enAnd:
				*ast->reg &= ast->val;
				break;
				 
			case REGSET_enXor:
				*ast->reg ^= ast->val;
				break;
			
			default:
				break;
		}
    ast++; 
  }
}
			
			
