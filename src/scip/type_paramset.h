/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2005 Tobias Achterberg                              */
/*                                                                           */
/*                  2002-2005 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic License.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: type_paramset.h,v 1.4 2005/01/21 09:17:11 bzfpfend Exp $"

/**@file   type_paramset.h
 * @brief  type definitions for handling parameter settings
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __TYPE_PARAMSET_H__
#define __TYPE_PARAMSET_H__


/** possible parameter types */
enum ParamType
{
   SCIP_PARAMTYPE_BOOL    = 0,           /**< bool values: TRUE or FALSE */
   SCIP_PARAMTYPE_INT     = 1,           /**< integer values */
   SCIP_PARAMTYPE_LONGINT = 2,           /**< long integer values */
   SCIP_PARAMTYPE_REAL    = 3,           /**< real values */
   SCIP_PARAMTYPE_CHAR    = 4,           /**< characters */
   SCIP_PARAMTYPE_STRING  = 5            /**< strings: arrays of characters */
};
typedef enum ParamType PARAMTYPE;

typedef struct Param PARAM;             /**< single parameter */
typedef struct ParamData PARAMDATA;     /**< locally defined parameter specific data */
typedef struct ParamSet PARAMSET;       /**< set of parameters */


/** information method for changes in the parameter
 *
 *  Method is called if the parameter was changed through a SCIPparamsetSetXxx() call
 *  (which is called by SCIPsetXxxParam()).
 *  It will not be called, if the parameter was changed directly by changing the value
 *  in the memory location.
 *
 *  input:
 *    scip            : SCIP main data structure
 *    param           : the changed parameter (already set to its new value)
 */
#define DECL_PARAMCHGD(x) RETCODE x (SCIP* scip, PARAM* param)



#include "def.h"
#include "type_retcode.h"
#include "type_scip.h"


#endif
