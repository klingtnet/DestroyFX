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
/* Generated on Sun Nov  7 20:43:54 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -real2hc 8 */

/*
 * This function contains 20 FP additions, 2 FP multiplications,
 * (or, 20 additions, 2 multiplications, 0 fused multiply/add),
 * 12 stack variables, and 16 memory accesses
 */
static const fftw_real K707106781 = FFTW_KONST(+0.707106781186547524400844362104849039284835938);

/*
 * Generator Id's : 
 * $Id: frc_8.c,v 1.5 2001-08-10 21:46:13 tom7 Exp $
 * $Id: frc_8.c,v 1.5 2001-08-10 21:46:13 tom7 Exp $
 * $Id: frc_8.c,v 1.5 2001-08-10 21:46:13 tom7 Exp $
 */

void fftw_real2hc_8(const fftw_real *input, fftw_real *real_output, fftw_real *imag_output, int istride, int real_ostride, int imag_ostride)
{
     fftw_real tmp3;
     fftw_real tmp7;
     fftw_real tmp13;
     fftw_real tmp19;
     fftw_real tmp6;
     fftw_real tmp16;
     fftw_real tmp10;
     fftw_real tmp18;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp1;
	  fftw_real tmp2;
	  fftw_real tmp11;
	  fftw_real tmp12;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp1 = input[0];
	  tmp2 = input[4 * istride];
	  tmp3 = tmp1 + tmp2;
	  tmp7 = tmp1 - tmp2;
	  tmp11 = input[7 * istride];
	  tmp12 = input[3 * istride];
	  tmp13 = tmp11 - tmp12;
	  tmp19 = tmp11 + tmp12;
     }
     {
	  fftw_real tmp4;
	  fftw_real tmp5;
	  fftw_real tmp8;
	  fftw_real tmp9;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp4 = input[2 * istride];
	  tmp5 = input[6 * istride];
	  tmp6 = tmp4 + tmp5;
	  tmp16 = tmp4 - tmp5;
	  tmp8 = input[istride];
	  tmp9 = input[5 * istride];
	  tmp10 = tmp8 - tmp9;
	  tmp18 = tmp8 + tmp9;
     }
     {
	  fftw_real tmp17;
	  fftw_real tmp20;
	  fftw_real tmp14;
	  fftw_real tmp15;
	  ASSERT_ALIGNED_DOUBLE;
	  real_output[2 * real_ostride] = tmp3 - tmp6;
	  tmp17 = tmp3 + tmp6;
	  tmp20 = tmp18 + tmp19;
	  real_output[4 * real_ostride] = tmp17 - tmp20;
	  real_output[0] = tmp17 + tmp20;
	  imag_output[2 * imag_ostride] = tmp19 - tmp18;
	  tmp14 = K707106781 * (tmp10 + tmp13);
	  real_output[3 * real_ostride] = tmp7 - tmp14;
	  real_output[real_ostride] = tmp7 + tmp14;
	  tmp15 = K707106781 * (tmp13 - tmp10);
	  imag_output[imag_ostride] = tmp15 - tmp16;
	  imag_output[3 * imag_ostride] = tmp16 + tmp15;
     }
}

fftw_codelet_desc fftw_real2hc_8_desc =
{
     "fftw_real2hc_8",
     (void (*)()) fftw_real2hc_8,
     8,
     FFTW_FORWARD,
     FFTW_REAL2HC,
     178,
     0,
     (const int *) 0,
};
