/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*  Copyright (c) 2002-2023 Zuse Institute Berlin (ZIB)                      */
/*                                                                           */
/*  Licensed under the Apache License, Version 2.0 (the "License");          */
/*  you may not use this file except in compliance with the License.         */
/*  You may obtain a copy of the License at                                  */
/*                                                                           */
/*      http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                           */
/*  Unless required by applicable law or agreed to in writing, software      */
/*  distributed under the License is distributed on an "AS IS" BASIS,        */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*  See the License for the specific language governing permissions and      */
/*  limitations under the License.                                           */
/*                                                                           */
/*  You should have received a copy of the Apache-2.0 license                */
/*  along with SCIP; see the file LICENSE. If not visit scipopt.org.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   branch_dpscost.h
 * @ingroup BRANCHINGRULES
 * @brief  discounted pseudo costs branching rule
 * @author Tobias Achterberg
 * @author Krunal Patel
 *
 * The pseudo costs branching rule selects the branching variable with respect to the so-called pseudo costs
 * of the variables. Pseudo costs measure the average gain per unit in the objective function when the variable
 * was branched on upwards or downwards, resp. The required information is updated at every node of
 * the solving process.
 * 
 * Discounted pseudo cost branching rule is an extension of the pseudo cost branching rule. We consider pseudo 
 * costs as the immediate reward for branching on a variable. Discounted pseudo cost is a discounted total reward
 * for branching on a variable (i.e. gains of nodes a few level below branched node is also considered). 
 *
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_BRANCH_DPSCOST_H__
#define __SCIP_BRANCH_DPSCOST_H__


#include "scip/def.h"
#include "scip/type_retcode.h"
#include "scip/type_scip.h"
#include "scip/type_var.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the discounted pseudo cost branching rule and includes it in SCIP
 *
 *   @ingroup BranchingRuleIncludes
 */
SCIP_EXPORT
SCIP_RETCODE SCIPincludeBranchruleDPscost(
   SCIP*                 scip                /**< SCIP data structure */
   );

/**@addtogroup BRANCHINGRULES
 *
 * @{
 */

/** selects a branching variable, due to discounted pseudo cost, from the given candidate array and returns this variable together
 *  with a branching point */
SCIP_EXPORT
SCIP_RETCODE SCIPselectBranchVarDPscost(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_VAR**            branchcands,        /**< branching candidates */
   SCIP_Real*            branchcandssol,     /**< solution value for the branching candidates */
   SCIP_Real*            branchcandsscore,   /**< array of candidate scores */
   int                   nbranchcands,       /**< number of branching candidates */
   SCIP_VAR**            var,                /**< pointer to store the variable to branch on, or NULL if none */
   SCIP_Real*            brpoint             /**< pointer to store the branching point for the branching variable, will be fractional for a discrete variable */
   );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
