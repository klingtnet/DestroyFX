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
/* Generated on Sun Nov  7 20:43:51 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -notwiddle 16 */

/*
 * This function contains 144 FP additions, 24 FP multiplications,
 * (or, 136 additions, 16 multiplications, 8 fused multiply/add),
 * 46 stack variables, and 64 memory accesses
 */
static const fftw_real K923879532 = FFTW_KONST(+0.923879532511286756128183189396788286822416626);
static const fftw_real K382683432 = FFTW_KONST(+0.382683432365089771728459984030398866761344562);
static const fftw_real K707106781 = FFTW_KONST(+0.707106781186547524400844362104849039284835938);

/*
 * Generator Id's : 
 * $Id: fn_16.c,v 1.3 2001-08-10 21:31:55 tom7 Exp $
 * $Id: fn_16.c,v 1.3 2001-08-10 21:31:55 tom7 Exp $
 * $Id: fn_16.c,v 1.3 2001-08-10 21:31:55 tom7 Exp $
 */

void fftw_no_twiddle_16(const fftw_complex *input, fftw_complex *output, int istride, int ostride)
{
     fftw_real tmp7;
     fftw_real tmp115;
     fftw_real tmp38;
     fftw_real tmp129;
     fftw_real tmp49;
     fftw_real tmp95;
     fftw_real tmp83;
     fftw_real tmp105;
     fftw_real tmp29;
     fftw_real tmp123;
     fftw_real tmp73;
     fftw_real tmp101;
     fftw_real tmp78;
     fftw_real tmp102;
     fftw_real tmp126;
     fftw_real tmp141;
     fftw_real tmp14;
     fftw_real tmp130;
     fftw_real tmp45;
     fftw_real tmp116;
     fftw_real tmp52;
     fftw_real tmp85;
     fftw_real tmp55;
     fftw_real tmp84;
     fftw_real tmp22;
     fftw_real tmp118;
     fftw_real tmp62;
     fftw_real tmp98;
     fftw_real tmp67;
     fftw_real tmp99;
     fftw_real tmp121;
     fftw_real tmp140;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp3;
	  fftw_real tmp47;
	  fftw_real tmp34;
	  fftw_real tmp82;
	  fftw_real tmp6;
	  fftw_real tmp81;
	  fftw_real tmp37;
	  fftw_real tmp48;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp1;
	       fftw_real tmp2;
	       fftw_real tmp32;
	       fftw_real tmp33;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp1 = c_re(input[0]);
	       tmp2 = c_re(input[8 * istride]);
	       tmp3 = tmp1 + tmp2;
	       tmp47 = tmp1 - tmp2;
	       tmp32 = c_im(input[0]);
	       tmp33 = c_im(input[8 * istride]);
	       tmp34 = tmp32 + tmp33;
	       tmp82 = tmp32 - tmp33;
	  }
	  {
	       fftw_real tmp4;
	       fftw_real tmp5;
	       fftw_real tmp35;
	       fftw_real tmp36;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp4 = c_re(input[4 * istride]);
	       tmp5 = c_re(input[12 * istride]);
	       tmp6 = tmp4 + tmp5;
	       tmp81 = tmp4 - tmp5;
	       tmp35 = c_im(input[4 * istride]);
	       tmp36 = c_im(input[12 * istride]);
	       tmp37 = tmp35 + tmp36;
	       tmp48 = tmp35 - tmp36;
	  }
	  tmp7 = tmp3 + tmp6;
	  tmp115 = tmp3 - tmp6;
	  tmp38 = tmp34 + tmp37;
	  tmp129 = tmp34 - tmp37;
	  tmp49 = tmp47 - tmp48;
	  tmp95 = tmp47 + tmp48;
	  tmp83 = tmp81 + tmp82;
	  tmp105 = tmp82 - tmp81;
     }
     {
	  fftw_real tmp25;
	  fftw_real tmp69;
	  fftw_real tmp77;
	  fftw_real tmp124;
	  fftw_real tmp28;
	  fftw_real tmp74;
	  fftw_real tmp72;
	  fftw_real tmp125;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp23;
	       fftw_real tmp24;
	       fftw_real tmp75;
	       fftw_real tmp76;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp23 = c_re(input[15 * istride]);
	       tmp24 = c_re(input[7 * istride]);
	       tmp25 = tmp23 + tmp24;
	       tmp69 = tmp23 - tmp24;
	       tmp75 = c_im(input[15 * istride]);
	       tmp76 = c_im(input[7 * istride]);
	       tmp77 = tmp75 - tmp76;
	       tmp124 = tmp75 + tmp76;
	  }
	  {
	       fftw_real tmp26;
	       fftw_real tmp27;
	       fftw_real tmp70;
	       fftw_real tmp71;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp26 = c_re(input[3 * istride]);
	       tmp27 = c_re(input[11 * istride]);
	       tmp28 = tmp26 + tmp27;
	       tmp74 = tmp26 - tmp27;
	       tmp70 = c_im(input[3 * istride]);
	       tmp71 = c_im(input[11 * istride]);
	       tmp72 = tmp70 - tmp71;
	       tmp125 = tmp70 + tmp71;
	  }
	  tmp29 = tmp25 + tmp28;
	  tmp123 = tmp25 - tmp28;
	  tmp73 = tmp69 - tmp72;
	  tmp101 = tmp69 + tmp72;
	  tmp78 = tmp74 + tmp77;
	  tmp102 = tmp77 - tmp74;
	  tmp126 = tmp124 - tmp125;
	  tmp141 = tmp124 + tmp125;
     }
     {
	  fftw_real tmp10;
	  fftw_real tmp51;
	  fftw_real tmp41;
	  fftw_real tmp50;
	  fftw_real tmp13;
	  fftw_real tmp53;
	  fftw_real tmp44;
	  fftw_real tmp54;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp8;
	       fftw_real tmp9;
	       fftw_real tmp39;
	       fftw_real tmp40;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp8 = c_re(input[2 * istride]);
	       tmp9 = c_re(input[10 * istride]);
	       tmp10 = tmp8 + tmp9;
	       tmp51 = tmp8 - tmp9;
	       tmp39 = c_im(input[2 * istride]);
	       tmp40 = c_im(input[10 * istride]);
	       tmp41 = tmp39 + tmp40;
	       tmp50 = tmp39 - tmp40;
	  }
	  {
	       fftw_real tmp11;
	       fftw_real tmp12;
	       fftw_real tmp42;
	       fftw_real tmp43;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp11 = c_re(input[14 * istride]);
	       tmp12 = c_re(input[6 * istride]);
	       tmp13 = tmp11 + tmp12;
	       tmp53 = tmp11 - tmp12;
	       tmp42 = c_im(input[14 * istride]);
	       tmp43 = c_im(input[6 * istride]);
	       tmp44 = tmp42 + tmp43;
	       tmp54 = tmp42 - tmp43;
	  }
	  tmp14 = tmp10 + tmp13;
	  tmp130 = tmp13 - tmp10;
	  tmp45 = tmp41 + tmp44;
	  tmp116 = tmp41 - tmp44;
	  tmp52 = tmp50 - tmp51;
	  tmp85 = tmp51 + tmp50;
	  tmp55 = tmp53 + tmp54;
	  tmp84 = tmp53 - tmp54;
     }
     {
	  fftw_real tmp18;
	  fftw_real tmp63;
	  fftw_real tmp61;
	  fftw_real tmp119;
	  fftw_real tmp21;
	  fftw_real tmp58;
	  fftw_real tmp66;
	  fftw_real tmp120;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp16;
	       fftw_real tmp17;
	       fftw_real tmp59;
	       fftw_real tmp60;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp16 = c_re(input[istride]);
	       tmp17 = c_re(input[9 * istride]);
	       tmp18 = tmp16 + tmp17;
	       tmp63 = tmp16 - tmp17;
	       tmp59 = c_im(input[istride]);
	       tmp60 = c_im(input[9 * istride]);
	       tmp61 = tmp59 - tmp60;
	       tmp119 = tmp59 + tmp60;
	  }
	  {
	       fftw_real tmp19;
	       fftw_real tmp20;
	       fftw_real tmp64;
	       fftw_real tmp65;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp19 = c_re(input[5 * istride]);
	       tmp20 = c_re(input[13 * istride]);
	       tmp21 = tmp19 + tmp20;
	       tmp58 = tmp19 - tmp20;
	       tmp64 = c_im(input[5 * istride]);
	       tmp65 = c_im(input[13 * istride]);
	       tmp66 = tmp64 - tmp65;
	       tmp120 = tmp64 + tmp65;
	  }
	  tmp22 = tmp18 + tmp21;
	  tmp118 = tmp18 - tmp21;
	  tmp62 = tmp58 + tmp61;
	  tmp98 = tmp61 - tmp58;
	  tmp67 = tmp63 - tmp66;
	  tmp99 = tmp63 + tmp66;
	  tmp121 = tmp119 - tmp120;
	  tmp140 = tmp119 + tmp120;
     }
     {
	  fftw_real tmp15;
	  fftw_real tmp30;
	  fftw_real tmp31;
	  fftw_real tmp46;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp15 = tmp7 + tmp14;
	  tmp30 = tmp22 + tmp29;
	  c_re(output[8 * ostride]) = tmp15 - tmp30;
	  c_re(output[0]) = tmp15 + tmp30;
	  tmp31 = tmp29 - tmp22;
	  tmp46 = tmp38 - tmp45;
	  c_im(output[4 * ostride]) = tmp31 + tmp46;
	  c_im(output[12 * ostride]) = tmp46 - tmp31;
     }
     {
	  fftw_real tmp143;
	  fftw_real tmp144;
	  fftw_real tmp139;
	  fftw_real tmp142;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp143 = tmp38 + tmp45;
	  tmp144 = tmp140 + tmp141;
	  c_im(output[8 * ostride]) = tmp143 - tmp144;
	  c_im(output[0]) = tmp143 + tmp144;
	  tmp139 = tmp7 - tmp14;
	  tmp142 = tmp140 - tmp141;
	  c_re(output[12 * ostride]) = tmp139 - tmp142;
	  c_re(output[4 * ostride]) = tmp139 + tmp142;
     }
     {
	  fftw_real tmp117;
	  fftw_real tmp131;
	  fftw_real tmp128;
	  fftw_real tmp132;
	  fftw_real tmp122;
	  fftw_real tmp127;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp117 = tmp115 + tmp116;
	  tmp131 = tmp129 - tmp130;
	  tmp122 = tmp118 + tmp121;
	  tmp127 = tmp123 - tmp126;
	  tmp128 = K707106781 * (tmp122 + tmp127);
	  tmp132 = K707106781 * (tmp127 - tmp122);
	  c_re(output[10 * ostride]) = tmp117 - tmp128;
	  c_re(output[2 * ostride]) = tmp117 + tmp128;
	  c_im(output[14 * ostride]) = tmp131 - tmp132;
	  c_im(output[6 * ostride]) = tmp131 + tmp132;
     }
     {
	  fftw_real tmp133;
	  fftw_real tmp137;
	  fftw_real tmp136;
	  fftw_real tmp138;
	  fftw_real tmp134;
	  fftw_real tmp135;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp133 = tmp115 - tmp116;
	  tmp137 = tmp130 + tmp129;
	  tmp134 = tmp121 - tmp118;
	  tmp135 = tmp123 + tmp126;
	  tmp136 = K707106781 * (tmp134 - tmp135);
	  tmp138 = K707106781 * (tmp134 + tmp135);
	  c_re(output[14 * ostride]) = tmp133 - tmp136;
	  c_re(output[6 * ostride]) = tmp133 + tmp136;
	  c_im(output[10 * ostride]) = tmp137 - tmp138;
	  c_im(output[2 * ostride]) = tmp137 + tmp138;
     }
     {
	  fftw_real tmp57;
	  fftw_real tmp89;
	  fftw_real tmp92;
	  fftw_real tmp94;
	  fftw_real tmp87;
	  fftw_real tmp93;
	  fftw_real tmp80;
	  fftw_real tmp88;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp56;
	       fftw_real tmp90;
	       fftw_real tmp91;
	       fftw_real tmp86;
	       fftw_real tmp68;
	       fftw_real tmp79;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp56 = K707106781 * (tmp52 - tmp55);
	       tmp57 = tmp49 + tmp56;
	       tmp89 = tmp49 - tmp56;
	       tmp90 = (K382683432 * tmp62) - (K923879532 * tmp67);
	       tmp91 = (K382683432 * tmp78) + (K923879532 * tmp73);
	       tmp92 = tmp90 - tmp91;
	       tmp94 = tmp90 + tmp91;
	       tmp86 = K707106781 * (tmp84 - tmp85);
	       tmp87 = tmp83 - tmp86;
	       tmp93 = tmp83 + tmp86;
	       tmp68 = (K923879532 * tmp62) + (K382683432 * tmp67);
	       tmp79 = (K382683432 * tmp73) - (K923879532 * tmp78);
	       tmp80 = tmp68 + tmp79;
	       tmp88 = tmp79 - tmp68;
	  }
	  c_re(output[11 * ostride]) = tmp57 - tmp80;
	  c_re(output[3 * ostride]) = tmp57 + tmp80;
	  c_im(output[15 * ostride]) = tmp87 - tmp88;
	  c_im(output[7 * ostride]) = tmp87 + tmp88;
	  c_re(output[15 * ostride]) = tmp89 - tmp92;
	  c_re(output[7 * ostride]) = tmp89 + tmp92;
	  c_im(output[11 * ostride]) = tmp93 - tmp94;
	  c_im(output[3 * ostride]) = tmp93 + tmp94;
     }
     {
	  fftw_real tmp97;
	  fftw_real tmp109;
	  fftw_real tmp112;
	  fftw_real tmp114;
	  fftw_real tmp107;
	  fftw_real tmp113;
	  fftw_real tmp104;
	  fftw_real tmp108;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp96;
	       fftw_real tmp110;
	       fftw_real tmp111;
	       fftw_real tmp106;
	       fftw_real tmp100;
	       fftw_real tmp103;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp96 = K707106781 * (tmp85 + tmp84);
	       tmp97 = tmp95 + tmp96;
	       tmp109 = tmp95 - tmp96;
	       tmp110 = (K923879532 * tmp98) - (K382683432 * tmp99);
	       tmp111 = (K923879532 * tmp102) + (K382683432 * tmp101);
	       tmp112 = tmp110 - tmp111;
	       tmp114 = tmp110 + tmp111;
	       tmp106 = K707106781 * (tmp52 + tmp55);
	       tmp107 = tmp105 - tmp106;
	       tmp113 = tmp105 + tmp106;
	       tmp100 = (K382683432 * tmp98) + (K923879532 * tmp99);
	       tmp103 = (K923879532 * tmp101) - (K382683432 * tmp102);
	       tmp104 = tmp100 + tmp103;
	       tmp108 = tmp103 - tmp100;
	  }
	  c_re(output[9 * ostride]) = tmp97 - tmp104;
	  c_re(output[ostride]) = tmp97 + tmp104;
	  c_im(output[13 * ostride]) = tmp107 - tmp108;
	  c_im(output[5 * ostride]) = tmp107 + tmp108;
	  c_re(output[13 * ostride]) = tmp109 - tmp112;
	  c_re(output[5 * ostride]) = tmp109 + tmp112;
	  c_im(output[9 * ostride]) = tmp113 - tmp114;
	  c_im(output[ostride]) = tmp113 + tmp114;
     }
}

fftw_codelet_desc fftw_no_twiddle_16_desc =
{
     "fftw_no_twiddle_16",
     (void (*)()) fftw_no_twiddle_16,
     16,
     FFTW_FORWARD,
     FFTW_NOTW,
     353,
     0,
     (const int *) 0,
};
