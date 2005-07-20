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
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: debug.h,v 1.6 2005/07/20 16:35:14 bzfpfend Exp $"

/**@file   debug.h
 * @brief  methods for debugging
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_DEBUG_H__
#define __SCIP_DEBUG_H__

/** uncomment this define to activate debugging on given solution */
/*#define DEBUG_SOLUTION "qiu.sol"*/


#include "scip/def.h"
#include "scip/memory.h"
#include "scip/type_retcode.h"
#include "scip/type_lp.h"
#include "scip/type_prob.h"
#include "scip/type_tree.h"


#ifdef DEBUG_SOLUTION

/** checks whether given row is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckRow(
   SET*             set,                /**< global SCIP settings */
   ROW*             row                 /**< row to check for validity */
   );

/** checks whether given global lower bound is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckLbGlobal(
   SET*             set,                /**< global SCIP settings */
   VAR*             var,                /**< problem variable */
   Real             lb                  /**< lower bound */
   );

/** checks whether given global upper bound is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckUbGlobal(
   SET*             set,                /**< global SCIP settings */
   VAR*             var,                /**< problem variable */
   Real             ub                  /**< upper bound */
   );

/** checks whether given local bound implication is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckInference(
   BLKMEM*          blkmem,             /**< block memory */
   SET*             set,                /**< global SCIP settings */
   NODE*            node,               /**< local node where this bound change was applied */
   VAR*             var,                /**< problem variable */
   Real             newbound,           /**< new value for bound */
   BOUNDTYPE        boundtype           /**< type of bound: lower or upper bound */
   );

/** informs solution debugger, that the given node will be freed */
extern
RETCODE SCIPdebugRemoveNode(
   NODE*            node                /**< node that will be freed */
   );

/** checks whether given variable bound is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckVbound(
   SET*             set,                /**< global SCIP settings */
   VAR*             var,                /**< problem variable x in x <= b*z + d  or  x >= b*z + d */
   BOUNDTYPE        vbtype,             /**< type of variable bound (LOWER or UPPER) */
   VAR*             vbvar,              /**< variable z    in x <= b*z + d  or  x >= b*z + d */
   Real             vbcoef,             /**< coefficient b in x <= b*z + d  or  x >= b*z + d */
   Real             vbconstant          /**< constant d    in x <= b*z + d  or  x >= b*z + d */
   );

/** checks whether given implication is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckImplic(
   SET*             set,                /**< global SCIP settings */
   VAR*             var,                /**< problem variable */
   Bool             varfixing,          /**< FALSE if y should be added in implications for x == 0, TRUE for x == 1 */
   VAR*             implvar,            /**< variable y in implication y <= b or y >= b */
   BOUNDTYPE        impltype,           /**< type       of implication y <= b (SCIP_BOUNDTYPE_UPPER) or y >= b (SCIP_BOUNDTYPE_LOWER) */
   Real             implbound           /**< bound b    in implication y <= b or y >= b */
   );

/** checks whether given conflict is valid for the debugging solution */
extern
RETCODE SCIPdebugCheckConflict(
   BLKMEM*          blkmem,             /**< block memory */
   SET*             set,                /**< global SCIP settings */
   NODE*            node,               /**< node where the conflict clause is added */
   VAR**            conflictset,        /**< variables in the conflict set */
   int              nliterals           /**< number of literals in the conflict set */
   );

/** creates the debugging propagator and includes it in SCIP */
extern
RETCODE SCIPdebugIncludeProp(
   SCIP*            scip                /**< SCIP data structure */
   );

#else

#define SCIPdebugCheckRow(set,row) SCIP_OKAY
#define SCIPdebugCheckLbGlobal(set,var,lb) SCIP_OKAY
#define SCIPdebugCheckUbGlobal(set,var,ub) SCIP_OKAY
#define SCIPdebugCheckInference(blkmem,set,node,var,newbound,boundtype) SCIP_OKAY
#define SCIPdebugRemoveNode(node) SCIP_OKAY
#define SCIPdebugCheckVbound(set,var,vbtype,vbvar,vbcoef,vbconstant) SCIP_OKAY
#define SCIPdebugCheckImplic(set,var,varfixing,implvar,impltype,implbound) SCIP_OKAY
#define SCIPdebugCheckConflict(blkmem,set,node,conflictset,nliterals) SCIP_OKAY
#define SCIPdebugIncludeProp(scip) SCIP_OKAY

#endif


#endif
