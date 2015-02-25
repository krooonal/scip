/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2015 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   struct_heur.h
 * @brief  datastructures for primal heuristics
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_STRUCT_HEUR_H__
#define __SCIP_STRUCT_HEUR_H__


#include "scip/def.h"
#include "scip/type_clock.h"
#include "scip/type_heur.h"

#ifdef __cplusplus
extern "C" {
#endif

/** common settings for diving heuristics */
struct SCIP_Diveset
{
   SCIP_HEUR*            heur;               /**< the heuristic to which this dive set belongs */
   SCIP_Real             minreldepth;        /**< minimal relative depth to start diving */
   SCIP_Real             maxreldepth;        /**< maximal relative depth to start diving */
   SCIP_Real             maxlpiterquot;      /**< maximal fraction of diving LP iterations compared to node LP iterations */
   SCIP_Real             maxdiveubquot;      /**< maximal quotient (curlowerbound - lowerbound)/(cutoffbound - lowerbound)
                                              *   where diving is performed (0.0: no limit) */
   SCIP_Real             maxdiveavgquot;     /**< maximal quotient (curlowerbound - lowerbound)/(avglowerbound - lowerbound)
                                              *   where diving is performed (0.0: no limit) */
   SCIP_Real             maxdiveubquotnosol; /**< maximal UBQUOT when no solution was found yet (0.0: no limit) */
   SCIP_Real             maxdiveavgquotnosol;/**< maximal AVGQUOT when no solution was found yet (0.0: no limit) */
   SCIP_Real             targetdepthfrac;    /**< fraction of lpcands to be reached before next LP solve */
   SCIP_Longint          nlpiterations;      /**< LP iterations used in this heuristic */
   SCIP_Longint          nlps;               /**< the number of LPs solved by this heuristic */
   SCIP_Longint          totaldepth;         /**< the total depth used in this heuristic */
   int                   maxlpiterofs;       /**< additional number of allowed LP iterations */
   SCIP_Bool             backtrack;          /**< use one level of backtracking if infeasibility is encountered? */
   SCIP_DECL_DIVESETGETSCORE((*divesetgetscore));  /**< method for candidate score and rounding direction */
};

/** primal heuristics data */
struct SCIP_Heur
{
   SCIP_Longint          ncalls;             /**< number of times, this heuristic was called */
   SCIP_Longint          nsolsfound;         /**< number of feasible primal solutions found so far by this heuristic */
   SCIP_Longint          nbestsolsfound;     /**< number of new best primal CIP solutions found so far by this heuristic */
   char*                 name;               /**< name of primal heuristic */
   char*                 desc;               /**< description of primal heuristic */
   SCIP_DECL_HEURCOPY    ((*heurcopy));      /**< copy method of primal heuristic or NULL if you don't want to copy your plugin into sub-SCIPs */
   SCIP_DECL_HEURFREE    ((*heurfree));      /**< destructor of primal heuristic */
   SCIP_DECL_HEURINIT    ((*heurinit));      /**< initialize primal heuristic */
   SCIP_DECL_HEUREXIT    ((*heurexit));      /**< deinitialize primal heuristic */
   SCIP_DECL_HEURINITSOL ((*heurinitsol));   /**< solving process initialization method of primal heuristic */
   SCIP_DECL_HEUREXITSOL ((*heurexitsol));   /**< solving process deinitialization method of primal heuristic */
   SCIP_DECL_HEUREXEC    ((*heurexec));      /**< execution method of primal heuristic */
   SCIP_HEURDATA*        heurdata;           /**< primal heuristics local data */
   SCIP_CLOCK*           setuptime;          /**< time spend for setting up this heuristic for the next stages */
   SCIP_CLOCK*           heurclock;          /**< heuristic execution time */
   int                   priority;           /**< priority of the primal heuristic */
   int                   freq;               /**< frequency for calling primal heuristic */
   int                   freqofs;            /**< frequency offset for calling primal heuristic */
   int                   maxdepth;           /**< maximal depth level to call heuristic at (-1: no limit) */
   int                   delaypos;           /**< position in the delayed heuristics queue, or -1 if not delayed */
   unsigned int          timingmask;         /**< positions in the node solving loop where heuristic should be executed */
   SCIP_Bool             usessubscip;        /**< does the heuristic use a secondary SCIP instance? */
   SCIP_Bool             initialized;        /**< is primal heuristic initialized? */
   char                  dispchar;           /**< display character of primal heuristic */
};

#ifdef __cplusplus
}
#endif

#endif
