/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2005 Tobias Achterberg                              */
/*                  2002-2005 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: branch.c,v 1.69 2005/08/24 17:26:36 bzfpfend Exp $"

/**@file   branch.c
 * @brief  methods for branching rules and branching candidate storage
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>
#include <string.h>

#include "scip/def.h"
#include "scip/memory.h"
#include "scip/set.h"
#include "scip/stat.h"
#include "scip/clock.h"
#include "scip/paramset.h"
#include "scip/event.h"
#include "scip/lp.h"
#include "scip/var.h"
#include "scip/prob.h"
#include "scip/tree.h"
#include "scip/sepastore.h"
#include "scip/scip.h"
#include "scip/branch.h"

#include "scip/struct_branch.h"



/*
 * memory growing methods for dynamically allocated arrays
 */

/** ensures, that lpcands array can store at least num entries */
static
SCIP_RETCODE ensureLpcandsSize(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_SET*             set,                /**< global SCIP settings */
   int                   num                 /**< minimum number of entries to store */
   )
{
   assert(branchcand->nlpcands <= branchcand->lpcandssize);
   
   if( num > branchcand->lpcandssize )
   {
      int newsize;

      newsize = SCIPsetCalcMemGrowSize(set, num);
      SCIP_ALLOC( BMSreallocMemoryArray(&branchcand->lpcands, newsize) );
      SCIP_ALLOC( BMSreallocMemoryArray(&branchcand->lpcandssol, newsize) );
      SCIP_ALLOC( BMSreallocMemoryArray(&branchcand->lpcandsfrac, newsize) );
      branchcand->lpcandssize = newsize;
   }
   assert(num <= branchcand->lpcandssize);

   return SCIP_OKAY;
}

/** ensures, that pseudocands array can store at least num entries */
static
SCIP_RETCODE ensurePseudocandsSize(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_SET*             set,                /**< global SCIP settings */
   int                   num                 /**< minimum number of entries to store */
   )
{
   assert(branchcand->npseudocands <= branchcand->pseudocandssize);
   
   if( num > branchcand->pseudocandssize )
   {
      int newsize;

      newsize = SCIPsetCalcMemGrowSize(set, num);
      SCIP_ALLOC( BMSreallocMemoryArray(&branchcand->pseudocands, newsize) );
      branchcand->pseudocandssize = newsize;
   }
   assert(num <= branchcand->pseudocandssize);

   return SCIP_OKAY;
}



/*
 * branching candidate storage methods
 */

/** creates a branching candidate storage */
SCIP_RETCODE SCIPbranchcandCreate(
   SCIP_BRANCHCAND**     branchcand          /**< pointer to store branching candidate storage */
   )
{
   assert(branchcand != NULL);

   SCIP_ALLOC( BMSallocMemory(branchcand) );
   (*branchcand)->lpcands = NULL;
   (*branchcand)->lpcandssol = NULL;
   (*branchcand)->lpcandsfrac = NULL;
   (*branchcand)->pseudocands = NULL;
   (*branchcand)->lpcandssize = 0;
   (*branchcand)->nlpcands = 0;
   (*branchcand)->npriolpcands = 0;
   (*branchcand)->npriolpbins = 0;
   (*branchcand)->lpmaxpriority = INT_MIN;
   (*branchcand)->pseudocandssize = 0;
   (*branchcand)->npseudocands = 0;
   (*branchcand)->npriopseudocands = 0;
   (*branchcand)->npriopseudobins = 0;
   (*branchcand)->npriopseudoints = 0;
   (*branchcand)->pseudomaxpriority = INT_MIN;
   (*branchcand)->validlpcandslp = -1;
   
   return SCIP_OKAY;
}

/** frees branching candidate storage */
SCIP_RETCODE SCIPbranchcandFree(
   SCIP_BRANCHCAND**     branchcand          /**< pointer to store branching candidate storage */
   )
{
   assert(branchcand != NULL);

   BMSfreeMemoryArrayNull(&(*branchcand)->lpcands);
   BMSfreeMemoryArrayNull(&(*branchcand)->lpcandssol);
   BMSfreeMemoryArrayNull(&(*branchcand)->lpcandsfrac);
   BMSfreeMemoryArrayNull(&(*branchcand)->pseudocands);
   BMSfreeMemory(branchcand);

   return SCIP_OKAY;
}

/** calculates branching candidates for LP solution branching (fractional variables) */
static
SCIP_RETCODE branchcandCalcLPCands(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   )
{
   assert(branchcand != NULL);
   assert(stat != NULL);
   assert(branchcand->validlpcandslp <= stat->lpcount);
   assert(lp != NULL);
   assert(lp->solved);
   assert(SCIPlpGetSolstat(lp) == SCIP_LPSOLSTAT_OPTIMAL || SCIPlpGetSolstat(lp) == SCIP_LPSOLSTAT_UNBOUNDEDRAY);

   SCIPdebugMessage("calculating LP branching candidates: validlp=%d, lpcount=%d\n",
      branchcand->validlpcandslp, stat->lpcount);

   /* check, if the current LP branching candidate array is invalid */
   if( branchcand->validlpcandslp < stat->lpcount )
   {
      SCIP_COL** cols;
      SCIP_VAR* var;
      SCIP_COL* col;
      SCIP_Real primsol;
      SCIP_Real frac;
      SCIP_VARTYPE vartype;
      int branchpriority;
      int ncols;
      int c;
      int insertpos;

      SCIPdebugMessage(" -> recalculating LP branching candidates\n");

      cols = SCIPlpGetCols(lp);
      ncols = SCIPlpGetNCols(lp);

      /* construct the LP branching candidate set, moving the candidates with maximal priority to the front */
      SCIP_CALL( ensureLpcandsSize(branchcand, set, ncols) );

      branchcand->lpmaxpriority = INT_MIN;
      branchcand->nlpcands = 0;
      branchcand->npriolpcands = 0;
      branchcand->npriolpbins = 0;
      for( c = 0; c < ncols; ++c )
      {
         col = cols[c];
         assert(col != NULL);
         assert(col->lppos == c);
         assert(col->lpipos >= 0);

         primsol = SCIPcolGetPrimsol(col);
         assert(primsol < SCIP_INVALID);
         assert(SCIPsetIsFeasGE(set, primsol, col->lb));
         assert(SCIPsetIsFeasLE(set, primsol, col->ub));

         var = col->var;
         assert(var != NULL);
         assert(SCIPvarGetStatus(var) == SCIP_VARSTATUS_COLUMN);
         assert(SCIPvarGetCol(var) == col);
         
         /* LP branching candidates are fractional binary and integer variables */
         vartype = SCIPvarGetType(var);
         if( vartype != SCIP_VARTYPE_BINARY && vartype != SCIP_VARTYPE_INTEGER )
            continue;

         /* ignore fixed variables (due to numerics, it is possible, that the LP solution of a fixed integer variable
          * (with large fixed value) is fractional in terms of absolute feasibility measure)
          */
         if( SCIPvarGetLbLocal(var) >= SCIPvarGetUbLocal(var) - 0.5 )
            continue;

         /* check, if the LP solution value is fractional */
         frac = SCIPsetFeasFrac(set, primsol);
         if( SCIPsetIsFeasFracIntegral(set, frac) )
            continue;

         assert(branchcand->nlpcands < branchcand->lpcandssize);

         /* insert candidate in candidate list */
         branchpriority = SCIPvarGetBranchPriority(var);
         insertpos = branchcand->nlpcands;
         branchcand->nlpcands++;
         if( branchpriority > branchcand->lpmaxpriority )
         {
            /* candidate has higher priority than the current maximum:
             * move it to the front and declare it to be the single best candidate
             */
            if( insertpos != 0 )
            {
               branchcand->lpcands[insertpos] = branchcand->lpcands[0];
               branchcand->lpcandssol[insertpos] = branchcand->lpcandssol[0];
               branchcand->lpcandsfrac[insertpos] = branchcand->lpcandsfrac[0];
               insertpos = 0;
            }
            branchcand->npriolpcands = 1;
            branchcand->npriolpbins = (vartype == SCIP_VARTYPE_BINARY ? 1 : 0);
            branchcand->lpmaxpriority = branchpriority;
         }
         else if( branchpriority == branchcand->lpmaxpriority )
         {
            /* candidate has equal priority as the current maximum:
             * move away the first non-maximal priority candidate, move the current candidate to the correct
             * slot (binaries first) and increase the number of maximal priority candidates
             */
            if( insertpos != branchcand->npriolpcands )
            {
               branchcand->lpcands[insertpos] = branchcand->lpcands[branchcand->npriolpcands];
               branchcand->lpcandssol[insertpos] = branchcand->lpcandssol[branchcand->npriolpcands];
               branchcand->lpcandsfrac[insertpos] = branchcand->lpcandsfrac[branchcand->npriolpcands];
               insertpos = branchcand->npriolpcands;
            }
            branchcand->npriolpcands++;
            if( vartype == SCIP_VARTYPE_BINARY )
            {
               if( insertpos != branchcand->npriolpbins )
               {
                  branchcand->lpcands[insertpos] = branchcand->lpcands[branchcand->npriolpbins];
                  branchcand->lpcandssol[insertpos] = branchcand->lpcandssol[branchcand->npriolpbins];
                  branchcand->lpcandsfrac[insertpos] = branchcand->lpcandsfrac[branchcand->npriolpbins];
                  insertpos = branchcand->npriolpbins;
               }
               branchcand->npriolpbins++;
            }
         }
         branchcand->lpcands[insertpos] = var;
         branchcand->lpcandssol[insertpos] = primsol;
         branchcand->lpcandsfrac[insertpos] = frac;

         SCIPdebugMessage(" -> candidate %d: var=<%s>, sol=%g, frac=%g, prio=%d (max: %d) -> pos %d\n", 
            branchcand->nlpcands, SCIPvarGetName(var), primsol, frac, branchpriority, branchcand->lpmaxpriority,
            insertpos);
      }

      branchcand->validlpcandslp = stat->lpcount;
   }
   assert(0 <= branchcand->npriolpcands && branchcand->npriolpcands <= branchcand->nlpcands);

   SCIPdebugMessage(" -> %d fractional variables (%d of maximal priority)\n", branchcand->nlpcands, branchcand->npriolpcands);

   return SCIP_OKAY;
}

/** gets branching candidates for LP solution branching (fractional variables) */
SCIP_RETCODE SCIPbranchcandGetLPCands(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_VAR***           lpcands,            /**< pointer to store the array of LP branching candidates, or NULL */
   SCIP_Real**           lpcandssol,         /**< pointer to store the array of LP candidate solution values, or NULL */
   SCIP_Real**           lpcandsfrac,        /**< pointer to store the array of LP candidate fractionalities, or NULL */
   int*                  nlpcands,           /**< pointer to store the number of LP branching candidates, or NULL */
   int*                  npriolpcands        /**< pointer to store the number of candidates with maximal priority, or NULL */
   )
{
   /* calculate branching candidates */
   SCIP_CALL( branchcandCalcLPCands(branchcand, set, stat, lp) );

   /* assign return values */
   if( lpcands != NULL )
      *lpcands = branchcand->lpcands;
   if( lpcandssol != NULL )
      *lpcandssol = branchcand->lpcandssol;
   if( lpcandsfrac != NULL )
      *lpcandsfrac = branchcand->lpcandsfrac;
   if( nlpcands != NULL )
      *nlpcands = branchcand->nlpcands;
   if( npriolpcands != NULL )
      *npriolpcands = (set->branch_preferbinary && branchcand->npriolpbins > 0 ? branchcand->npriolpbins
         : branchcand->npriolpcands);

   return SCIP_OKAY;
}

/** gets branching candidates for pseudo solution branching (nonfixed variables) */
SCIP_RETCODE SCIPbranchcandGetPseudoCands(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_VAR***           pseudocands,        /**< pointer to store the array of pseudo branching candidates, or NULL */
   int*                  npseudocands,       /**< pointer to store the number of pseudo branching candidates, or NULL */
   int*                  npriopseudocands    /**< pointer to store the number of candidates with maximal priority, or NULL */
   )
{
   assert(branchcand != NULL);

#ifndef NDEBUG
   /* check, if the current pseudo branching candidate array is correct */
   {
      SCIP_VAR* var;
      int npcs;
      int v;
      
      assert(prob != NULL);
      
      /* pseudo branching candidates are non-fixed binary, integer, and implicit integer variables */
      npcs = 0;
      for( v = 0; v < prob->nbinvars + prob->nintvars + prob->nimplvars; ++v )
      {
         var = prob->vars[v];
         assert(var != NULL);
         assert(SCIPvarGetStatus(var) == SCIP_VARSTATUS_LOOSE || SCIPvarGetStatus(var) == SCIP_VARSTATUS_COLUMN);
         assert(SCIPvarGetType(var) == SCIP_VARTYPE_BINARY
            || SCIPvarGetType(var) == SCIP_VARTYPE_INTEGER
            || SCIPvarGetType(var) == SCIP_VARTYPE_IMPLINT);
         assert(SCIPsetIsFeasIntegral(set, SCIPvarGetLbLocal(var)));
         assert(SCIPsetIsFeasIntegral(set, SCIPvarGetUbLocal(var)));
         assert(SCIPsetIsLE(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)));

         if( SCIPsetIsLT(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)) )
         {
            assert(0 <= var->pseudocandindex && var->pseudocandindex < branchcand->npseudocands);
            assert(branchcand->pseudocands[var->pseudocandindex] == var);
            npcs++;
         }
         else
         {
            assert(var->pseudocandindex == -1);
         }
      }
      assert(branchcand->npseudocands == npcs);
   }
#endif

   /* assign return values */
   if( pseudocands != NULL )
      *pseudocands = branchcand->pseudocands;
   if( npseudocands != NULL )
      *npseudocands = branchcand->npseudocands;
   if( npriopseudocands != NULL )
      *npriopseudocands = (set->branch_preferbinary && branchcand->npriopseudobins > 0 ? branchcand->npriopseudobins
         : branchcand->npriopseudocands);

   return SCIP_OKAY;
}

/** gets number of branching candidates for pseudo solution branching (nonfixed variables) */
int SCIPbranchcandGetNPseudoCands(
   SCIP_BRANCHCAND*      branchcand          /**< branching candidate storage */
   )
{
   assert(branchcand != NULL);

   return branchcand->npseudocands;
}

/** gets number of branching candidates with maximal branch priority for pseudo solution branching */
int SCIPbranchcandGetNPrioPseudoCands(
   SCIP_BRANCHCAND*      branchcand          /**< branching candidate storage */
   )
{
   assert(branchcand != NULL);

   return branchcand->npriopseudocands;
}

/** gets number of binary branching candidates with maximal branch priority for pseudo solution branching */
int SCIPbranchcandGetNPrioPseudoBins(
   SCIP_BRANCHCAND*      branchcand          /**< branching candidate storage */
   )
{
   assert(branchcand != NULL);

   return branchcand->npriopseudobins;
}

/** gets number of integer branching candidates with maximal branch priority for pseudo solution branching */
int SCIPbranchcandGetNPrioPseudoInts(
   SCIP_BRANCHCAND*      branchcand          /**< branching candidate storage */
   )
{
   assert(branchcand != NULL);

   return branchcand->npriopseudoints;
}

/** gets number of implicit integer branching candidates with maximal branch priority for pseudo solution branching */
int SCIPbranchcandGetNPrioPseudoImpls(
   SCIP_BRANCHCAND*      branchcand          /**< branching candidate storage */
   )
{
   assert(branchcand != NULL);

   return branchcand->npriopseudocands - branchcand->npriopseudobins - branchcand->npriopseudoints;
}

/** insert pseudocand at given position, or to the first positions of the maximal priority candidates, using the
 *  given position as free slot for the other candidates
 */
static
void branchcandInsertPseudoCand(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_VAR*             var,                /**< variable to insert */
   int                   insertpos           /**< free position to insert the variable */
   )
{
   SCIP_VARTYPE vartype;
   int branchpriority;

   assert(branchcand != NULL);
   assert(var != NULL);
   assert(branchcand->npriopseudocands <= insertpos && insertpos < branchcand->npseudocands);
   assert(branchcand->npseudocands <= branchcand->pseudocandssize);

   vartype = SCIPvarGetType(var);
   branchpriority = SCIPvarGetBranchPriority(var);

   SCIPdebugMessage("inserting pseudo candidate <%s> of type %d and priority %d into candidate set at position %d (maxprio: %d)\n",
      SCIPvarGetName(var), vartype, branchpriority, insertpos, branchcand->pseudomaxpriority);

   /* insert the variable into pseudocands, making sure, that the highest priority candidates are at the front
    * and ordered binaries, integers, implicit integers
    */
   if( branchpriority > branchcand->pseudomaxpriority )
   {
      /* candidate has higher priority than the current maximum:
       * move it to the front and declare it to be the single best candidate
       */
      if( insertpos != 0 )
      {
         branchcand->pseudocands[insertpos] = branchcand->pseudocands[0];
         branchcand->pseudocands[insertpos]->pseudocandindex = insertpos;
         insertpos = 0;
      }
      branchcand->npriopseudocands = 1;
      branchcand->npriopseudobins = (vartype == SCIP_VARTYPE_BINARY ? 1 : 0);
      branchcand->npriopseudoints = (vartype == SCIP_VARTYPE_INTEGER ? 1 : 0);
      branchcand->pseudomaxpriority = branchpriority;
   }
   else if( branchpriority == branchcand->pseudomaxpriority )
   {
      /* candidate has equal priority as the current maximum:
       * move away the first non-maximal priority candidate, move the current candidate to the correct
       * slot (binaries first, integers next, implicits last) and increase the number of maximal priority candidates
       */
      if( insertpos != branchcand->npriopseudocands )
      {
         branchcand->pseudocands[insertpos] = branchcand->pseudocands[branchcand->npriopseudocands];
         branchcand->pseudocands[insertpos]->pseudocandindex = insertpos;
         insertpos = branchcand->npriopseudocands;
      }
      branchcand->npriopseudocands++;
      if( vartype == SCIP_VARTYPE_BINARY || vartype == SCIP_VARTYPE_INTEGER )
      {
         if( insertpos != branchcand->npriopseudobins + branchcand->npriopseudoints )
         {
            branchcand->pseudocands[insertpos] =
               branchcand->pseudocands[branchcand->npriopseudobins + branchcand->npriopseudoints];
            branchcand->pseudocands[insertpos]->pseudocandindex = insertpos;
            insertpos = branchcand->npriopseudobins + branchcand->npriopseudoints;
         }
         branchcand->npriopseudoints++;

         if( vartype == SCIP_VARTYPE_BINARY )
         {
            if( insertpos != branchcand->npriopseudobins )
            {
               branchcand->pseudocands[insertpos] = branchcand->pseudocands[branchcand->npriopseudobins];
               branchcand->pseudocands[insertpos]->pseudocandindex = insertpos;
               insertpos = branchcand->npriopseudobins;
            }
            branchcand->npriopseudobins++;
            branchcand->npriopseudoints--;
         }
      }
   }
   branchcand->pseudocands[insertpos] = var;
   var->pseudocandindex = insertpos;

   SCIPdebugMessage(" -> inserted at position %d (npriopseudocands=%d)\n", insertpos, branchcand->npriopseudocands);

   assert(0 <= branchcand->npriopseudocands && branchcand->npriopseudocands <= branchcand->npseudocands);
   assert(0 <= branchcand->npriopseudobins && branchcand->npriopseudobins <= branchcand->npriopseudocands);
   assert(0 <= branchcand->npriopseudoints && branchcand->npriopseudoints <= branchcand->npriopseudocands);
}

/** sorts the pseudo branching candidates, such that the candidates of maximal priority are at the front,
 *  ordered by binaries, integers, implicit integers
 */
static
void branchcandSortPseudoCands(
   SCIP_BRANCHCAND*      branchcand          /**< branching candidate storage */
   )
{
   SCIP_VAR* var;
   int i;

   assert(branchcand != NULL);
   assert(branchcand->npriopseudocands == 0); /* is only be called after removal of last maximal candidate */
   assert(branchcand->npriopseudobins == 0);
   assert(branchcand->npriopseudoints == 0);

   SCIPdebugMessage("resorting pseudo candidates\n");

   branchcand->pseudomaxpriority = INT_MIN;
   
   for( i = 0; i < branchcand->npseudocands; ++i )
   {
      var = branchcand->pseudocands[i];
      assert(var->pseudocandindex == i);

      if( SCIPvarGetBranchPriority(var) >= branchcand->pseudomaxpriority )
         branchcandInsertPseudoCand(branchcand, var, i);
   }

   assert(0 <= branchcand->npriopseudocands && branchcand->npriopseudocands <= branchcand->npseudocands);
   assert(0 <= branchcand->npriopseudobins && branchcand->npriopseudobins <= branchcand->npriopseudocands);
   assert(0 <= branchcand->npriopseudoints && branchcand->npriopseudoints <= branchcand->npriopseudocands);
}

/** removes pseudo candidate from pseudocands array
 */
static
void branchcandRemovePseudoCand(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_VAR*             var                 /**< variable to remove */
   )
{
   SCIP_VARTYPE vartype;
   int branchpriority;
   int freepos;

   assert(branchcand != NULL);
   assert(var != NULL);
   assert(var->pseudocandindex < branchcand->npseudocands);
   assert(branchcand->pseudocands[var->pseudocandindex] == var);
   assert(branchcand->pseudocands[branchcand->npseudocands-1] != NULL);

   vartype = SCIPvarGetType(var);
   branchpriority = SCIPvarGetBranchPriority(var);

   SCIPdebugMessage("removing pseudo candidate <%s> of type %d and priority %d at %d from candidate set (maxprio: %d)\n",
      SCIPvarGetName(var), vartype, branchpriority, var->pseudocandindex, branchcand->pseudomaxpriority);

   /* delete the variable from pseudocands, making sure, that the highest priority candidates are at the front
    * and ordered binaries, integers, implicit integers
    */
   freepos = var->pseudocandindex;
   var->pseudocandindex = -1;
   assert(0 <= freepos && freepos < branchcand->npseudocands);

   if( freepos < branchcand->npriopseudobins )
   {
      /* a binary candidate of maximal priority was removed */
      assert(vartype == SCIP_VARTYPE_BINARY);
      assert(branchpriority == branchcand->pseudomaxpriority);
      if( freepos != branchcand->npriopseudobins - 1 )
      {
         branchcand->pseudocands[freepos] = branchcand->pseudocands[branchcand->npriopseudobins - 1];
         branchcand->pseudocands[freepos]->pseudocandindex = freepos;
         freepos = branchcand->npriopseudobins - 1;
      }
      branchcand->npriopseudobins--;
      branchcand->npriopseudoints++;
   }
   if( freepos < branchcand->npriopseudobins + branchcand->npriopseudoints )
   {
      /* a binary or integer candidate of maximal priority was removed */
      assert(vartype == SCIP_VARTYPE_BINARY || vartype == SCIP_VARTYPE_INTEGER);
      assert(branchpriority == branchcand->pseudomaxpriority);
      if( freepos != branchcand->npriopseudobins + branchcand->npriopseudoints - 1 )
      {
         branchcand->pseudocands[freepos] =
            branchcand->pseudocands[branchcand->npriopseudobins + branchcand->npriopseudoints - 1];
         branchcand->pseudocands[freepos]->pseudocandindex = freepos;
         freepos = branchcand->npriopseudobins + branchcand->npriopseudoints - 1;
      }
      branchcand->npriopseudoints--;
   }
   if( freepos < branchcand->npriopseudocands )
   {
      /* a candidate of maximal priority was removed */
      assert(branchpriority == branchcand->pseudomaxpriority);
      if( freepos != branchcand->npriopseudocands - 1 )
      {
         branchcand->pseudocands[freepos] = branchcand->pseudocands[branchcand->npriopseudocands - 1];
         branchcand->pseudocands[freepos]->pseudocandindex = freepos;
         freepos = branchcand->npriopseudocands - 1;
      }
      branchcand->npriopseudocands--;
   }
   if( freepos != branchcand->npseudocands - 1 )
   {
      branchcand->pseudocands[freepos] = branchcand->pseudocands[branchcand->npseudocands - 1];
      branchcand->pseudocands[freepos]->pseudocandindex = freepos;
   }
   branchcand->npseudocands--;

   assert(0 <= branchcand->npriopseudocands && branchcand->npriopseudocands <= branchcand->npseudocands);
   assert(0 <= branchcand->npriopseudobins && branchcand->npriopseudobins <= branchcand->npriopseudocands);
   assert(0 <= branchcand->npriopseudoints && branchcand->npriopseudoints <= branchcand->npriopseudocands);

   /* if all maximal priority candidates were removed, resort the array s.t. the new maximal priority candidates
    * are at the front
    */
   if( branchcand->npriopseudocands == 0 )
      branchcandSortPseudoCands(branchcand);
}

/** removes variable from branching candidate list */
SCIP_RETCODE SCIPbranchcandRemoveVar(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_VAR*             var                 /**< variable that changed its bounds */
   )
{
   assert(var != NULL);

   /* make sure, variable is not member of the pseudo branching candidate list */
   if( var->pseudocandindex >= 0 )
   {
      branchcandRemovePseudoCand(branchcand, var);
   }

   return SCIP_OKAY;
}

/** updates branching candidate list for a given variable */
SCIP_RETCODE SCIPbranchcandUpdateVar(
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var                 /**< variable that changed its bounds */
   )
{
   assert(branchcand != NULL);
   assert(var != NULL);
   assert(SCIPsetIsFeasLE(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)));
   
   if( (SCIPvarGetStatus(var) == SCIP_VARSTATUS_LOOSE || SCIPvarGetStatus(var) == SCIP_VARSTATUS_COLUMN)
      && SCIPvarGetType(var) != SCIP_VARTYPE_CONTINUOUS
      && SCIPsetIsLT(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)) )
   {
      /* variable is neither continuous nor fixed: make sure it is member of the pseudo branching candidate list */
      if( var->pseudocandindex == -1 )
      {
         SCIP_CALL( ensurePseudocandsSize(branchcand, set, branchcand->npseudocands+1) );

         branchcand->npseudocands++;
         branchcandInsertPseudoCand(branchcand, var, branchcand->npseudocands-1);
      }
   }
   else
   {
      assert(SCIPvarGetStatus(var) == SCIP_VARSTATUS_ORIGINAL
         || SCIPvarGetStatus(var) == SCIP_VARSTATUS_FIXED
         || SCIPvarGetStatus(var) == SCIP_VARSTATUS_AGGREGATED
         || SCIPvarGetStatus(var) == SCIP_VARSTATUS_MULTAGGR
         || SCIPvarGetStatus(var) == SCIP_VARSTATUS_NEGATED
         || SCIPvarGetType(var) == SCIP_VARTYPE_CONTINUOUS
         || SCIPsetIsEQ(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)));

      /* variable is continuous or fixed: make sure it is not member of the pseudo branching candidate list */
      SCIP_CALL( SCIPbranchcandRemoveVar(branchcand, var) );
   }

   return SCIP_OKAY;
}




/*
 * branching rule methods
 */

/** compares two branching rules w. r. to their priority */
SCIP_DECL_SORTPTRCOMP(SCIPbranchruleComp)
{  /*lint --e{715}*/
   return ((SCIP_BRANCHRULE*)elem2)->priority - ((SCIP_BRANCHRULE*)elem1)->priority;
}

/** method to call, when the priority of a branching rule was changed */
static
SCIP_DECL_PARAMCHGD(paramChgdBranchrulePriority)
{  /*lint --e{715}*/
   SCIP_PARAMDATA* paramdata;

   paramdata = SCIPparamGetData(param);
   assert(paramdata != NULL);

   /* use SCIPsetBranchrulePriority() to mark the branchrules unsorted */
   SCIP_CALL( SCIPsetBranchrulePriority(scip, (SCIP_BRANCHRULE*)paramdata, SCIPparamGetInt(param)) ); /*lint !e740*/

   return SCIP_OKAY;
}

/** creates a branching rule */
SCIP_RETCODE SCIPbranchruleCreate(
   SCIP_BRANCHRULE**     branchrule,         /**< pointer to store branching rule */
   BMS_BLKMEM*           blkmem,             /**< block memory for parameter settings */
   SCIP_SET*             set,                /**< global SCIP settings */
   const char*           name,               /**< name of branching rule */
   const char*           desc,               /**< description of branching rule */
   int                   priority,           /**< priority of the branching rule */
   int                   maxdepth,           /**< maximal depth level, up to which this branching rule should be used (or -1) */
   SCIP_Real             maxbounddist,       /**< maximal relative distance from current node's dual bound to primal bound
                                              *   compared to best node's dual bound for applying branching rule
                                              *   (0.0: only on current best node, 1.0: on all nodes) */
   SCIP_DECL_BRANCHFREE  ((*branchfree)),    /**< destructor of branching rule */
   SCIP_DECL_BRANCHINIT  ((*branchinit)),    /**< initialize branching rule */
   SCIP_DECL_BRANCHEXIT  ((*branchexit)),    /**< deinitialize branching rule */
   SCIP_DECL_BRANCHINITSOL((*branchinitsol)),/**< solving process initialization method of branching rule */
   SCIP_DECL_BRANCHEXITSOL((*branchexitsol)),/**< solving process deinitialization method of branching rule */
   SCIP_DECL_BRANCHEXECLP((*branchexeclp)),  /**< branching execution method for fractional LP solutions */
   SCIP_DECL_BRANCHEXECPS((*branchexecps)),  /**< branching execution method for not completely fixed pseudo solutions */
   SCIP_BRANCHRULEDATA*  branchruledata      /**< branching rule data */
   )
{
   char paramname[SCIP_MAXSTRLEN];
   char paramdesc[SCIP_MAXSTRLEN];

   assert(branchrule != NULL);
   assert(name != NULL);
   assert(desc != NULL);

   SCIP_ALLOC( BMSallocMemory(branchrule) );
   SCIP_ALLOC( BMSduplicateMemoryArray(&(*branchrule)->name, name, strlen(name)+1) );
   SCIP_ALLOC( BMSduplicateMemoryArray(&(*branchrule)->desc, desc, strlen(desc)+1) );
   (*branchrule)->priority = priority;
   (*branchrule)->maxdepth = maxdepth;
   (*branchrule)->maxbounddist = maxbounddist;
   (*branchrule)->branchfree = branchfree;
   (*branchrule)->branchinit = branchinit;
   (*branchrule)->branchexit = branchexit;
   (*branchrule)->branchinitsol = branchinitsol;
   (*branchrule)->branchexitsol = branchexitsol;
   (*branchrule)->branchexeclp = branchexeclp;
   (*branchrule)->branchexecps = branchexecps;
   (*branchrule)->branchruledata = branchruledata;
   SCIP_CALL( SCIPclockCreate(&(*branchrule)->branchclock, SCIP_CLOCKTYPE_DEFAULT) );
   (*branchrule)->nlpcalls = 0;
   (*branchrule)->npseudocalls = 0;
   (*branchrule)->ncutoffs = 0;
   (*branchrule)->ncutsfound = 0;
   (*branchrule)->nconssfound = 0;
   (*branchrule)->ndomredsfound = 0;
   (*branchrule)->nchildren = 0;
   (*branchrule)->initialized = FALSE;

   /* add parameters */
   sprintf(paramname, "branching/%s/priority", name);
   sprintf(paramdesc, "priority of branching rule <%s>", name);
   SCIP_CALL( SCIPsetAddIntParam(set, blkmem, paramname, paramdesc,
         &(*branchrule)->priority, priority, INT_MIN, INT_MAX, 
         paramChgdBranchrulePriority, (SCIP_PARAMDATA*)(*branchrule)) ); /*lint !e740*/
   sprintf(paramname, "branching/%s/maxdepth", name);
   sprintf(paramdesc, "maximal depth level, up to which branching rule <%s> should be used (-1 for no limit)", name);
   SCIP_CALL( SCIPsetAddIntParam(set, blkmem, paramname, paramdesc,
         &(*branchrule)->maxdepth, maxdepth, -1, INT_MAX, 
         NULL, NULL) ); /*lint !e740*/
   sprintf(paramname, "branching/%s/maxbounddist", name);
   sprintf(paramdesc, "maximal relative distance from current node's dual bound to primal bound compared to best node's dual bound for applying branching rule (0.0: only on current best node, 1.0: on all nodes)");
   SCIP_CALL( SCIPsetAddRealParam(set, blkmem, paramname, paramdesc,
         &(*branchrule)->maxbounddist, maxbounddist, 0.0, 1.0, 
         NULL, NULL) ); /*lint !e740*/

   return SCIP_OKAY;
}

/** frees memory of branching rule */   
SCIP_RETCODE SCIPbranchruleFree(
   SCIP_BRANCHRULE**     branchrule,         /**< pointer to branching rule data structure */
   SCIP_SET*             set                 /**< global SCIP settings */
   )
{
   assert(branchrule != NULL);
   assert(*branchrule != NULL);
   assert(!(*branchrule)->initialized);
   assert(set != NULL);

   /* call destructor of branching rule */
   if( (*branchrule)->branchfree != NULL )
   {
      SCIP_CALL( (*branchrule)->branchfree(set->scip, *branchrule) );
   }

   SCIPclockFree(&(*branchrule)->branchclock);
   BMSfreeMemoryArray(&(*branchrule)->name);
   BMSfreeMemoryArray(&(*branchrule)->desc);
   BMSfreeMemory(branchrule);

   return SCIP_OKAY;
}

/** initializes branching rule */
SCIP_RETCODE SCIPbranchruleInit(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set                 /**< global SCIP settings */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);

   if( branchrule->initialized )
   {
      SCIPerrorMessage("branching rule <%s> already initialized\n", branchrule->name);
      return SCIP_INVALIDCALL;
   }

   SCIPclockReset(branchrule->branchclock);

   branchrule->nlpcalls = 0;
   branchrule->npseudocalls = 0;
   branchrule->ncutoffs = 0;
   branchrule->ncutsfound = 0;
   branchrule->nconssfound = 0;
   branchrule->ndomredsfound = 0;
   branchrule->nchildren = 0;

   if( branchrule->branchinit != NULL )
   {
      SCIP_CALL( branchrule->branchinit(set->scip, branchrule) );
   }
   branchrule->initialized = TRUE;

   return SCIP_OKAY;
}

/** deinitializes branching rule */
SCIP_RETCODE SCIPbranchruleExit(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set                 /**< global SCIP settings */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);

   if( !branchrule->initialized )
   {
      SCIPerrorMessage("branching rule <%s> not initialized\n", branchrule->name);
      return SCIP_INVALIDCALL;
   }

   if( branchrule->branchexit != NULL )
   {
      SCIP_CALL( branchrule->branchexit(set->scip, branchrule) );
   }
   branchrule->initialized = FALSE;

   return SCIP_OKAY;
}

/** informs branching rule that the branch and bound process is being started */
SCIP_RETCODE SCIPbranchruleInitsol(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set                 /**< global SCIP settings */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);

   /* call solving process initialization method of branching rule */
   if( branchrule->branchinitsol != NULL )
   {
      SCIP_CALL( branchrule->branchinitsol(set->scip, branchrule) );
   }

   return SCIP_OKAY;
}

/** informs branching rule that the branch and bound process data is being freed */
SCIP_RETCODE SCIPbranchruleExitsol(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set                 /**< global SCIP settings */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);

   /* call solving process deinitialization method of branching rule */
   if( branchrule->branchexitsol != NULL )
   {
      SCIP_CALL( branchrule->branchexitsol(set->scip, branchrule) );
   }

   return SCIP_OKAY;
}

/** executes branching rule for fractional LP solution */
SCIP_RETCODE SCIPbranchruleExecLPSol(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_SEPASTORE*       sepastore,          /**< separation storage */
   SCIP_Real             cutoffbound,        /**< global upper cutoff bound */
   SCIP_Bool             allowaddcons,       /**< should adding constraints be allowed to avoid a branching? */
   SCIP_RESULT*          result              /**< pointer to store the result of the callback method */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);
   assert(tree != NULL);
   assert(tree->focusnode != NULL);
   assert(tree->nchildren == 0);
   assert(result != NULL);

   *result = SCIP_DIDNOTRUN;
   if( branchrule->branchexeclp != NULL
      && (branchrule->maxdepth == -1 || branchrule->maxdepth >= SCIPtreeGetCurrentDepth(tree)) )
   {
      SCIP_Real loclowerbound;
      SCIP_Real glblowerbound;

      loclowerbound = SCIPnodeGetLowerbound(tree->focusnode);
      glblowerbound = SCIPtreeGetLowerbound(tree, set);
      if( SCIPsetIsLE(set, loclowerbound - glblowerbound, branchrule->maxbounddist * (cutoffbound - glblowerbound)) )
      {
         SCIP_Longint oldndomchgs;
         int oldncutsstored;
         int oldnactiveconss;

         SCIPdebugMessage("executing LP branching rule <%s>\n", branchrule->name);

         oldndomchgs = stat->nboundchgs + stat->nholechgs;
         oldncutsstored = SCIPsepastoreGetNCutsStored(sepastore);
         oldnactiveconss = stat->nactiveconss;

         /* start timing */
         SCIPclockStart(branchrule->branchclock, set);
   
         /* call external method */
         SCIP_CALL( branchrule->branchexeclp(set->scip, branchrule, allowaddcons, result) );

         /* stop timing */
         SCIPclockStop(branchrule->branchclock, set);
      
         /* evaluate result */
         if( *result != SCIP_CUTOFF
            && *result != SCIP_CONSADDED
            && *result != SCIP_REDUCEDDOM
            && *result != SCIP_SEPARATED
            && *result != SCIP_BRANCHED
            && *result != SCIP_DIDNOTRUN )
         {
            SCIPerrorMessage("branching rule <%s> returned invalid result code <%d> from LP solution branching\n",
               branchrule->name, *result);
            return SCIP_INVALIDRESULT;
         }
         if( *result == SCIP_CONSADDED && !allowaddcons )
         {
            SCIPerrorMessage("branching rule <%s> added a constraint in LP solution branching without permission\n",
               branchrule->name);
            return SCIP_INVALIDRESULT;
         }

         /* update statistics */
         if( *result != SCIP_DIDNOTRUN )
            branchrule->nlpcalls++;
         if( *result == SCIP_CUTOFF )
            branchrule->ncutoffs++;
         if( *result != SCIP_BRANCHED )
         {
            assert(tree->nchildren == 0);
            branchrule->ndomredsfound += stat->nboundchgs + stat->nholechgs - oldndomchgs;
            branchrule->ncutsfound += SCIPsepastoreGetNCutsStored(sepastore) - oldncutsstored; /*lint !e776*/
            branchrule->nconssfound += stat->nactiveconss - oldnactiveconss; /*lint !e776*/
         }
         else
            branchrule->nchildren += tree->nchildren;
      }
   }

   return SCIP_OKAY;
}

/** executes branching rule for not completely fixed pseudo solution */
SCIP_RETCODE SCIPbranchruleExecPseudoSol(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_Real             cutoffbound,        /**< global upper cutoff bound */
   SCIP_Bool             allowaddcons,       /**< should adding constraints be allowed to avoid a branching? */
   SCIP_RESULT*          result              /**< pointer to store the result of the callback method */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);
   assert(tree != NULL);
   assert(tree->nchildren == 0);
   assert(result != NULL);

   *result = SCIP_DIDNOTRUN;
   if( branchrule->branchexecps != NULL
      && (branchrule->maxdepth == -1 || branchrule->maxdepth >= SCIPtreeGetCurrentDepth(tree)) )
   {
      SCIP_Real loclowerbound;
      SCIP_Real glblowerbound;

      loclowerbound = SCIPnodeGetLowerbound(tree->focusnode);
      glblowerbound = SCIPtreeGetLowerbound(tree, set);
      if( SCIPsetIsLE(set, loclowerbound - glblowerbound, branchrule->maxbounddist * (cutoffbound - glblowerbound)) )
      {
         SCIP_Longint oldndomchgs;
         SCIP_Longint oldnactiveconss;

         SCIPdebugMessage("executing pseudo branching rule <%s>\n", branchrule->name);

         oldndomchgs = stat->nboundchgs + stat->nholechgs;
         oldnactiveconss = stat->nactiveconss;

         /* start timing */
         SCIPclockStart(branchrule->branchclock, set);
   
         /* call external method */
         SCIP_CALL( branchrule->branchexecps(set->scip, branchrule, allowaddcons, result) );

         /* stop timing */
         SCIPclockStop(branchrule->branchclock, set);
      
         /* evaluate result */
         if( *result != SCIP_CUTOFF
            && *result != SCIP_CONSADDED
            && *result != SCIP_REDUCEDDOM
            && *result != SCIP_BRANCHED
            && *result != SCIP_DIDNOTRUN )
         {
            SCIPerrorMessage("branching rule <%s> returned invalid result code <%d> from pseudo solution branching\n",
               branchrule->name, *result);
            return SCIP_INVALIDRESULT;
         }
         if( *result == SCIP_CONSADDED && !allowaddcons )
         {
            SCIPerrorMessage("branching rule <%s> added a constraint in pseudo solution branching without permission\n",
               branchrule->name);
            return SCIP_INVALIDRESULT;
         }

         /* update statistics */
         if( *result != SCIP_DIDNOTRUN )
            branchrule->npseudocalls++;
         if( *result == SCIP_CUTOFF )
            branchrule->ncutoffs++;
         if( *result != SCIP_BRANCHED )
         {
            assert(tree->nchildren == 0);
            branchrule->ndomredsfound += stat->nboundchgs + stat->nholechgs - oldndomchgs;
            branchrule->nconssfound += stat->nactiveconss - oldnactiveconss;
         }
         else
            branchrule->nchildren += tree->nchildren;
      }
   }

   return SCIP_OKAY;
}

/** gets user data of branching rule */
SCIP_BRANCHRULEDATA* SCIPbranchruleGetData(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->branchruledata;
}

/** sets user data of branching rule; user has to free old data in advance! */
void SCIPbranchruleSetData(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_BRANCHRULEDATA*  branchruledata      /**< new branching rule user data */
   )
{
   assert(branchrule != NULL);

   branchrule->branchruledata = branchruledata;
}

/** gets name of branching rule */
const char* SCIPbranchruleGetName(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->name;
}

/** gets description of branching rule */
const char* SCIPbranchruleGetDesc(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->desc;
}

/** gets priority of branching rule */
int SCIPbranchruleGetPriority(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->priority;
}

/** sets priority of branching rule */
void SCIPbranchruleSetPriority(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_SET*             set,                /**< global SCIP settings */
   int                   priority            /**< new priority of the branching rule */
   )
{
   assert(branchrule != NULL);
   assert(set != NULL);
   
   branchrule->priority = priority;
   set->branchrulessorted = FALSE;
}

/** gets maximal depth level, up to which this branching rule should be used (-1 for no limit) */
int SCIPbranchruleGetMaxdepth(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->maxdepth;
}

/** sets maximal depth level, up to which this branching rule should be used (-1 for no limit) */
void SCIPbranchruleSetMaxdepth(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   int                   maxdepth            /**< new maxdepth of the branching rule */
   )
{
   assert(branchrule != NULL);
   assert(maxdepth >= -1);

   branchrule->maxdepth = maxdepth;
}

/** gets maximal relative distance from current node's dual bound to primal bound for applying branching rule */
SCIP_Real SCIPbranchruleGetMaxbounddist(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->maxbounddist;
}

/** sets maximal relative distance from current node's dual bound to primal bound for applying branching rule */
void SCIPbranchruleSetMaxbounddist(
   SCIP_BRANCHRULE*      branchrule,         /**< branching rule */
   SCIP_Real             maxbounddist        /**< new maxbounddist of the branching rule */
   )
{
   assert(branchrule != NULL);
   assert(maxbounddist >= -1);

   branchrule->maxbounddist = maxbounddist;
}

/** gets time in seconds used in this branching rule */
SCIP_Real SCIPbranchruleGetTime(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return SCIPclockGetTime(branchrule->branchclock);
}

/** gets the total number of times, the branching rule was called on an LP solution */
SCIP_Longint SCIPbranchruleGetNLPCalls(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->nlpcalls;
}

/** gets the total number of times, the branching rule was called on a pseudo solution */
SCIP_Longint SCIPbranchruleGetNPseudoCalls(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->npseudocalls;
}

/** gets the total number of times, the branching rule detected a cutoff */
SCIP_Longint SCIPbranchruleGetNCutoffs(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->ncutoffs;
}

/** gets the total number of cuts, the branching rule separated */
SCIP_Longint SCIPbranchruleGetNCutsFound(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->ncutsfound;
}

/** gets the total number of constraints, the branching rule added to the respective local nodes (not counting constraints
 *  that were added to the child nodes as branching decisions)
 */
SCIP_Longint SCIPbranchruleGetNConssFound(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->nconssfound;
}

/** gets the total number of domain reductions, the branching rule found */
SCIP_Longint SCIPbranchruleGetNDomredsFound(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->ndomredsfound;
}

/** gets the total number of children, the branching rule created */
SCIP_Longint SCIPbranchruleGetNChildren(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->nchildren;
}

/** is branching rule initialized? */
SCIP_Bool SCIPbranchruleIsInitialized(
   SCIP_BRANCHRULE*      branchrule          /**< branching rule */
   )
{
   assert(branchrule != NULL);

   return branchrule->initialized;
}




/*
 * branching methods
 */

/** calculates the branching score out of the gain predictions for a binary branching */
SCIP_Real SCIPbranchGetScore(
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< variable, of which the branching factor should be applied, or NULL */
   SCIP_Real             downgain,           /**< prediction of objective gain for rounding downwards */
   SCIP_Real             upgain              /**< prediction of objective gain for rounding upwards */
   )
{
   SCIP_Real score;
   SCIP_Real eps;

   assert(set != NULL);

   /* slightly increase gains, such that for zero gains, the branch factor comes into account */
   eps = SCIPsetSumepsilon(set);
   downgain = MAX(downgain, eps);
   upgain = MAX(upgain, eps);

   switch( set->branch_scorefunc )
   {
   case 's':  /* linear sum score function */
      /* weigh the two child nodes with branchscorefac and 1-branchscorefac */
      if( downgain > upgain )
         score = set->branch_scorefac * downgain + (1.0-set->branch_scorefac) * upgain;
      else
         score = set->branch_scorefac * upgain + (1.0-set->branch_scorefac) * downgain;
      break;

   case 'p':  /* product score function */
      score = downgain * upgain;
      break;

   default:
      SCIPerrorMessage("invalid branching score function <%c>\n", set->branch_scorefunc);
      score = 0.0;
      SCIPABORT();
   }

   /* apply the branch factor of the variable */
   if( var != NULL )
      score *= SCIPvarGetBranchFactor(var);

   return score;
}

/** calculates the branching score out of the gain predictions for a branching with arbitrary many children */
SCIP_Real SCIPbranchGetScoreMultiple(
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< variable, of which the branching factor should be applied, or NULL */
   int                   nchildren,          /**< number of children that the branching will create */
   SCIP_Real*            gains               /**< prediction of objective gain for each child */
   )
{
   SCIP_Real min1;
   SCIP_Real min2;
   int c;

   assert(nchildren == 0 || gains != NULL);

   /* search for the two minimal gains in the child list and use these to calculate the branching score */
   min1 = SCIPsetInfinity(set);
   min2 = SCIPsetInfinity(set);
   for( c = 0; c < nchildren; ++c )
   {
      if( gains[c] < min1 )
      {
         min2 = min1;
         min1 = gains[c];
      }
      else if( gains[c] < min2 )
         min2 = gains[c];
   }

   return SCIPbranchGetScore(set, var, min1, min2);
}

/** calls branching rules to branch on an LP solution; if no fractional variables exist, the result is SCIP_DIDNOTRUN;
 *  if the branch priority of an unfixed variable is larger than the maximal branch priority of the fractional
 *  variables, pseudo solution branching is applied on the unfixed variables with maximal branch priority
 */
SCIP_RETCODE SCIPbranchExecLP(
   BMS_BLKMEM*           blkmem,             /**< block memory for parameter settings */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SEPASTORE*       sepastore,          /**< separation storage */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             cutoffbound,        /**< global upper cutoff bound */
   SCIP_Bool             allowaddcons,       /**< should adding constraints be allowed to avoid a branching? */
   SCIP_RESULT*          result              /**< pointer to store the result of the branching (s. branch.h) */
   )
{
   int i;

   assert(branchcand != NULL);
   assert(result != NULL);

   *result = SCIP_DIDNOTRUN;

   /* calculate branching candidates */
   SCIP_CALL( branchcandCalcLPCands(branchcand, set, stat, lp) );
   assert(0 <= branchcand->npriolpcands && branchcand->npriolpcands <= branchcand->nlpcands);
   assert((branchcand->npriolpcands == 0) == (branchcand->nlpcands == 0));

   SCIPdebugMessage("branching on LP solution with %d fractional variables (%d of maximal priority)\n",
      branchcand->nlpcands, branchcand->npriolpcands);

   /* do nothing, if no fractional variables exist */
   if( branchcand->nlpcands == 0 )
      return SCIP_OKAY;

   /* if there is a non-fixed variable with higher priority than the maximal priority of the fractional candidates,
    * use pseudo solution branching instead
    */
   if( branchcand->pseudomaxpriority > branchcand->lpmaxpriority )
   {
      SCIP_CALL( SCIPbranchExecPseudo(blkmem, set, stat, tree, lp, branchcand, eventqueue, cutoffbound, allowaddcons,
            result) );
      assert(*result != SCIP_DIDNOTRUN);
      return SCIP_OKAY;
   }

   /* sort the branching rules by priority */
   SCIPsetSortBranchrules(set);

   /* try all branching rules until one succeeded to branch */
   for( i = 0; i < set->nbranchrules && *result == SCIP_DIDNOTRUN; ++i )
   {
      SCIP_CALL( SCIPbranchruleExecLPSol(set->branchrules[i], set, stat, tree, sepastore, cutoffbound, allowaddcons, 
            result) );
   }

   if( *result == SCIP_DIDNOTRUN )
   {
      SCIP_VAR* var;
      SCIP_Real factor;
      SCIP_Real bestfactor;
      int priority;
      int bestpriority;
      int bestcand;

      /* no branching method succeeded in choosing a branching: just branch on the first fractional variable with maximal
       * priority, and out of these on the one with maximal branch factor
       */
      bestcand = -1;
      bestpriority = INT_MIN;
      bestfactor = SCIP_REAL_MIN;
      for( i = 0; i < branchcand->nlpcands; ++i )
      {
         priority = SCIPvarGetBranchPriority(branchcand->lpcands[i]);
         factor = SCIPvarGetBranchFactor(branchcand->lpcands[i]);
         if( priority > bestpriority || (priority == bestpriority && factor > bestfactor) )
         {
            bestcand = i;
            bestpriority = priority;
            bestfactor = factor;
         }
      }
      assert(0 <= bestcand && bestcand < branchcand->nlpcands);

      var = branchcand->lpcands[bestcand];
      assert(SCIPvarGetType(var) != SCIP_VARTYPE_CONTINUOUS);
      assert(!SCIPsetIsEQ(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)));

      SCIP_CALL( SCIPtreeBranchVar(tree, blkmem, set, stat, lp, branchcand, eventqueue, var, SCIP_BRANCHDIR_AUTO) );

      *result = SCIP_BRANCHED;
   }

   return SCIP_OKAY;
}

/** calls branching rules to branch on a pseudo solution; if no unfixed variables exist, the result is SCIP_DIDNOTRUN */
SCIP_RETCODE SCIPbranchExecPseudo(
   BMS_BLKMEM*           blkmem,             /**< block memory for parameter settings */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             cutoffbound,        /**< global upper cutoff bound */
   SCIP_Bool             allowaddcons,       /**< should adding constraints be allowed to avoid a branching? */
   SCIP_RESULT*          result              /**< pointer to store the result of the branching (s. branch.h) */
   )
{
   int i;
   
   assert(branchcand != NULL);
   assert(result != NULL);

   SCIPdebugMessage("branching on pseudo solution with %d unfixed variables\n", branchcand->npseudocands);

   *result = SCIP_DIDNOTRUN;

   /* do nothing, if no unfixed variables exist */
   if( branchcand->npseudocands == 0 )
      return SCIP_OKAY;

   /* sort the branching rules by priority */
   SCIPsetSortBranchrules(set);

   /* try all branching rules until one succeeded to branch */
   for( i = 0; i < set->nbranchrules && *result == SCIP_DIDNOTRUN; ++i )
   {
      SCIP_CALL( SCIPbranchruleExecPseudoSol(set->branchrules[i], set, stat, tree, cutoffbound, allowaddcons, result) );
   }

   if( *result == SCIP_DIDNOTRUN )
   {
      SCIP_VAR* var;
      SCIP_Real factor;
      SCIP_Real bestfactor;
      int priority;
      int bestpriority;
      int bestcand;

      /* no branching method succeeded in choosing a branching: just branch on the first unfixed variable with maximal
       * priority, and out of these on the one with maximal branch factor
       */
      bestcand = -1;
      bestpriority = INT_MIN;
      bestfactor = SCIP_REAL_MIN;
      for( i = 0; i < branchcand->npseudocands; ++i )
      {
         priority = SCIPvarGetBranchPriority(branchcand->pseudocands[i]);
         factor = SCIPvarGetBranchFactor(branchcand->pseudocands[i]);
         if( priority > bestpriority || (priority == bestpriority && factor > bestfactor) )
         {
            bestcand = i;
            bestpriority = priority;
            bestfactor = factor;
         }
      }
      assert(0 <= bestcand && bestcand < branchcand->npseudocands);

      var = branchcand->pseudocands[bestcand];
      assert(SCIPvarGetType(var) != SCIP_VARTYPE_CONTINUOUS);
      assert(!SCIPsetIsEQ(set, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var)));

      SCIP_CALL( SCIPtreeBranchVar(tree, blkmem, set, stat, lp, branchcand, eventqueue, var, SCIP_BRANCHDIR_AUTO) );

      *result = SCIP_BRANCHED;
   }

   return SCIP_OKAY;
}

