/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2003 Tobias Achterberg                              */
/*                                                                           */
/*                  2002-2003 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic Licence.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: struct_stat.h,v 1.4 2004/01/24 17:21:13 bzfpfend Exp $"

/**@file   struct_stat.h
 * @brief  datastructures for problem statistics
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __STRUCT_STAT_H__
#define __STRUCT_STAT_H__


#include "def.h"
#include "type_stat.h"
#include "type_clock.h"
#include "type_history.h"


/** problem and runtime specific statistics */
struct Stat
{
   CLOCK*           solvingtime;        /**< total time used for solving (including presolving) the current problem */
   CLOCK*           presolvingtime;     /**< total time used for presolving the current problem */
   CLOCK*           primallptime;       /**< primal LP solution time */
   CLOCK*           duallptime;         /**< dual LP solution time */
   CLOCK*           strongbranchtime;   /**< strong branching time */
   CLOCK*           lpsoltime;          /**< time needed for storing feasible LP solutions */
   CLOCK*           pseudosoltime;      /**< time needed for storing feasible pseudo solutions */
   CLOCK*           redcoststrtime;     /**< time needed for reduced cost strengthening */
   CLOCK*           nodeactivationtime; /**< time needed for path switching and activating nodes */
   HISTORY*         glblphistory;       /**< average branching history for downwards and upwards branching on LP */
   Longint          nlpiterations;      /**< number of simplex iterations (primal + dual) */
   Longint          nprimallpiterations;/**< number of iterations in primal simplex */
   Longint          nduallpiterations;  /**< number of iterations in dual simplex */
   Longint          ndivinglpiterations;/**< number of iterations in diving */
   Longint          nsblpiterations;    /**< number of simplex iterations used in strong branching */
   Longint          nredcoststrcalls;   /**< number of times, reduced cost strengthening was called */
   Longint          nredcoststrfound;   /**< number of reduced cost strengthenings found */
   Longint          nnodes;             /**< number of nodes processed (including active node) */
   Longint          nboundchanges;      /**< number of times a variable's bound has been changed */
   Longint          nlpsolsfound;       /**< number of CIP-feasible LP solutions found so far */
   Longint          npssolsfound;       /**< number of CIP-feasible pseudo solutions found so far */
   Longint          lastdispnode;       /**< last node for which an information line was displayed */
   Longint          lastdivenode;       /**< last node where LP diving was applied */
   int              nvaridx;            /**< number of used variable indices */
   int              ncolidx;            /**< number of used column indices */
   int              nrowidx;            /**< number of used row indices */
   int              marked_nvaridx;     /**< number of used variable indices before solving started */
   int              marked_ncolidx;     /**< number of used column indices before solving started */
   int              marked_nrowidx;     /**< number of used row indices before solving started */
   int              lpcount;            /**< internal LP counter, where all SCIPlpSolve() calls are counted */
   int              nlps;               /**< number of LPs solved (primal + dual) with at least 1 iteration */
   int              nprimallps;         /**< number of primal LPs solved */
   int              nduallps;           /**< number of dual LPs solved */
   int              ndivinglps;         /**< number of LPs solved during diving */
   int              nstrongbranchs;     /**< number of strong branching calls */
   int              npricerounds;       /**< number of pricing rounds performed in actual node */
   int              nseparounds;        /**< number of separation rounds performed in actual node */
   int              ndisplines;         /**< number of displayed information lines */
   int              maxdepth;           /**< maximal depth of all processed nodes */
   int              plungedepth;        /**< actual plunging depth (successive times, a child was selected as next node) */
   Bool             memsavemode;        /**< should algorithms be switched to memory saving mode? */
};


#endif
