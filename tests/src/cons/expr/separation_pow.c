/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2013 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   separation_prod.c
 * @brief  tests separation of power expressions
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include "scip/cons_expr_pow.c"
#include "separation.h"

/* test estimateTangent */
Test(estimation, tangent, .description = "test computation of tangent")
{
   SCIP_Real exponent;
   SCIP_Real constant;
   SCIP_Real slope;
   SCIP_Real xref;
   SCIP_Bool success;

   SCIP_CALL( SCIPcreate(&scip) );

   for( exponent = -3.0; exponent <= 3.0; exponent += 0.5 )
   {
      if( exponent == 0.0 )
         continue;

      for( xref = -2.0; xref <= 2.0; xref += 1.0 )
      {
         /* skip negative reference points when exponent is fractional */
         if( xref < 0.0 && !EPSISINT(exponent, 0.0) )
            continue;

         /* skip zero reference point when exponent is negative */
         if( xref == 0.0 && exponent < 0.0 )
            continue;

         success = FALSE;
         constant = DBL_MAX;
         slope = DBL_MAX;

         estimateTangent(scip, exponent, xref, &constant, &slope, &success);

         /* x^p -> x0^p + p*x0^{p-1} (x-x0) */

         /* estimateTangent must fail iff xref is 0 and exponent < 1 (infinite gradient in reference point) */
         cr_assert(success != (xref == 0.0 && exponent < 1.0));

         if( success )
         {
            cr_assert(SCIPisEQ(scip, slope, exponent * pow(xref, exponent-1.0)));
            cr_assert(SCIPisEQ(scip, constant, pow(xref, exponent) - slope * xref));
         }
      }
   }

   SCIP_CALL( SCIPfree(&scip) );
}

/* test estimateSecant */
Test(estimation, secant, .description = "test computation of secant")
{
   SCIP_Real exponent;
   SCIP_Real constant;
   SCIP_Real slope;
   SCIP_Real xlb;
   SCIP_Real xub;
   SCIP_Bool success;

   SCIP_CALL( SCIPcreate(&scip) );

   for( exponent = -3.0; exponent <= 3.0; exponent += 0.5 )
   {
      if( exponent == 0.0 )
         continue;

      for( xlb = -2.0; xlb <= 2.0; xlb += 1.0 )
      {
         for( xub = xlb + 1.0; xub <= 3.0; xub += 1.0 )
         {
            /* skip negative lower bound when exponent is fractional */
            if( xlb < 0.0 && !EPSISINT(exponent, 0.0) )
               continue;

            success = FALSE;
            constant = DBL_MAX;
            slope = DBL_MAX;

            estimateSecant(scip, exponent, xlb, xub, &constant, &slope, &success);

            /* x^p -> xlb^p + (xub^p - xlb^p) / (xub - xlb) * (x - xlb) */

            /* estimateSecant must fail iff xlb or xub is 0 and exponent < 0 (pole at boundary) */
            cr_assert(success != ((xlb == 0.0 || xub == 0.0) && exponent < 0.0));

            if( success )
            {
               cr_assert(SCIPisEQ(scip, slope, (pow(xub, exponent) - pow(xlb, exponent)) / (xub - xlb)));
               cr_assert(SCIPisEQ(scip, constant, pow(xlb, exponent) - slope * xlb));
            }
         }
      }
   }

   SCIP_CALL( SCIPfree(&scip) );
}


Test(separation, convexsquare, .init = setup, .fini = teardown,
   .description = "test separation for a convex square expression"
   )
{
   SCIP_CONSEXPR_EXPR* expr;
   SCIP_ROW* cut;
   int i;

   SCIP_CALL( SCIPcreateConsExprExprPow(scip, conshdlr, &expr, xexpr, 2.0) );

   /* add the auxiliary variable to the expression; variable will be released in CONSEXITSOL */
   SCIP_CALL( SCIPcaptureVar(scip, auxvar) );
   SCIP_CALL( SCIPaddVarLocks(scip, auxvar, 1, 1) );
   expr->auxvar = auxvar;

   /*
    * compute cut for w = x^2 with x* = 1.0 and w* = -5.0
    * this should result in an gradient cut at x*
    */
   SCIP_CALL( SCIPsetSolVal(scip, sol, x, 1.0) );
   SCIP_CALL( SCIPsetSolVal(scip, sol, auxvar, -5.0) );

   cut = NULL;
   SCIP_CALL( separatePointPow(scip,  conshdlr, expr, sol, 0.0, FALSE, &cut) );
   cr_assert(cut != NULL);

   cr_assert_eq(SCIProwGetNNonz(cut), 2);
   cr_assert_eq(SCIProwGetLhs(cut), -SCIPinfinity(scip));
   cr_assert_eq(SCIProwGetRhs(cut), 1.0);

   for( i = 0; i < SCIProwGetNNonz(cut); ++i )
   {
      SCIP_VAR* var;
      SCIP_Real coef;

      var = SCIPcolGetVar(SCIProwGetCols(cut)[i]);
      coef = SCIProwGetVals(cut)[i];

      if( var == SCIPvarGetTransVar(x) )
         cr_assert_eq(coef, 2.0);
      else if( var == SCIPvarGetTransVar(auxvar) )
         cr_assert_eq(coef, -1.0);
      else
         cr_assert(FALSE, "found an unknown variable");
   }
   SCIP_CALL( SCIPreleaseRow(scip, &cut) );

   /*
    * compute cut for w = x^2 with x* = 1.0 and w* = 5.0
    * this should result in a secant cut
    */
   SCIP_CALL( SCIPsetSolVal(scip, sol, x, 1.0) );
   SCIP_CALL( SCIPsetSolVal(scip, sol, auxvar, 5.0) );

   cut = NULL;
   SCIP_CALL( separatePointPow(scip,  conshdlr, expr, sol, 0.0, TRUE, &cut) );
   cr_assert(cut != NULL);
   cr_assert_eq(SCIProwGetNNonz(cut), 2);
   cr_assert_eq(SCIProwGetLhs(cut), -5.0);
   cr_assert_eq(SCIProwGetRhs(cut), SCIPinfinity(scip));

   for( i = 0; i < SCIProwGetNNonz(cut); ++i )
   {
      SCIP_VAR* var;
      SCIP_Real coef;

      var = SCIPcolGetVar(SCIProwGetCols(cut)[i]);
      coef = SCIProwGetVals(cut)[i];

      if( var == SCIPvarGetTransVar(x) )
         cr_assert_eq(coef, 4.0);
      else if( var == SCIPvarGetTransVar(auxvar) )
         cr_assert_eq(coef, -1.0);
      else
         cr_assert(FALSE, "found an unknown variable");
   }
   SCIP_CALL( SCIPreleaseRow(scip, &cut) );

   /* release expression */
   SCIP_CALL( SCIPreleaseConsExprExpr(scip, &expr) );
}
