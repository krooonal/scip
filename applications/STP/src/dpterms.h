/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2019 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not visit scip.zib.de.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   dpterms.h
 * @brief  Dynamic programming solver for Steiner tree (sub-) problems with small number of terminals
 * @author Daniel Rehfeldt
 *
 * This file implements a dynamic programming method to solve Steiner tree problems to optimality.
 * FPT with respect to the number of terminals. Based on algorithm by Erickson, Monma and Veinott,
 * which itself is a slight extension of Dryefus-Wagner.
 * This implementation uses several reduction methods to improve the practical performance.
 * It also uses a node-separator technique from "Separator-Based Pruned Dynamic Programming for Steiner Tree"
 * by Iwata and Shigemura.
 *
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef APPLICATIONS_STP_SRC_DPTERMS_H_
#define APPLICATIONS_STP_SRC_DPTERMS_H_

#include "scip/scip.h"
#include "graph.h"
#include "stpvector.h"
#include "stpbitset.h"


/** dynamic programming search tree */
typedef struct dynamic_programming_search_tree DPSTREE;


#ifdef __cplusplus
extern "C" {
#endif


extern SCIP_RETCODE     dpterms_solve(SCIP*, GRAPH*, int*);
extern SCIP_Bool        dpterms_isPromising(const GRAPH*);

extern SCIP_RETCODE     dpterms_streeInit(SCIP*, int, int, DPSTREE**);
extern void             dpterms_streeFree(SCIP*, DPSTREE**);
extern SCIP_RETCODE     dpterms_streeInsert(SCIP*, STP_Bitset, STP_Bitset, int64_t, DPSTREE*);
extern STP_Vectype(int) dpterms_streeCollectIntersects(SCIP*, STP_Bitset, STP_Bitset, DPSTREE*);


#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_STP_SRC_DPTERMS_H_ */
