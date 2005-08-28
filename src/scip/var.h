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
#pragma ident "@(#) $Id: var.h,v 1.102 2005/08/28 12:24:03 bzfpfend Exp $"

/**@file   var.h
 * @brief  internal methods for problem variables
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_VAR_H__
#define __SCIP_VAR_H__


#include "scip/def.h"
#include "blockmemshell/memory.h"
#include "scip/type_retcode.h"
#include "scip/type_set.h"
#include "scip/type_stat.h"
#include "scip/type_misc.h"
#include "scip/type_history.h"
#include "scip/type_event.h"
#include "scip/type_lp.h"
#include "scip/type_var.h"
#include "scip/type_prob.h"
#include "scip/type_primal.h"
#include "scip/type_tree.h"
#include "scip/type_branch.h"
#include "scip/type_cons.h"
#include "scip/pub_var.h"

#ifndef NDEBUG
#include "scip/struct_var.h"
#else
#include "scip/event.h"
#endif




/*
 * domain change methods
 */

/** applies single bound change */
extern
SCIP_RETCODE SCIPboundchgApply(
   SCIP_BOUNDCHG*        boundchg,           /**< bound change to apply */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   int                   depth,              /**< depth in the tree, where the bound change takes place */
   int                   pos,                /**< position of the bound change in its bound change array */
   SCIP_Bool*            cutoff              /**< pointer to store whether an infeasible bound change was detected */
   );

/** undoes single bound change */
extern
SCIP_RETCODE SCIPboundchgUndo(
   SCIP_BOUNDCHG*        boundchg,           /**< bound change to remove */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue          /**< event queue */
   );

/** frees domain change data */
extern
SCIP_RETCODE SCIPdomchgFree(
   SCIP_DOMCHG**         domchg,             /**< pointer to domain change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** converts a dynamic domain change data into a static one, using less memory than for a dynamic one */
extern
SCIP_RETCODE SCIPdomchgMakeStatic(
   SCIP_DOMCHG**         domchg,             /**< pointer to domain change data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** applies domain change */
extern
SCIP_RETCODE SCIPdomchgApply(
   SCIP_DOMCHG*          domchg,             /**< domain change to apply */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   int                   depth,              /**< depth in the tree, where the domain change takes place */
   SCIP_Bool*            cutoff              /**< pointer to store whether an infeasible domain change was detected */
   );

/** undoes domain change */
extern
SCIP_RETCODE SCIPdomchgUndo(
   SCIP_DOMCHG*          domchg,             /**< domain change to remove */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue          /**< event queue */
   );

/** adds bound change to domain changes */
extern
SCIP_RETCODE SCIPdomchgAddBoundchg(
   SCIP_DOMCHG**         domchg,             /**< pointer to domain change data structure */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< variable to change the bounds for */
   SCIP_Real             newbound,           /**< new value for bound */
   SCIP_BOUNDTYPE        boundtype,          /**< type of bound for var: lower or upper bound */
   SCIP_BOUNDCHGTYPE     boundchgtype,       /**< type of bound change: branching decision or inference */
   SCIP_Real             lpsolval,           /**< solval of variable in last LP on path to node, or SCIP_INVALID if unknown */
   SCIP_VAR*             infervar,           /**< variable that was changed (parent of var, or var itself) */
   SCIP_CONS*            infercons,          /**< constraint that deduced the bound change, or NULL */
   SCIP_PROP*            inferprop,          /**< propagator that deduced the bound change, or NULL */
   int                   inferinfo,          /**< user information for inference to help resolving the conflict */
   SCIP_BOUNDTYPE        inferboundtype      /**< type of bound for inference var: lower or upper bound */
   );

/** adds hole change to domain changes */
extern
SCIP_RETCODE SCIPdomchgAddHolechg(
   SCIP_DOMCHG**         domchg,             /**< pointer to domain change data structure */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_HOLELIST**       ptr,                /**< changed list pointer */
   SCIP_HOLELIST*        newlist,            /**< new value of list pointer */
   SCIP_HOLELIST*        oldlist             /**< old value of list pointer */
   );

#ifndef NDEBUG

/* In SCIPdebug mode, the following methods are implemented as function calls to ensure
 * type validity.
 */

/** returns the new value of the bound in the bound change data */
extern
SCIP_Real SCIPboundchgGetNewbound(
   SCIP_BOUNDCHG*        boundchg            /**< bound change data */
   );

/** returns the variable of the bound change in the bound change data */
extern
SCIP_VAR* SCIPboundchgGetVar(
   SCIP_BOUNDCHG*        boundchg            /**< bound change data */
   );

/** returns the bound change type of the bound change in the bound change data */
extern
SCIP_BOUNDCHGTYPE SCIPboundchgGetBoundchgtype(
   SCIP_BOUNDCHG*        boundchg            /**< bound change data */
   );

/** returns the bound type of the bound change in the bound change data */
extern
SCIP_BOUNDTYPE SCIPboundchgGetBoundtype(
   SCIP_BOUNDCHG*        boundchg            /**< bound change data */
   );

/** returns the number of bound changes in the domain change data */
extern
int SCIPdomchgGetNBoundchgs(
   SCIP_DOMCHG*          domchg              /**< domain change data */
   );

/** returns a particular bound change in the domain change data */
extern
SCIP_BOUNDCHG* SCIPdomchgGetBoundchg(
   SCIP_DOMCHG*          domchg,             /**< domain change data */
   int                   pos                 /**< position of the bound change in the domain change data */
   );

#else

/* In optimized mode, the methods are implemented as defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIPboundchgGetNewbound(boundchg)      ((boundchg)->newbound)
#define SCIPboundchgGetVar(boundchg)           ((boundchg)->var)
#define SCIPboundchgGetBoundchgtype(boundchg)  ((SCIP_BOUNDCHGTYPE)((boundchg)->boundchgtype))
#define SCIPboundchgGetBoundtype(boundchg)     ((SCIP_BOUNDTYPE)((boundchg)->boundtype))
#define SCIPdomchgGetNBoundchgs(domchg)        ((domchg) != NULL ? (domchg)->domchgbound.nboundchgs : 0)
#define SCIPdomchgGetBoundchg(domchg, pos)     (&(domchg)->domchgbound.boundchgs[pos])

#endif




/*
 * methods for variables 
 */

/** creates and captures an original problem variable; an integer variable with bounds
 *  zero and one is automatically converted into a binary variable
 */
extern
SCIP_RETCODE SCIPvarCreateOriginal(
   SCIP_VAR**            var,                /**< pointer to variable data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   const char*           name,               /**< name of variable */
   SCIP_Real             lb,                 /**< lower bound of variable */
   SCIP_Real             ub,                 /**< upper bound of variable */
   SCIP_Real             obj,                /**< objective function value */
   SCIP_VARTYPE          vartype,            /**< type of variable */
   SCIP_Bool             initial,            /**< should var's column be present in the initial root LP? */
   SCIP_Bool             removeable,         /**< is var's column removeable from the LP (due to aging or cleanup)? */
   SCIP_DECL_VARDELORIG  ((*vardelorig)),    /**< frees user data of original variable */
   SCIP_DECL_VARTRANS    ((*vartrans)),      /**< creates transformed user data by transforming original user data */
   SCIP_DECL_VARDELTRANS ((*vardeltrans)),   /**< frees user data of transformed variable */
   SCIP_VARDATA*         vardata             /**< user data for this specific variable */
   );

/** creates and captures a loose variable belonging to the transformed problem; an integer variable with bounds
 *  zero and one is automatically converted into a binary variable
 */
extern
SCIP_RETCODE SCIPvarCreateTransformed(
   SCIP_VAR**            var,                /**< pointer to variable data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   const char*           name,               /**< name of variable */
   SCIP_Real             lb,                 /**< lower bound of variable */
   SCIP_Real             ub,                 /**< upper bound of variable */
   SCIP_Real             obj,                /**< objective function value */
   SCIP_VARTYPE          vartype,            /**< type of variable */
   SCIP_Bool             initial,            /**< should var's column be present in the initial root LP? */
   SCIP_Bool             removeable,         /**< is var's column removeable from the LP (due to aging or cleanup)? */
   SCIP_DECL_VARDELORIG  ((*vardelorig)),    /**< frees user data of original variable */
   SCIP_DECL_VARTRANS    ((*vartrans)),      /**< creates transformed user data by transforming original user data */
   SCIP_DECL_VARDELTRANS ((*vardeltrans)),   /**< frees user data of transformed variable */
   SCIP_VARDATA*         vardata             /**< user data for this specific variable */
   );

/** increases usage counter of variable */
extern
void SCIPvarCapture(
   SCIP_VAR*             var                 /**< variable */
   );

/** decreases usage counter of variable, and frees memory if necessary */
extern
SCIP_RETCODE SCIPvarRelease(
   SCIP_VAR**            var,                /**< pointer to variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp                  /**< current LP data (may be NULL, if it's not a column variable) */
   );

/** initializes variable data structure for solving */
extern
void SCIPvarInitSolve(
   SCIP_VAR*             var                 /**< problem variable */
   );

/** gets and captures transformed variable of a given variable; if the variable is not yet transformed,
 *  a new transformed variable for this variable is created
 */
extern
SCIP_RETCODE SCIPvarTransform(
   SCIP_VAR*             origvar,            /**< original problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory of transformed problem */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_OBJSENSE         objsense,           /**< objective sense of original problem; transformed is always MINIMIZE */
   SCIP_VAR**            transvar            /**< pointer to store the transformed variable */
   );

/** gets corresponding transformed variable of an original or negated original variable */
extern
SCIP_RETCODE SCIPvarGetTransformed(
   SCIP_VAR*             origvar,            /**< original problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory of transformed problem */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_VAR**            transvar            /**< pointer to store the transformed variable, or NULL if not existing yet */
   );

/** converts transformed variable into column variable and creates LP column */
extern
SCIP_RETCODE SCIPvarColumn(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** converts column transformed variable back into loose variable, frees LP column */
extern
SCIP_RETCODE SCIPvarLoose(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** converts variable into fixed variable */
extern
SCIP_RETCODE SCIPvarFix(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             fixedval,           /**< value to fix variable at */
   SCIP_Bool*            infeasible,         /**< pointer to store whether the fixing is infeasible */
   SCIP_Bool*            fixed               /**< pointer to store whether the fixing was performed (variable was unfixed) */
   );

/** converts loose variable into aggregated variable */
extern
SCIP_RETCODE SCIPvarAggregate(
   SCIP_VAR*             var,                /**< loose problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_CLIQUETABLE*     cliquetable,        /**< clique table data structure */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_VAR*             aggvar,             /**< loose variable y in aggregation x = a*y + c */
   SCIP_Real             scalar,             /**< multiplier a in aggregation x = a*y + c */
   SCIP_Real             constant,           /**< constant shift c in aggregation x = a*y + c */
   SCIP_Bool*            infeasible,         /**< pointer to store whether the aggregation is infeasible */
   SCIP_Bool*            aggregated          /**< pointer to store whether the aggregation was successful */
   );

/** converts variable into multi-aggregated variable */
extern
SCIP_RETCODE SCIPvarMultiaggregate(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_CLIQUETABLE*     cliquetable,        /**< clique table data structure */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   int                   naggvars,           /**< number n of variables in aggregation x = a_1*y_1 + ... + a_n*y_n + c */
   SCIP_VAR**            aggvars,            /**< variables y_i in aggregation x = a_1*y_1 + ... + a_n*y_n + c */
   SCIP_Real*            scalars,            /**< multipliers a_i in aggregation x = a_1*y_1 + ... + a_n*y_n + c */
   SCIP_Real             constant,           /**< constant shift c in aggregation x = a_1*y_1 + ... + a_n*y_n + c */
   SCIP_Bool*            infeasible,         /**< pointer to store whether the aggregation is infeasible */
   SCIP_Bool*            aggregated          /**< pointer to store whether the aggregation was successful */
   );

/** gets negated variable x' = offset - x of problem variable x; the negated variable is created if not yet existing;
 *  the negation offset of binary variables is always 1, the offset of other variables is fixed to lb + ub when the
 *  negated variable is created
 */
extern
SCIP_RETCODE SCIPvarNegate(
   SCIP_VAR*             var,                /**< problem variable to negate */
   BMS_BLKMEM*           blkmem,             /**< block memory of transformed problem */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_VAR**            negvar              /**< pointer to store the negated variable */
   );

/** informs variable that its position in problem's vars array changed */
extern
void SCIPvarSetProbindex(
   SCIP_VAR*             var,                /**< problem variable */
   int                   probindex           /**< new problem index of variable */
   );

/** marks the variable to be deleted from the problem */
extern
void SCIPvarMarkDeleted(
   SCIP_VAR*             var                 /**< problem variable */
   );

/** modifies lock numbers for rounding */
extern
SCIP_RETCODE SCIPvarAddLocks(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   int                   addnlocksdown,      /**< increase in number of rounding down locks */
   int                   addnlocksup         /**< increase in number of rounding up locks */
   );

/** changes type of variable; cannot be called, if var belongs to a problem */
extern
SCIP_RETCODE SCIPvarChgType(
   SCIP_VAR*             var,                /**< problem variable to change */
   SCIP_VARTYPE          vartype             /**< new type of variable */
   );

/** changes objective value of variable */
extern
SCIP_RETCODE SCIPvarChgObj(
   SCIP_VAR*             var,                /**< variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newobj              /**< new objective value for variable */
   );

/** adds value to objective value of variable */
extern
SCIP_RETCODE SCIPvarAddObj(
   SCIP_VAR*             var,                /**< variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< transformed problem after presolve */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             addobj              /**< additional objective value for variable */
   );

/** changes objective value of variable in current dive */
extern
SCIP_RETCODE SCIPvarChgObjDive(
   SCIP_VAR*             var,                /**< problem variable to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             newobj              /**< new objective value for variable */
   );

/** adjust lower bound to integral value, if variable is integral */
extern
void SCIPvarAdjustLb(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real*            lb                  /**< pointer to lower bound to adjust */
   );

/** adjust upper bound to integral value, if variable is integral */
extern
void SCIPvarAdjustUb(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real*            ub                  /**< pointer to upper bound to adjust */
   );

/** adjust lower or upper bound to integral value, if variable is integral */
extern
void SCIPvarAdjustBd(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_BOUNDTYPE        boundtype,          /**< type of bound to adjust */
   SCIP_Real*            bd                  /**< pointer to bound to adjust */
   );

/** changes lower bound of original variable in original problem */
extern
SCIP_RETCODE SCIPvarChgLbOriginal(
   SCIP_VAR*             var,                /**< problem variable to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes upper bound of original variable in original problem */
extern
SCIP_RETCODE SCIPvarChgUbOriginal(
   SCIP_VAR*             var,                /**< problem variable to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes global lower bound of variable; if possible, adjusts bound to integral value;
 *  updates local lower bound if the global bound is tighter
 */
extern
SCIP_RETCODE SCIPvarChgLbGlobal(
   SCIP_VAR*             var,                /**< problem variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes global upper bound of variable; if possible, adjusts bound to integral value;
 *  updates local upper bound if the global bound is tighter
 */
extern
SCIP_RETCODE SCIPvarChgUbGlobal(
   SCIP_VAR*             var,                /**< problem variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes global bound of variable; if possible, adjusts bound to integral value;
 *  updates local bound if the global bound is tighter
 */
extern
SCIP_RETCODE SCIPvarChgBdGlobal(
   SCIP_VAR*             var,                /**< problem variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newbound,           /**< new bound for variable */
   SCIP_BOUNDTYPE        boundtype           /**< type of bound: lower or upper bound */
   );

/** changes current local lower bound of variable; if possible, adjusts bound to integral value; stores inference
 *  information in variable
 */
extern
SCIP_RETCODE SCIPvarChgLbLocal(
   SCIP_VAR*             var,                /**< problem variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes current local upper bound of variable; if possible, adjusts bound to integral value; stores inference
 *  information in variable
 */
extern
SCIP_RETCODE SCIPvarChgUbLocal(
   SCIP_VAR*             var,                /**< problem variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes current local bound of variable; if possible, adjusts bound to integral value; stores inference
 *  information in variable
 */
extern
SCIP_RETCODE SCIPvarChgBdLocal(
   SCIP_VAR*             var,                /**< problem variable to change */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Real             newbound,           /**< new bound for variable */
   SCIP_BOUNDTYPE        boundtype           /**< type of bound: lower or upper bound */
   );

/** changes lower bound of variable in current dive; if possible, adjusts bound to integral value */
extern
SCIP_RETCODE SCIPvarChgLbDive(
   SCIP_VAR*             var,                /**< problem variable to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** changes upper bound of variable in current dive; if possible, adjusts bound to integral value */
extern
SCIP_RETCODE SCIPvarChgUbDive(
   SCIP_VAR*             var,                /**< problem variable to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             newbound            /**< new bound for variable */
   );

/** adds a hole to the original variable's original domain */
extern
SCIP_RETCODE SCIPvarAddHoleOriginal(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             left,               /**< left bound of open interval in new hole */
   SCIP_Real             right               /**< right bound of open interval in new hole */
   );

/** adds a hole to the variable's global domain */
extern
SCIP_RETCODE SCIPvarAddHoleGlobal(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             left,               /**< left bound of open interval in new hole */
   SCIP_Real             right               /**< right bound of open interval in new hole */
   );

/** adds a hole to the variable's current local domain */
extern
SCIP_RETCODE SCIPvarAddHoleLocal(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             left,               /**< left bound of open interval in new hole */
   SCIP_Real             right               /**< right bound of open interval in new hole */
   );

/** resets the global and local bounds of original variable to their original values */
extern
SCIP_RETCODE SCIPvarResetBounds(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** informs variable x about a globally valid variable lower bound x >= b*z + d with integer variable z;
 *  if z is binary, the corresponding valid implication for z is also added;
 *  improves the global bounds of the variable and the vlb variable if possible
 */
extern
SCIP_RETCODE SCIPvarAddVlb(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_CLIQUETABLE*     cliquetable,        /**< clique table data structure */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_VAR*             vlbvar,             /**< variable z    in x >= b*z + d */
   SCIP_Real             vlbcoef,            /**< coefficient b in x >= b*z + d */
   SCIP_Real             vlbconstant,        /**< constant d    in x >= b*z + d */
   SCIP_Bool             transitive,         /**< should transitive closure of implication also be added? */
   SCIP_Bool*            infeasible,         /**< pointer to store whether an infeasibility was detected */
   int*                  nbdchgs             /**< pointer to store the number of performed bound changes, or NULL */
   );

/** informs variable x about a globally valid variable upper bound x <= b*z + d with integer variable z;
 *  if z is binary, the corresponding valid implication for z is also added;
 *  updates the global bounds of the variable and the vub variable correspondingly
 */
extern
SCIP_RETCODE SCIPvarAddVub(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_CLIQUETABLE*     cliquetable,        /**< clique table data structure */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_VAR*             vubvar,             /**< variable z    in x <= b*z + d */
   SCIP_Real             vubcoef,            /**< coefficient b in x <= b*z + d */
   SCIP_Real             vubconstant,        /**< constant d    in x <= b*z + d */
   SCIP_Bool             transitive,         /**< should transitive closure of implication also be added? */
   SCIP_Bool*            infeasible,         /**< pointer to store whether an infeasibility was detected */
   int*                  nbdchgs             /**< pointer to store the number of performed bound changes, or NULL */
   );

/** informs binary variable x about a globally valid implication:  x == 0 or x == 1  ==>  y <= b  or  y >= b;
 *  also adds the corresponding implication or variable bound to the implied variable;
 *  if the implication is conflicting, the variable is fixed to the opposite value;
 *  if the variable is already fixed to the given value, the implication is performed immediately;
 *  if the implication is redundant with respect to the variables' global bounds, it is ignored
 */
extern
SCIP_RETCODE SCIPvarAddImplic(
   SCIP_VAR*             var,                /**< problem variable  */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_CLIQUETABLE*     cliquetable,        /**< clique table data structure */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Bool             varfixing,          /**< FALSE if y should be added in implications for x == 0, TRUE for x == 1 */
   SCIP_VAR*             implvar,            /**< variable y in implication y <= b or y >= b */
   SCIP_BOUNDTYPE        impltype,           /**< type       of implication y <= b (SCIP_BOUNDTYPE_UPPER) or y >= b (SCIP_BOUNDTYPE_LOWER) */
   SCIP_Real             implbound,          /**< bound b    in implication y <= b or y >= b */
   SCIP_Bool             transitive,         /**< should transitive closure of implication also be added? */
   SCIP_Bool*            infeasible,         /**< pointer to store whether an infeasibility was detected */
   int*                  nbdchgs             /**< pointer to store the number of performed bound changes, or NULL */
   );

/** adds the variable to the given clique and updates the list of cliques the binary variable is member of;
 *  if the variable now appears twice in the clique with the same value, it is fixed to the opposite value;
 *  if the variable now appears twice in the clique with opposite values, all other variables are fixed to
 *  the opposite of the value they take in the clique
 */
extern
SCIP_RETCODE SCIPvarAddClique(
   SCIP_VAR*             var,                /**< problem variable  */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Bool             value,              /**< value of the variable in the clique */
   SCIP_CLIQUE*          clique,             /**< clique the variable should be added to */
   SCIP_Bool*            infeasible,         /**< pointer to store whether an infeasibility was detected */
   int*                  nbdchgs             /**< pointer to count the number of performed bound changes, or NULL */
   );

/** deletes the variable from the given clique and updates the list of cliques the binary variable is member of */
extern
SCIP_RETCODE SCIPvarDelClique(
   SCIP_VAR*             var,                /**< problem variable  */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_Bool             value,              /**< value of the variable in the clique */
   SCIP_CLIQUE*          clique              /**< clique the variable should be removed from */
   );

/** deletes the variable from the list of cliques the binary variable is member of, but does not change the clique
 *  itself
 */
extern
SCIP_RETCODE SCIPvarDelCliqueFromList(
   SCIP_VAR*             var,                /**< problem variable  */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_Bool             value,              /**< value of the variable in the clique */
   SCIP_CLIQUE*          clique              /**< clique that should be removed from the variable's clique list */
   );

/** sets the branch factor of the variable; this value can be used in the branching methods to scale the score
 *  values of the variables; higher factor leads to a higher probability that this variable is chosen for branching
 */
extern
void SCIPvarChgBranchFactor(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             branchfactor        /**< factor to weigh variable's branching score with */
   );

/** sets the branch priority of the variable; variables with higher branch priority are always preferred to variables
 *  with lower priority in selection of branching variable
 */
extern
void SCIPvarChgBranchPriority(
   SCIP_VAR*             var,                /**< problem variable */
   int                   branchpriority      /**< branching priority of the variable */
   );

/** sets the branch direction of the variable; variables with higher branch direction are always preferred to variables
 *  with lower direction in selection of branching variable
 */
extern
void SCIPvarChgBranchDirection(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_BRANCHDIR        branchdirection     /**< preferred branch direction of the variable (downwards, upwards, auto) */
   );

/** gets objective value of variable in current SCIP_LP; the value can be different from the bound stored in the variable's own
 *  data due to diving, that operate only on the LP without updating the variables
 */
extern
SCIP_Real SCIPvarGetObjLP(
   SCIP_VAR*             var                 /**< problem variable */
   );

/** gets lower bound of variable in current SCIP_LP; the bound can be different from the bound stored in the variable's own
 *  data due to diving or conflict analysis, that operate only on the LP without updating the variables
 */
extern
SCIP_Real SCIPvarGetLbLP(
   SCIP_VAR*             var                 /**< problem variable */
   );

/** gets upper bound of variable in current SCIP_LP; the bound can be different from the bound stored in the variable's own
 *  data due to diving or conflict analysis, that operate only on the LP without updating the variables
 */
extern
SCIP_Real SCIPvarGetUbLP(
   SCIP_VAR*             var                 /**< problem variable */
   );

/** remembers the current solution as root solution in the problem variables */
extern
void SCIPvarStoreRootSol(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_Bool             roothaslp           /**< is the root solution from LP? */
   );

/** resolves variable to columns and adds them with the coefficient to the row */
extern
SCIP_RETCODE SCIPvarAddToRow(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_ROW*             row,                /**< LP row */
   SCIP_Real             val                 /**< value of coefficient */
   );

/** updates the pseudo costs of the given variable and the global pseudo costs after a change of
 *  "solvaldelta" in the variable's solution value and resulting change of "objdelta" in the in the LP's objective value
 */
extern
SCIP_RETCODE SCIPvarUpdatePseudocost(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_Real             solvaldelta,        /**< difference of variable's new LP value - old LP value */
   SCIP_Real             objdelta,           /**< difference of new LP's objective value - old LP's objective value */
   SCIP_Real             weight              /**< weight in (0,1] of this update in pseudo cost sum */
   );

/** gets the variable's pseudo cost value for the given step size "solvaldelta" in the variable's LP solution value */
extern
SCIP_Real SCIPvarGetPseudocost(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_Real             solvaldelta         /**< difference of variable's new LP value - old LP value */
   );

/** gets the variable's pseudo cost value for the given step size "solvaldelta" in the variable's LP solution value,
 *  only using the pseudo cost information of the current run
 */
extern
SCIP_Real SCIPvarGetPseudocostCurrentRun(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_Real             solvaldelta         /**< difference of variable's new LP value - old LP value */
   );

/** gets the variable's (possible fractional) number of pseudo cost updates for the given direction */
extern
SCIP_Real SCIPvarGetPseudocostCount(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** gets the variable's (possible fractional) number of pseudo cost updates for the given direction,
 *  only using the pseudo cost information of the current run
 */
extern
SCIP_Real SCIPvarGetPseudocostCountCurrentRun(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** increases the number of branchings counter of the variable */
extern
SCIP_RETCODE SCIPvarIncNBranchings(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   int                   depth,              /**< depth at which the bound change took place */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** increases the number of inferences counter of the variable */
extern
SCIP_RETCODE SCIPvarIncNInferences(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** increases the number of cutoffs counter of the variable */
extern
SCIP_RETCODE SCIPvarIncNCutoffs(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** returns the average number of inferences found after branching on the variable in given direction */
extern
SCIP_Real SCIPvarGetAvgInferences(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** returns the average number of inferences found after branching on the variable in given direction
 *  in the current run
 */
extern
SCIP_Real SCIPvarGetAvgInferencesCurrentRun(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** returns the average number of cutoffs found after branching on the variable in given direction */
extern
SCIP_Real SCIPvarGetAvgCutoffs(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** returns the average number of cutoffs found after branching on the variable in given direction in the current run */
extern
SCIP_Real SCIPvarGetAvgCutoffsCurrentRun(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_BRANCHDIR        dir                 /**< branching direction (downwards, or upwards) */
   );

/** outputs variable information into file stream */
extern
void SCIPvarPrint(
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_SET*             set,                /**< global SCIP settings */
   FILE*                 file                /**< output file (or NULL for standard output) */
   );


#ifndef NDEBUG

/* In SCIPdebug mode, the following methods are implemented as function calls to ensure
 * type validity.
 */

/** includes event handler with given data in variable's event filter */
extern
SCIP_RETCODE SCIPvarCatchEvent(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTTYPE        eventtype,          /**< event type to catch */
   SCIP_EVENTHDLR*       eventhdlr,          /**< event handler to call for the event processing */
   SCIP_EVENTDATA*       eventdata,          /**< event data to pass to the event handler for the event processing */
   int*                  filterpos           /**< pointer to store position of event filter entry, or NULL */
   );

/** deletes event handler with given data from variable's event filter */
extern
SCIP_RETCODE SCIPvarDropEvent(
   SCIP_VAR*             var,                /**< problem variable */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTTYPE        eventtype,          /**< event type mask of dropped event */
   SCIP_EVENTHDLR*       eventhdlr,          /**< event handler to call for the event processing */
   SCIP_EVENTDATA*       eventdata,          /**< event data to pass to the event handler for the event processing */
   int                   filterpos           /**< position of event filter entry returned by SCIPvarCatchEvent(), or -1 */
   );

#else

/* In optimized mode, the methods are implemented as defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIPvarCatchEvent(var, blkmem, set, eventtype, eventhdlr, eventdata, filterpos) \
   SCIPeventfilterAdd(var->eventfilter, blkmem, set, eventtype, eventhdlr, eventdata, filterpos)
#define SCIPvarDropEvent(var, blkmem, set, eventtype, eventhdlr, eventdata, filterpos) \
   SCIPeventfilterDel(var->eventfilter, blkmem, set, eventtype, eventhdlr, eventdata, filterpos)

#endif




/*
 * Hash functions
 */

/** gets the key (i.e. the name) of the given variable */
extern
SCIP_DECL_HASHGETKEY(SCIPhashGetKeyVar);



#endif
