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
/* Generated on Sun Nov  7 20:43:55 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -real2hc 12 */

/*
 * This function contains 38 FP additions, 8 FP multiplications,
 * (or, 34 additions, 4 multiplications, 4 fused multiply/add),
 * 18 stack variables, and 24 memory accesses
 */
static const fftw_real K866025403 = FFTW_KONST(+0.866025403784438646763723170752936183471402627);
static const fftw_real K500000000 = FFTW_KONST(+0.500000000000000000000000000000000000000000000);

/*
 * Generator Id's : 
 * $Id: frc_12.c,v 1.3 2001-08-10 12:51:40 tom7 Exp $
 * $Id: frc_12.c,v 1.3 2001-08-10 12:51:40 tom7 Exp $
 * $Id: frc_12.c,v 1.3 2001-08-10 12:51:40 tom7 Exp $
 */

void fftw_real2hc_12(const fftw_real *input, fftw_real *real_output, fftw_real *imag_output, int istride, int real_ostride, int imag_ostride)
{
     fftw_real tmp5;
     fftw_real tmp25;
     fftw_real tmp11;
     fftw_real tmp23;
     fftw_real tmp30;
     fftw_real tmp35;
     fftw_real tmp10;
     fftw_real tmp26;
     fftw_real tmp12;
     fftw_real tmp18;
     fftw_real tmp29;
     fftw_real tmp34;
     fftw_real tmp31;
     fftw_real tmp32;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp1;
	  fftw_real tmp2;
	  fftw_real tmp3;
	  fftw_real tmp4;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp1 = input[0];
	  tmp2 = input[4 * istride];
	  tmp3 = input[8 * istride];
	  tmp4 = tmp2 + tmp3;
	  tmp5 = tmp1 + tmp4;
	  tmp25 = tmp1 - (K500000000 * tmp4);
	  tmp11 = tmp3 - tmp2;
     }
     {
	  fftw_real tmp19;
	  fftw_real tmp20;
	  fftw_real tmp21;
	  fftw_real tmp22;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp19 = input[9 * istride];
	  tmp20 = input[istride];
	  tmp21 = input[5 * istride];
	  tmp22 = tmp20 + tmp21;
	  tmp23 = tmp19 - (K500000000 * tmp22);
	  tmp30 = tmp19 + tmp22;
	  tmp35 = tmp21 - tmp20;
     }
     {
	  fftw_real tmp6;
	  fftw_real tmp7;
	  fftw_real tmp8;
	  fftw_real tmp9;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp6 = input[6 * istride];
	  tmp7 = input[10 * istride];
	  tmp8 = input[2 * istride];
	  tmp9 = tmp7 + tmp8;
	  tmp10 = tmp6 + tmp9;
	  tmp26 = tmp6 - (K500000000 * tmp9);
	  tmp12 = tmp8 - tmp7;
     }
     {
	  fftw_real tmp14;
	  fftw_real tmp15;
	  fftw_real tmp16;
	  fftw_real tmp17;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp14 = input[3 * istride];
	  tmp15 = input[7 * istride];
	  tmp16 = input[11 * istride];
	  tmp17 = tmp15 + tmp16;
	  tmp18 = tmp14 - (K500000000 * tmp17);
	  tmp29 = tmp14 + tmp17;
	  tmp34 = tmp16 - tmp15;
     }
     real_output[3 * real_ostride] = tmp5 - tmp10;
     imag_output[3 * imag_ostride] = tmp29 - tmp30;
     tmp31 = tmp5 + tmp10;
     tmp32 = tmp29 + tmp30;
     real_output[6 * real_ostride] = tmp31 - tmp32;
     real_output[0] = tmp31 + tmp32;
     {
	  fftw_real tmp37;
	  fftw_real tmp38;
	  fftw_real tmp33;
	  fftw_real tmp36;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp37 = tmp34 + tmp35;
	  tmp38 = tmp11 + tmp12;
	  imag_output[2 * imag_ostride] = K866025403 * (tmp37 - tmp38);
	  imag_output[4 * imag_ostride] = K866025403 * (tmp38 + tmp37);
	  tmp33 = tmp25 - tmp26;
	  tmp36 = K866025403 * (tmp34 - tmp35);
	  real_output[5 * real_ostride] = tmp33 - tmp36;
	  real_output[real_ostride] = tmp33 + tmp36;
     }
     {
	  fftw_real tmp27;
	  fftw_real tmp28;
	  fftw_real tmp13;
	  fftw_real tmp24;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp27 = tmp25 + tmp26;
	  tmp28 = tmp18 + tmp23;
	  real_output[2 * real_ostride] = tmp27 - tmp28;
	  real_output[4 * real_ostride] = tmp27 + tmp28;
	  tmp13 = K866025403 * (tmp11 - tmp12);
	  tmp24 = tmp18 - tmp23;
	  imag_output[imag_ostride] = tmp13 - tmp24;
	  imag_output[5 * imag_ostride] = -(tmp13 + tmp24);
     }
}

fftw_codelet_desc fftw_real2hc_12_desc =
{
     "fftw_real2hc_12",
     (void (*)()) fftw_real2hc_12,
     12,
     FFTW_FORWARD,
     FFTW_REAL2HC,
     266,
     0,
     (const int *) 0,
};
