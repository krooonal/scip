/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2018 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not visit scip.zib.de.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   cons_expr_log.c
 * @brief  logarithm expression handler
 * @author Stefan Vigerske
 * @author Benjamin Mueller
 *
 * @todo initsepaLog
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <string.h>

#include "scip/cons_expr_value.h"
#include "scip/cons_expr_log.h"

#define EXPRHDLR_NAME         "log"
#define EXPRHDLR_DESC         "logarithmic expression"
#define EXPRHDLR_PRECEDENCE  80000
#define EXPRHDLR_HASHKEY     SCIPcalcFibHash(16273.0)

/*
 * Data structures
 */


/*
 * Local methods
 */


/*
 * Callback methods of expression handler
 */

/** simplifies a log expression.
 * Evaluates the logarithm function when its child is a value expression
 * TODO: split products ?
 * TODO: log(exp(*)) = *
 */
static
SCIP_DECL_CONSEXPR_EXPRSIMPLIFY(simplifyLog)
{  /*lint --e{715}*/
   SCIP_CONSEXPR_EXPR* child;

   assert(scip != NULL);
   assert(expr != NULL);
   assert(simplifiedexpr != NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);

   child = SCIPgetConsExprExprChildren(expr)[0];
   assert(child != NULL);

   /* check for value expression */
   if( SCIPgetConsExprExprHdlr(child) == SCIPgetConsExprExprHdlrValue(conshdlr) )
   {
      /* TODO how to handle a non-positive value? */
      assert(SCIPgetConsExprExprValueValue(child) > 0.0);

      SCIP_CALL( SCIPcreateConsExprExprValue(scip, conshdlr, simplifiedexpr, log(SCIPgetConsExprExprValueValue(child))) );
   }
   else
   {
      *simplifiedexpr = expr;

      /* we have to capture it, since it must simulate a "normal" simplified call in which a new expression is created */
      SCIPcaptureConsExprExpr(*simplifiedexpr);
   }

   return SCIP_OKAY;
}

static
SCIP_DECL_CONSEXPR_EXPRCOPYHDLR(copyhdlrLog)
{  /*lint --e{715}*/
   SCIP_CALL( SCIPincludeConsExprExprHdlrLog(scip, consexprhdlr) );
   *valid = TRUE;

   return SCIP_OKAY;
}

static
SCIP_DECL_CONSEXPR_EXPRCOPYDATA(copydataLog)
{  /*lint --e{715}*/
   assert(targetexprdata != NULL);
   assert(sourceexpr != NULL);
   assert(SCIPgetConsExprExprData(sourceexpr) == NULL);

   *targetexprdata = NULL;

   return SCIP_OKAY;
}

static
SCIP_DECL_CONSEXPR_EXPRFREEDATA(freedataLog)
{  /*lint --e{715}*/
   assert(expr != NULL);

   SCIPsetConsExprExprData(expr, NULL);

   return SCIP_OKAY;
}

static
SCIP_DECL_CONSEXPR_EXPRPARSE(parseLog)
{  /*lint --e{715}*/
   SCIP_CONSEXPR_EXPR* childexpr;

   assert(expr != NULL);

   /* parse child expression from remaining string */
   SCIP_CALL( SCIPparseConsExprExpr(scip, consexprhdlr, string, endstring, &childexpr) );
   assert(childexpr != NULL);

   /* create logarithmic expression */
   SCIP_CALL( SCIPcreateConsExprExprLog(scip, consexprhdlr, expr, childexpr) );
   assert(*expr != NULL);

   /* release child expression since it has been captured by the logarithmic expression */
   SCIP_CALL( SCIPreleaseConsExprExpr(scip, &childexpr) );

   *success = TRUE;

   return SCIP_OKAY;
}

/** expression point evaluation callback */
static
SCIP_DECL_CONSEXPR_EXPREVAL(evalLog)
{  /*lint --e{715}*/
   assert(expr != NULL);
   assert(SCIPgetConsExprExprData(expr) == NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);
   assert(SCIPgetConsExprExprValue(SCIPgetConsExprExprChildren(expr)[0]) != SCIP_INVALID); /*lint !e777*/

   if( SCIPgetConsExprExprValue(SCIPgetConsExprExprChildren(expr)[0]) <= 0.0 )
   {
      SCIPdebugMsg(scip, "invalid evaluation of logarithmic expression\n");
      *val = SCIP_INVALID;
   }
   else
   {
      *val = log(SCIPgetConsExprExprValue(SCIPgetConsExprExprChildren(expr)[0]));
   }

   return SCIP_OKAY;
}

/** expression derivative evaluation callback */
static
SCIP_DECL_CONSEXPR_EXPRBWDIFF(bwdiffLog)
{  /*lint --e{715}*/
   SCIP_CONSEXPR_EXPR* child;

   assert(expr != NULL);
   assert(childidx == 0);
   assert(SCIPgetConsExprExprValue(expr) != SCIP_INVALID); /*lint !e777*/

   child = SCIPgetConsExprExprChildren(expr)[0];
   assert(child != NULL);
   assert(strcmp(SCIPgetConsExprExprHdlrName(SCIPgetConsExprExprHdlr(child)), "val") != 0);
   assert(SCIPgetConsExprExprValue(child) > 0.0);

   *val = 1.0 / SCIPgetConsExprExprValue(child);

   return SCIP_OKAY;
}

/** expression interval evaluation callback */
static
SCIP_DECL_CONSEXPR_EXPRINTEVAL(intevalLog)
{  /*lint --e{715}*/
   SCIP_INTERVAL childinterval;

   assert(expr != NULL);
   assert(SCIPgetConsExprExprData(expr) == NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);

   childinterval = SCIPgetConsExprExprActivity(scip, SCIPgetConsExprExprChildren(expr)[0]);
   assert(!SCIPintervalIsEmpty(SCIP_INTERVAL_INFINITY, childinterval));

   SCIPintervalLog(SCIP_INTERVAL_INFINITY, interval, childinterval);

   return SCIP_OKAY;
}

/** expression estimation callback */
static
SCIP_DECL_CONSEXPR_EXPRESTIMATE(estimateLog)
{  /*lint --e{715}*/
   SCIP_CONSEXPR_EXPR* child;
   SCIP_VAR* childvar;

   assert(scip != NULL);
   assert(conshdlr != NULL);
   assert(strcmp(SCIPconshdlrGetName(conshdlr), "expr") == 0);
   assert(expr != NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);
   assert(strcmp(SCIPgetConsExprExprHdlrName(SCIPgetConsExprExprHdlr(expr)), EXPRHDLR_NAME) == 0);
   assert(coefs != NULL);
   assert(constant != NULL);
   assert(islocal != NULL);
   assert(success != NULL);

   /* get expression data */
   child = SCIPgetConsExprExprChildren(expr)[0];
   assert(child != NULL);
   childvar = SCIPgetConsExprExprAuxVar(child);
   assert(childvar != NULL);

   *coefs = 0.0;
   *constant = 0.0;
   *success = TRUE;

   if( overestimate )
   {
      SCIP_Real refpoint;

      /* get reference point */
      refpoint = SCIPgetSolVal(scip, sol, childvar);
      if( SCIPisLE(scip, refpoint, 0.0) )
      {
         *success = FALSE;
         return SCIP_OKAY;
      }

      SCIPaddLogLinearization(scip, refpoint, SCIPvarIsIntegral(childvar), coefs, constant, success);
      *islocal = FALSE; /* linearization are globally valid */
   }
   else
   {
      SCIP_Real lb;
      SCIP_Real ub;

      lb = SCIPvarGetLbLocal(childvar);
      ub = SCIPvarGetUbLocal(childvar);

      SCIPaddLogSecant(scip, lb, ub, coefs, constant, success);
      *islocal = TRUE; /* secants are only valid locally */
   }

   return SCIP_OKAY;
}

/** expression reverse propagation callback */
static
SCIP_DECL_CONSEXPR_EXPRREVERSEPROP(reversepropLog)
{  /*lint --e{715}*/
   SCIP_INTERVAL childbound;

   assert(scip != NULL);
   assert(expr != NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);
   assert(nreductions != NULL);

   *nreductions = 0;

   /* f = log(c0) -> c0 = exp(f) */
   SCIPintervalExp(SCIP_INTERVAL_INFINITY, &childbound, SCIPgetConsExprExprActivity(scip, expr));

   /* try to tighten the bounds of the child node */
   SCIP_CALL( SCIPtightenConsExprExprInterval(scip, SCIPgetConsExprExprChildren(expr)[0], childbound, force, reversepropqueue,
         infeasible, nreductions) );

   return SCIP_OKAY;
}

/** expression hash callback */
static
SCIP_DECL_CONSEXPR_EXPRHASH(hashLog)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(expr != NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);
   assert(hashkey != NULL);
   assert(childrenhashes != NULL);

   *hashkey = EXPRHDLR_HASHKEY;
   *hashkey ^= childrenhashes[0];

   return SCIP_OKAY;
}

/** expression curvature detection callback */
static
SCIP_DECL_CONSEXPR_EXPRCURVATURE(curvatureLog)
{  /*lint --e{715}*/
   SCIP_CONSEXPR_EXPR* child;

   assert(scip != NULL);
   assert(expr != NULL);
   assert(curvature != NULL);
   assert(SCIPgetConsExprExprNChildren(expr) == 1);

   child = SCIPgetConsExprExprChildren(expr)[0];
   assert(child != NULL);

   /* expression is convex if child is concave */
   if( (int)(SCIPgetConsExprExprCurvature(child) & SCIP_EXPRCURV_CONCAVE) != 0 )
      *curvature = SCIP_EXPRCURV_CONCAVE;
   else
      *curvature = SCIP_EXPRCURV_UNKNOWN;

   return SCIP_OKAY;
}

/** expression monotonicity detection callback */
static
SCIP_DECL_CONSEXPR_EXPRMONOTONICITY(monotonicityLog)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(expr != NULL);
   assert(result != NULL);
   assert(childidx == 0);

   *result = SCIP_MONOTONE_INC;

   return SCIP_OKAY;
}

/** creates the handler for logarithmic expression and includes it into the expression constraint handler */
SCIP_RETCODE SCIPincludeConsExprExprHdlrLog(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONSHDLR*        consexprhdlr        /**< expression constraint handler */
   )
{
   SCIP_CONSEXPR_EXPRHDLR* exprhdlr;

   SCIP_CALL( SCIPincludeConsExprExprHdlrBasic(scip, consexprhdlr, &exprhdlr, EXPRHDLR_NAME, EXPRHDLR_DESC,
         EXPRHDLR_PRECEDENCE, evalLog, NULL) );
   assert(exprhdlr != NULL);

   SCIP_CALL( SCIPsetConsExprExprHdlrCopyFreeHdlr(scip, consexprhdlr, exprhdlr, copyhdlrLog, NULL) );
   SCIP_CALL( SCIPsetConsExprExprHdlrCopyFreeData(scip, consexprhdlr, exprhdlr, copydataLog, freedataLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrSimplify(scip, consexprhdlr, exprhdlr, simplifyLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrParse(scip, consexprhdlr, exprhdlr, parseLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrIntEval(scip, consexprhdlr, exprhdlr, intevalLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrSepa(scip, consexprhdlr, exprhdlr, NULL, NULL, NULL, estimateLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrReverseProp(scip, consexprhdlr, exprhdlr, reversepropLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrHash(scip, consexprhdlr, exprhdlr, hashLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrBwdiff(scip, consexprhdlr, exprhdlr, bwdiffLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrCurvature(scip, consexprhdlr, exprhdlr, curvatureLog) );
   SCIP_CALL( SCIPsetConsExprExprHdlrMonotonicity(scip, consexprhdlr, exprhdlr, monotonicityLog) );

   return SCIP_OKAY;
}

/** creates a logarithmic expression */
SCIP_RETCODE SCIPcreateConsExprExprLog(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONSHDLR*        consexprhdlr,       /**< expression constraint handler */
   SCIP_CONSEXPR_EXPR**  expr,               /**< pointer where to store expression */
   SCIP_CONSEXPR_EXPR*   child               /**< single child */
   )
{
   assert(expr != NULL);
   assert(child != NULL);
   assert(SCIPfindConsExprExprHdlr(consexprhdlr, EXPRHDLR_NAME) != NULL);

   SCIP_CALL( SCIPcreateConsExprExpr(scip, expr, SCIPfindConsExprExprHdlr(consexprhdlr, EXPRHDLR_NAME), NULL, 1, &child) );

   return SCIP_OKAY;
}
