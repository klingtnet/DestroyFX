/*
 * Copyright (c) 1997-1999 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Nov  7 20:44:22 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2real 11 */

/*
 * This function contains 60 FP additions, 51 FP multiplications,
 * (or, 59 additions, 50 multiplications, 1 fused multiply/add),
 * 21 stack variables, and 22 memory accesses
 */
static const fftw_real K2_000000000 = FFTW_KONST(+2.000000000000000000000000000000000000000000000);
static const fftw_real K1_918985947 = FFTW_KONST(+1.918985947228994779780736114132655398124909697);
static const fftw_real K1_309721467 = FFTW_KONST(+1.309721467890570128113850144932587106367582399);
static const fftw_real K284629676 = FFTW_KONST(+0.284629676546570280887585337232739337582102722);
static const fftw_real K830830026 = FFTW_KONST(+0.830830026003772851058548298459246407048009821);
static const fftw_real K1_682507065 = FFTW_KONST(+1.682507065662362337723623297838735435026584997);
static const fftw_real K563465113 = FFTW_KONST(+0.563465113682859395422835830693233798071555798);
static const fftw_real K1_511499148 = FFTW_KONST(+1.511499148708516567548071687944688840359434890);
static const fftw_real K1_979642883 = FFTW_KONST(+1.979642883761865464752184075553437574753038744);
static const fftw_real K1_819263990 = FFTW_KONST(+1.819263990709036742823430766158056920120482102);
static const fftw_real K1_081281634 = FFTW_KONST(+1.081281634911195164215271908637383390863541216);

/*
 * Generator Id's : 
 * $Id: fcr_11.c,v 1.3 2001-08-10 21:46:13 tom7 Exp $
 * $Id: fcr_11.c,v 1.3 2001-08-10 21:46:13 tom7 Exp $
 * $Id: fcr_11.c,v 1.3 2001-08-10 21:46:13 tom7 Exp $
 */

void fftw_hc2real_11(const fftw_real *real_input, const fftw_real *imag_input, fftw_real *output, int real_istride, int imag_istride, int ostride)
{
     fftw_real tmp13;
     fftw_real tmp21;
     fftw_real tmp15;
     fftw_real tmp17;
     fftw_real tmp19;
     fftw_real tmp1;
     fftw_real tmp2;
     fftw_real tmp6;
     fftw_real tmp5;
     fftw_real tmp4;
     fftw_real tmp3;
     fftw_real tmp7;
     fftw_real tmp20;
     fftw_real tmp14;
     fftw_real tmp16;
     fftw_real tmp18;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp8;
	  fftw_real tmp12;
	  fftw_real tmp9;
	  fftw_real tmp10;
	  fftw_real tmp11;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp8 = imag_input[2 * imag_istride];
	  tmp12 = imag_input[imag_istride];
	  tmp9 = imag_input[4 * imag_istride];
	  tmp10 = imag_input[5 * imag_istride];
	  tmp11 = imag_input[3 * imag_istride];
	  tmp13 = (K1_081281634 * tmp8) + (K1_819263990 * tmp9) - (K1_979642883 * tmp10) - (K1_511499148 * tmp11) - (K563465113 * tmp12);
	  tmp21 = (K1_979642883 * tmp8) + (K1_819263990 * tmp10) - (K563465113 * tmp9) - (K1_081281634 * tmp11) - (K1_511499148 * tmp12);
	  tmp15 = (K563465113 * tmp8) + (K1_819263990 * tmp11) - (K1_511499148 * tmp10) - (K1_081281634 * tmp9) - (K1_979642883 * tmp12);
	  tmp17 = (K1_081281634 * tmp12) + (K1_819263990 * tmp8) + (K1_979642883 * tmp11) + (K1_511499148 * tmp9) + (K563465113 * tmp10);
	  tmp19 = (K563465113 * tmp11) + (K1_979642883 * tmp9) + (K1_081281634 * tmp10) - (K1_511499148 * tmp8) - (K1_819263990 * tmp12);
     }
     tmp1 = real_input[0];
     tmp2 = real_input[real_istride];
     tmp6 = real_input[5 * real_istride];
     tmp5 = real_input[4 * real_istride];
     tmp4 = real_input[3 * real_istride];
     tmp3 = real_input[2 * real_istride];
     tmp7 = tmp1 + (K1_682507065 * tmp3) + (K830830026 * tmp5) - (K284629676 * tmp6) - (K1_309721467 * tmp4) - (K1_918985947 * tmp2);
     tmp20 = tmp1 + (K1_682507065 * tmp4) + (K830830026 * tmp6) - (K1_918985947 * tmp5) - (K284629676 * tmp3) - (K1_309721467 * tmp2);
     tmp14 = tmp1 + (K830830026 * tmp4) + (K1_682507065 * tmp5) - (K1_309721467 * tmp6) - (K1_918985947 * tmp3) - (K284629676 * tmp2);
     tmp16 = tmp1 + (K1_682507065 * tmp2) + (K830830026 * tmp3) - (K1_918985947 * tmp6) - (K1_309721467 * tmp5) - (K284629676 * tmp4);
     tmp18 = tmp1 + (K830830026 * tmp2) + (K1_682507065 * tmp6) - (K284629676 * tmp5) - (K1_918985947 * tmp4) - (K1_309721467 * tmp3);
     output[6 * ostride] = tmp7 - tmp13;
     output[5 * ostride] = tmp7 + tmp13;
     output[7 * ostride] = tmp20 - tmp21;
     output[4 * ostride] = tmp20 + tmp21;
     output[0] = tmp1 + (K2_000000000 * (tmp2 + tmp3 + tmp4 + tmp5 + tmp6));
     output[2 * ostride] = tmp18 + tmp19;
     output[9 * ostride] = tmp18 - tmp19;
     output[10 * ostride] = tmp16 + tmp17;
     output[ostride] = tmp16 - tmp17;
     output[8 * ostride] = tmp14 - tmp15;
     output[3 * ostride] = tmp14 + tmp15;
}

fftw_codelet_desc fftw_hc2real_11_desc =
{
     "fftw_hc2real_11",
     (void (*)()) fftw_hc2real_11,
     11,
     FFTW_BACKWARD,
     FFTW_HC2REAL,
     257,
     0,
     (const int *) 0,
};
