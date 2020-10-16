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

/**@file   extreduce.h
 * @brief  This file implements extended reduction techniques for several Steiner problems.
 * @author Daniel Rehfeldt
 *
 *
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/


#ifndef APPLICATIONS_STP_SRC_EXTREDUCE_H_
#define APPLICATIONS_STP_SRC_EXTREDUCE_H_

#include "scip/scip.h"
#include "graph.h"
#include "reduce.h"
#include "completegraph.h"
#include "portab.h"
#include "extreducedefs.h"

/* extreduce_base.c
 */
extern SCIP_RETCODE    extreduce_deleteArcs(SCIP*, const REDCOST*, const int*, GRAPH*, STP_Bool*, int*);
extern SCIP_RETCODE    extreduce_deleteEdges(SCIP*, REDCOST*, const int*, GRAPH*, STP_Bool*, int*);
extern SCIP_RETCODE    extreduce_pseudoDeleteNodes(SCIP*, const SCIP_Bool*, REDCOST*, GRAPH*, SCIP_Real*, int*);
extern SCIP_RETCODE    extreduce_deleteGeneralStars(SCIP*, REDCOST*, const int*, GRAPH*, STP_Bool*, int*);
extern int             extreduce_getMaxTreeDepth(const GRAPH*);
extern int             extreduce_getMaxStackSize(void);
extern int             extreduce_getMaxStackNcomponents(const GRAPH*);
extern int             extreduce_getMaxStackNedges(const GRAPH*);
extern void            extreduce_edgeRemove(SCIP*, int, GRAPH*, DISTDATA*, EXTPERMA*);
extern SCIP_Bool       extreduce_edgeIsValid(const GRAPH*, const REDCOST*, int);
extern void            extreduce_treeRecompCosts(SCIP*, const GRAPH*, EXTDATA*);

/* extreduce_core.c
 */

extern SCIP_RETCODE    extreduce_checkComponent(SCIP*, const GRAPH*, const REDCOST*, EXTCOMP*, DISTDATA*, EXTPERMA*, SCIP_Bool*);
extern SCIP_RETCODE    extreduce_checkArc(SCIP*, const GRAPH*, const REDCOST*, int, DISTDATA*, EXTPERMA*, SCIP_Bool*);
extern SCIP_RETCODE    extreduce_checkEdge(SCIP*, const GRAPH*, const REDCOST*, int, DISTDATA*, EXTPERMA*, SCIP_Bool*);
extern SCIP_RETCODE    extreduce_checkNode(SCIP*, const GRAPH*, const REDCOST*, int, STAR*, DISTDATA*, EXTPERMA*, SCIP_Bool*);


/* extreduce_util.c
 */
extern void               extreduce_extCompRevert(const GRAPH*, const EXTPERMA*, EXTCOMP*);
extern SCIP_Bool          extreduce_extCompIsPromising(const GRAPH*, const EXTPERMA*, const EXTCOMP*);
extern SCIP_Bool          extreduce_extCompFullIsPromising(const GRAPH*, const EXTPERMA*, const EXTCOMP*);
extern SCIP_RETCODE       extreduce_distDataInit(SCIP*, GRAPH*, int, SCIP_Bool, DISTDATA*);
extern SCIP_Real          extreduce_distDataGetSd(SCIP*, const GRAPH*, int, int, DISTDATA*);
extern SCIP_Real          extreduce_distDataGetSdDouble(SCIP*, const GRAPH*, int, int, DISTDATA*);
extern SCIP_Real          extreduce_distDataGetSdDoubleForbiddenSingle(SCIP*, const GRAPH*, int, int, int, DISTDATA*);
extern SCIP_Real          extreduce_distDataGetSdDoubleForbiddenLast(SCIP*, const GRAPH*, int, int, int, int, DISTDATA*);
extern SCIP_Real          extreduce_distDataGetSdDoubleForbiddenEq(SCIP*, const GRAPH*, SCIP_Real, int, int, int, EXTDATA*);
extern SCIP_Real          extreduce_distDataGetSdDoubleForbidden(SCIP*, const GRAPH*, int, int, EXTDATA*);
extern void               extreduce_distDataFreeMembers(SCIP*, const GRAPH*, DISTDATA*);
extern void               extreduce_distDataDeleteEdge(SCIP*, const GRAPH*, int, DISTDATA*);
extern SCIP_RETCODE       extreduce_mldistsInit(SCIP*, int, int, int, int, SCIP_Bool, MLDISTS**);
extern void               extreduce_mldistsFree(SCIP*, MLDISTS**);
extern SCIP_Bool          extreduce_mldistsIsEmpty(const MLDISTS*);
extern SCIP_Bool          extreduce_mldistsEmptySlotExists(const MLDISTS*);
extern int*               extreduce_mldistsEmptySlotTargetIds(const MLDISTS*);
extern int*               extreduce_mldistsEmptySlotTargetIdsDirty(const MLDISTS*);
extern SCIP_Real*         extreduce_mldistsEmptySlotTargetDists(const MLDISTS*);
extern SCIP_Real*         extreduce_mldistsEmptySlotTargetDistsDirty(const MLDISTS*);
extern int                extreduce_mldistsEmptySlotLevel(const MLDISTS*);
extern void               extreduce_mldistsEmptySlotSetBase(int, MLDISTS*);
extern void               extreduce_mldistsEmptySlotReset(MLDISTS*);
extern void               extreduce_mldistsEmptySlotSetFilled(MLDISTS*);
extern void               extreduce_mldistsLevelAddTop(int, int, MLDISTS*);
extern void               extreduce_mldistsLevelCloseTop(MLDISTS*);
extern void               extreduce_mldistsLevelRemoveTop(MLDISTS*);
extern void               extreduce_mldistsLevelRemoveTopNonClosed(MLDISTS*);
extern int                extreduce_mldistsLevelNTargets(const MLDISTS*, int);
extern int                extreduce_mldistsLevelNTopTargets(const MLDISTS*);
extern int                extreduce_mldistsLevelNSlots(const MLDISTS*, int);
extern int                extreduce_mldistsNlevels(const MLDISTS*);
extern int                extreduce_mldistsTopLevel(const MLDISTS*);
extern const int*         extreduce_mldistsTopLevelBases(const MLDISTS*);
extern int                extreduce_mldistsTopLevelNSlots(const MLDISTS*);
extern SCIP_Bool          extreduce_mldistsLevelContainsBase(const MLDISTS*, int, int);
extern const int*         extreduce_mldistsTargetIds(const MLDISTS*, int, int);
extern const SCIP_Real*   extreduce_mldistsTargetDists(const MLDISTS*, int, int);
extern SCIP_Real          extreduce_mldistsTargetDist(const MLDISTS*, int, int, int);
extern const int*         extreduce_mldistsTopTargetIds(const MLDISTS*, int);
extern const SCIP_Real*   extreduce_mldistsTopTargetDists(const MLDISTS*, int);
extern SCIP_Real          extreduce_mldistsTopTargetDist(const MLDISTS*, int, int);


/* extreduce_extmst.c
 */
extern void       extreduce_mstAddRootLevel(SCIP*, int, EXTDATA*);
extern void       extreduce_mstCompRemove(const GRAPH*, EXTDATA*);
extern void       extreduce_mstLevelRemove(REDDATA*);
extern void       extreduce_mstLevelClose(SCIP*, const GRAPH*, int, EXTDATA*);
extern void       extreduce_mstLevelInit(REDDATA*, EXTDATA*);
extern void       extreduce_mstLevelVerticalAddLeaf(SCIP*, const GRAPH*, int, EXTDATA*, SCIP_Bool*);
extern void       extreduce_mstLevelVerticalAddLeafInitial(SCIP*, const GRAPH*, int, EXTDATA*, SCIP_Bool*);
extern void       extreduce_mstLevelVerticalClose(REDDATA*);
extern void       extreduce_mstLevelVerticalRemove(REDDATA*);
extern void       extreduce_mstLevelHorizontalAdd(SCIP*, const GRAPH*, int, const int*, EXTDATA*);
extern SCIP_Real  extreduce_extGetSd(SCIP*, const GRAPH*, int, int, EXTDATA*);
extern SCIP_Real  extreduce_extGetSdDouble(SCIP*, const GRAPH*, int, int, EXTDATA*);
extern SCIP_Real  extreduce_extGetSdProper(SCIP*, const GRAPH*, int, int, EXTDATA*);
extern SCIP_Real  extreduce_extGetSdProperDouble(SCIP*, const GRAPH*, int, int, EXTDATA*);
extern SCIP_Bool  extreduce_mstRuleOutPeriph(SCIP*, const GRAPH*, EXTDATA*);


/* extreduce_recosts.c
 */
extern void            extreduce_redcostAddEdge(const GRAPH*, int, const REDDATA*, SCIP_Bool*, EXTDATA*);
extern void            extreduce_redcostRemoveEdge(int, const REDDATA*, EXTDATA*);
extern SCIP_Bool       extreduce_redcostReverseTreeRuledOut(const GRAPH*, const EXTDATA*);
extern SCIP_Bool       extreduce_redcostRuleOutPeriph(const GRAPH*, EXTDATA*);


/* extreduce_data.c
 */

void                      extreduce_extCompClean(SCIP*, const GRAPH*, const EXTCOMP*, SCIP_Bool, EXTDATA*);
extern SCIP_RETCODE       extreduce_extPermaInit(SCIP*, const GRAPH*, STP_Bool*, EXTPERMA*);
extern SCIP_Bool          extreduce_extPermaIsClean(const GRAPH*, const EXTPERMA*);
extern void               extreduce_extPermaFreeMembers(SCIP*, EXTPERMA*);
extern void               extreduce_extdataClean(EXTDATA*);
extern SCIP_Bool          extreduce_extdataIsClean(const GRAPH*, const EXTDATA*);
extern void               extreduce_reddataClean(REDDATA*);
extern SCIP_Bool          extreduce_reddataIsClean(const GRAPH*, const REDDATA*);
extern void               extreduce_pcdataClean(PCDATA*);
extern SCIP_Bool          extreduce_pcdataIsClean(const GRAPH*, const PCDATA*);


/* extreduce_dbg.c
 */
extern int             extreduce_extStackCompNOutedges(const EXTDATA*, int);
extern SCIP_Bool       extreduce_stackTopIsHashed(const GRAPH*, const EXTDATA*);
extern void            extreduce_extdataCleanArraysDbg(const GRAPH*, EXTDATA*);
extern SCIP_Bool       extreduce_treeIsFlawed(SCIP*, const GRAPH*, const EXTDATA*);
extern SCIP_Bool       extreduce_treeIsHashed(const GRAPH*, const EXTDATA*);
extern SCIP_Bool       extreduce_nodeIsInStackTop(const GRAPH*, const EXTDATA*, int);
extern SCIP_Bool       extreduce_distCloseNodesAreValid(SCIP*, const GRAPH*, const DISTDATA*);
extern SCIP_Real       extreduce_distComputeRestrictedDist(SCIP*, const GRAPH*, int, const DISTDATA*, int, int);
extern void            extreduce_printStack(const GRAPH*, const EXTDATA*);
extern void            extreduce_printLeaves(const EXTDATA*);
extern void            extreduce_printTopLevel(const EXTDATA*);
extern void            extreduce_extendInitDebug(int*, int*);
extern SCIP_Bool       extreduce_sdsverticalInSync(SCIP*, const GRAPH*, int, int, int, EXTDATA*);
extern SCIP_Bool       extreduce_sdshorizontalInSync(SCIP*, const GRAPH*, int, EXTDATA*);
extern SCIP_Bool       extreduce_sdsTopInSync(SCIP*, const GRAPH*, const SCIP_Real[], int, EXTDATA*);
extern SCIP_Bool       extreduce_mstTopCompInSync(SCIP*, const GRAPH*, EXTDATA*);
extern SCIP_Bool       extreduce_mstTopCompExtObjValid(SCIP*, const GRAPH*, int, SCIP_Real, EXTDATA*);
extern SCIP_Bool       extreduce_mstTopCompObjValid(SCIP*, const GRAPH*, SCIP_Real, EXTDATA*);
extern SCIP_Bool       extreduce_mstInternalsInSync(const EXTDATA*);
extern SCIP_Bool       extreduce_mstTopLevelBaseObjValid(SCIP*, const GRAPH*, int, EXTDATA*);


#endif /* APPLICATIONS_STP_SRC_EXTREDUCE_H_ */
