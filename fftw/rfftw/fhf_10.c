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
/* Generated on Sun Nov  7 20:44:49 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2hc-forward 10 */

/*
 * This function contains 168 FP additions, 84 FP multiplications,
 * (or, 126 additions, 42 multiplications, 42 fused multiply/add),
 * 43 stack variables, and 80 memory accesses
 */
static const fftw_real K587785252 = FFTW_KONST(+0.587785252292473129168705954639072768597652438);
static const fftw_real K951056516 = FFTW_KONST(+0.951056516295153572116439333379382143405698634);
static const fftw_real K250000000 = FFTW_KONST(+0.250000000000000000000000000000000000000000000);
static const fftw_real K559016994 = FFTW_KONST(+0.559016994374947424102293417182819058860154590);

/*
 * Generator Id's : 
 * $Id: fhf_10.c,v 1.3 2001-08-10 21:46:13 tom7 Exp $
 * $Id: fhf_10.c,v 1.3 2001-08-10 21:46:13 tom7 Exp $
 * $Id: fhf_10.c,v 1.3 2001-08-10 21:46:13 tom7 Exp $
 */

void fftw_hc2hc_forward_10(fftw_real *A, const fftw_complex *W, int iostride, int m, int dist)
{
     int i;
     fftw_real *X;
     fftw_real *Y;
     X = A;
     Y = A + (10 * iostride);
     {
	  fftw_real tmp170;
	  fftw_real tmp181;
	  fftw_real tmp162;
	  fftw_real tmp175;
	  fftw_real tmp165;
	  fftw_real tmp176;
	  fftw_real tmp166;
	  fftw_real tmp183;
	  fftw_real tmp155;
	  fftw_real tmp178;
	  fftw_real tmp158;
	  fftw_real tmp179;
	  fftw_real tmp159;
	  fftw_real tmp182;
	  fftw_real tmp168;
	  fftw_real tmp169;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp168 = X[0];
	  tmp169 = X[5 * iostride];
	  tmp170 = tmp168 - tmp169;
	  tmp181 = tmp168 + tmp169;
	  {
	       fftw_real tmp160;
	       fftw_real tmp161;
	       fftw_real tmp163;
	       fftw_real tmp164;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp160 = X[4 * iostride];
	       tmp161 = X[9 * iostride];
	       tmp162 = tmp160 - tmp161;
	       tmp175 = tmp160 + tmp161;
	       tmp163 = X[6 * iostride];
	       tmp164 = X[iostride];
	       tmp165 = tmp163 - tmp164;
	       tmp176 = tmp163 + tmp164;
	  }
	  tmp166 = tmp162 + tmp165;
	  tmp183 = tmp175 + tmp176;
	  {
	       fftw_real tmp153;
	       fftw_real tmp154;
	       fftw_real tmp156;
	       fftw_real tmp157;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp153 = X[2 * iostride];
	       tmp154 = X[7 * iostride];
	       tmp155 = tmp153 - tmp154;
	       tmp178 = tmp153 + tmp154;
	       tmp156 = X[8 * iostride];
	       tmp157 = X[3 * iostride];
	       tmp158 = tmp156 - tmp157;
	       tmp179 = tmp156 + tmp157;
	  }
	  tmp159 = tmp155 + tmp158;
	  tmp182 = tmp178 + tmp179;
	  {
	       fftw_real tmp167;
	       fftw_real tmp171;
	       fftw_real tmp172;
	       fftw_real tmp186;
	       fftw_real tmp184;
	       fftw_real tmp185;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp167 = K559016994 * (tmp159 - tmp166);
	       tmp171 = tmp159 + tmp166;
	       tmp172 = tmp170 - (K250000000 * tmp171);
	       X[iostride] = tmp167 + tmp172;
	       X[3 * iostride] = tmp172 - tmp167;
	       X[5 * iostride] = tmp170 + tmp171;
	       tmp186 = K559016994 * (tmp182 - tmp183);
	       tmp184 = tmp182 + tmp183;
	       tmp185 = tmp181 - (K250000000 * tmp184);
	       X[2 * iostride] = tmp185 - tmp186;
	       X[4 * iostride] = tmp186 + tmp185;
	       X[0] = tmp181 + tmp184;
	  }
	  {
	       fftw_real tmp173;
	       fftw_real tmp174;
	       fftw_real tmp177;
	       fftw_real tmp180;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp173 = tmp155 - tmp158;
	       tmp174 = tmp162 - tmp165;
	       Y[-iostride] = -((K951056516 * tmp173) + (K587785252 * tmp174));
	       Y[-3 * iostride] = (K587785252 * tmp173) - (K951056516 * tmp174);
	       tmp177 = tmp175 - tmp176;
	       tmp180 = tmp178 - tmp179;
	       Y[-2 * iostride] = (K951056516 * tmp177) - (K587785252 * tmp180);
	       Y[-4 * iostride] = (K951056516 * tmp180) + (K587785252 * tmp177);
	  }
     }
     X = X + dist;
     Y = Y - dist;
     for (i = 2; i < m; i = i + 2, X = X + dist, Y = Y - dist, W = W + 9) {
	  fftw_real tmp39;
	  fftw_real tmp87;
	  fftw_real tmp132;
	  fftw_real tmp144;
	  fftw_real tmp73;
	  fftw_real tmp84;
	  fftw_real tmp85;
	  fftw_real tmp91;
	  fftw_real tmp92;
	  fftw_real tmp93;
	  fftw_real tmp100;
	  fftw_real tmp103;
	  fftw_real tmp128;
	  fftw_real tmp121;
	  fftw_real tmp122;
	  fftw_real tmp142;
	  fftw_real tmp50;
	  fftw_real tmp61;
	  fftw_real tmp62;
	  fftw_real tmp88;
	  fftw_real tmp89;
	  fftw_real tmp90;
	  fftw_real tmp107;
	  fftw_real tmp110;
	  fftw_real tmp127;
	  fftw_real tmp118;
	  fftw_real tmp119;
	  fftw_real tmp141;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp33;
	       fftw_real tmp131;
	       fftw_real tmp38;
	       fftw_real tmp130;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp33 = X[0];
	       tmp131 = Y[-9 * iostride];
	       {
		    fftw_real tmp35;
		    fftw_real tmp37;
		    fftw_real tmp34;
		    fftw_real tmp36;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp35 = X[5 * iostride];
		    tmp37 = Y[-4 * iostride];
		    tmp34 = c_re(W[4]);
		    tmp36 = c_im(W[4]);
		    tmp38 = (tmp34 * tmp35) - (tmp36 * tmp37);
		    tmp130 = (tmp36 * tmp35) + (tmp34 * tmp37);
	       }
	       tmp39 = tmp33 - tmp38;
	       tmp87 = tmp33 + tmp38;
	       tmp132 = tmp130 + tmp131;
	       tmp144 = tmp131 - tmp130;
	  }
	  {
	       fftw_real tmp67;
	       fftw_real tmp98;
	       fftw_real tmp83;
	       fftw_real tmp102;
	       fftw_real tmp72;
	       fftw_real tmp99;
	       fftw_real tmp78;
	       fftw_real tmp101;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp64;
		    fftw_real tmp66;
		    fftw_real tmp63;
		    fftw_real tmp65;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp64 = X[4 * iostride];
		    tmp66 = Y[-5 * iostride];
		    tmp63 = c_re(W[3]);
		    tmp65 = c_im(W[3]);
		    tmp67 = (tmp63 * tmp64) - (tmp65 * tmp66);
		    tmp98 = (tmp65 * tmp64) + (tmp63 * tmp66);
	       }
	       {
		    fftw_real tmp80;
		    fftw_real tmp82;
		    fftw_real tmp79;
		    fftw_real tmp81;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp80 = X[iostride];
		    tmp82 = Y[-8 * iostride];
		    tmp79 = c_re(W[0]);
		    tmp81 = c_im(W[0]);
		    tmp83 = (tmp79 * tmp80) - (tmp81 * tmp82);
		    tmp102 = (tmp81 * tmp80) + (tmp79 * tmp82);
	       }
	       {
		    fftw_real tmp69;
		    fftw_real tmp71;
		    fftw_real tmp68;
		    fftw_real tmp70;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp69 = X[9 * iostride];
		    tmp71 = Y[0];
		    tmp68 = c_re(W[8]);
		    tmp70 = c_im(W[8]);
		    tmp72 = (tmp68 * tmp69) - (tmp70 * tmp71);
		    tmp99 = (tmp70 * tmp69) + (tmp68 * tmp71);
	       }
	       {
		    fftw_real tmp75;
		    fftw_real tmp77;
		    fftw_real tmp74;
		    fftw_real tmp76;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp75 = X[6 * iostride];
		    tmp77 = Y[-3 * iostride];
		    tmp74 = c_re(W[5]);
		    tmp76 = c_im(W[5]);
		    tmp78 = (tmp74 * tmp75) - (tmp76 * tmp77);
		    tmp101 = (tmp76 * tmp75) + (tmp74 * tmp77);
	       }
	       tmp73 = tmp67 - tmp72;
	       tmp84 = tmp78 - tmp83;
	       tmp85 = tmp73 + tmp84;
	       tmp91 = tmp67 + tmp72;
	       tmp92 = tmp78 + tmp83;
	       tmp93 = tmp91 + tmp92;
	       tmp100 = tmp98 + tmp99;
	       tmp103 = tmp101 + tmp102;
	       tmp128 = tmp100 + tmp103;
	       tmp121 = tmp98 - tmp99;
	       tmp122 = tmp101 - tmp102;
	       tmp142 = tmp121 + tmp122;
	  }
	  {
	       fftw_real tmp44;
	       fftw_real tmp105;
	       fftw_real tmp60;
	       fftw_real tmp109;
	       fftw_real tmp49;
	       fftw_real tmp106;
	       fftw_real tmp55;
	       fftw_real tmp108;
	       ASSERT_ALIGNED_DOUBLE;
	       {
		    fftw_real tmp41;
		    fftw_real tmp43;
		    fftw_real tmp40;
		    fftw_real tmp42;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp41 = X[2 * iostride];
		    tmp43 = Y[-7 * iostride];
		    tmp40 = c_re(W[1]);
		    tmp42 = c_im(W[1]);
		    tmp44 = (tmp40 * tmp41) - (tmp42 * tmp43);
		    tmp105 = (tmp42 * tmp41) + (tmp40 * tmp43);
	       }
	       {
		    fftw_real tmp57;
		    fftw_real tmp59;
		    fftw_real tmp56;
		    fftw_real tmp58;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp57 = X[3 * iostride];
		    tmp59 = Y[-6 * iostride];
		    tmp56 = c_re(W[2]);
		    tmp58 = c_im(W[2]);
		    tmp60 = (tmp56 * tmp57) - (tmp58 * tmp59);
		    tmp109 = (tmp58 * tmp57) + (tmp56 * tmp59);
	       }
	       {
		    fftw_real tmp46;
		    fftw_real tmp48;
		    fftw_real tmp45;
		    fftw_real tmp47;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp46 = X[7 * iostride];
		    tmp48 = Y[-2 * iostride];
		    tmp45 = c_re(W[6]);
		    tmp47 = c_im(W[6]);
		    tmp49 = (tmp45 * tmp46) - (tmp47 * tmp48);
		    tmp106 = (tmp47 * tmp46) + (tmp45 * tmp48);
	       }
	       {
		    fftw_real tmp52;
		    fftw_real tmp54;
		    fftw_real tmp51;
		    fftw_real tmp53;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp52 = X[8 * iostride];
		    tmp54 = Y[-iostride];
		    tmp51 = c_re(W[7]);
		    tmp53 = c_im(W[7]);
		    tmp55 = (tmp51 * tmp52) - (tmp53 * tmp54);
		    tmp108 = (tmp53 * tmp52) + (tmp51 * tmp54);
	       }
	       tmp50 = tmp44 - tmp49;
	       tmp61 = tmp55 - tmp60;
	       tmp62 = tmp50 + tmp61;
	       tmp88 = tmp44 + tmp49;
	       tmp89 = tmp55 + tmp60;
	       tmp90 = tmp88 + tmp89;
	       tmp107 = tmp105 + tmp106;
	       tmp110 = tmp108 + tmp109;
	       tmp127 = tmp107 + tmp110;
	       tmp118 = tmp105 - tmp106;
	       tmp119 = tmp108 - tmp109;
	       tmp141 = tmp118 + tmp119;
	  }
	  {
	       fftw_real tmp115;
	       fftw_real tmp86;
	       fftw_real tmp116;
	       fftw_real tmp124;
	       fftw_real tmp126;
	       fftw_real tmp120;
	       fftw_real tmp123;
	       fftw_real tmp125;
	       fftw_real tmp117;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp115 = K559016994 * (tmp62 - tmp85);
	       tmp86 = tmp62 + tmp85;
	       tmp116 = tmp39 - (K250000000 * tmp86);
	       tmp120 = tmp118 - tmp119;
	       tmp123 = tmp121 - tmp122;
	       tmp124 = (K951056516 * tmp120) + (K587785252 * tmp123);
	       tmp126 = (K951056516 * tmp123) - (K587785252 * tmp120);
	       Y[-5 * iostride] = tmp39 + tmp86;
	       tmp125 = tmp116 - tmp115;
	       Y[-7 * iostride] = tmp125 - tmp126;
	       X[3 * iostride] = tmp125 + tmp126;
	       tmp117 = tmp115 + tmp116;
	       Y[-9 * iostride] = tmp117 - tmp124;
	       X[iostride] = tmp117 + tmp124;
	  }
	  {
	       fftw_real tmp148;
	       fftw_real tmp143;
	       fftw_real tmp149;
	       fftw_real tmp147;
	       fftw_real tmp151;
	       fftw_real tmp145;
	       fftw_real tmp146;
	       fftw_real tmp152;
	       fftw_real tmp150;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp148 = K559016994 * (tmp141 - tmp142);
	       tmp143 = tmp141 + tmp142;
	       tmp149 = tmp144 - (K250000000 * tmp143);
	       tmp145 = tmp50 - tmp61;
	       tmp146 = tmp73 - tmp84;
	       tmp147 = (K951056516 * tmp145) + (K587785252 * tmp146);
	       tmp151 = (K587785252 * tmp145) - (K951056516 * tmp146);
	       X[5 * iostride] = -(tmp143 + tmp144);
	       tmp152 = tmp149 - tmp148;
	       X[7 * iostride] = tmp151 - tmp152;
	       Y[-3 * iostride] = tmp151 + tmp152;
	       tmp150 = tmp148 + tmp149;
	       X[9 * iostride] = -(tmp147 + tmp150);
	       Y[-iostride] = tmp150 - tmp147;
	  }
	  {
	       fftw_real tmp96;
	       fftw_real tmp94;
	       fftw_real tmp95;
	       fftw_real tmp112;
	       fftw_real tmp114;
	       fftw_real tmp104;
	       fftw_real tmp111;
	       fftw_real tmp113;
	       fftw_real tmp97;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp96 = K559016994 * (tmp90 - tmp93);
	       tmp94 = tmp90 + tmp93;
	       tmp95 = tmp87 - (K250000000 * tmp94);
	       tmp104 = tmp100 - tmp103;
	       tmp111 = tmp107 - tmp110;
	       tmp112 = (K951056516 * tmp104) - (K587785252 * tmp111);
	       tmp114 = (K951056516 * tmp111) + (K587785252 * tmp104);
	       X[0] = tmp87 + tmp94;
	       tmp113 = tmp96 + tmp95;
	       X[4 * iostride] = tmp113 - tmp114;
	       Y[-6 * iostride] = tmp113 + tmp114;
	       tmp97 = tmp95 - tmp96;
	       X[2 * iostride] = tmp97 - tmp112;
	       Y[-8 * iostride] = tmp97 + tmp112;
	  }
	  {
	       fftw_real tmp134;
	       fftw_real tmp129;
	       fftw_real tmp133;
	       fftw_real tmp138;
	       fftw_real tmp140;
	       fftw_real tmp136;
	       fftw_real tmp137;
	       fftw_real tmp139;
	       fftw_real tmp135;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp134 = K559016994 * (tmp127 - tmp128);
	       tmp129 = tmp127 + tmp128;
	       tmp133 = tmp132 - (K250000000 * tmp129);
	       tmp136 = tmp91 - tmp92;
	       tmp137 = tmp88 - tmp89;
	       tmp138 = (K951056516 * tmp136) - (K587785252 * tmp137);
	       tmp140 = (K951056516 * tmp137) + (K587785252 * tmp136);
	       Y[0] = tmp129 + tmp132;
	       tmp139 = tmp134 + tmp133;
	       X[6 * iostride] = -(tmp139 - tmp140);
	       Y[-4 * iostride] = tmp140 + tmp139;
	       tmp135 = tmp133 - tmp134;
	       X[8 * iostride] = -(tmp135 - tmp138);
	       Y[-2 * iostride] = tmp138 + tmp135;
	  }
     }
     if (i == m) {
	  fftw_real tmp1;
	  fftw_real tmp24;
	  fftw_real tmp8;
	  fftw_real tmp10;
	  fftw_real tmp25;
	  fftw_real tmp26;
	  fftw_real tmp14;
	  fftw_real tmp28;
	  fftw_real tmp23;
	  fftw_real tmp17;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp1 = X[0];
	  tmp24 = X[5 * iostride];
	  {
	       fftw_real tmp2;
	       fftw_real tmp3;
	       fftw_real tmp4;
	       fftw_real tmp5;
	       fftw_real tmp6;
	       fftw_real tmp7;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp2 = X[4 * iostride];
	       tmp3 = X[6 * iostride];
	       tmp4 = tmp2 - tmp3;
	       tmp5 = X[8 * iostride];
	       tmp6 = X[2 * iostride];
	       tmp7 = tmp5 - tmp6;
	       tmp8 = tmp4 + tmp7;
	       tmp10 = K559016994 * (tmp4 - tmp7);
	       tmp25 = tmp2 + tmp3;
	       tmp26 = tmp5 + tmp6;
	  }
	  {
	       fftw_real tmp12;
	       fftw_real tmp13;
	       fftw_real tmp22;
	       fftw_real tmp15;
	       fftw_real tmp16;
	       fftw_real tmp21;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp12 = X[iostride];
	       tmp13 = X[9 * iostride];
	       tmp22 = tmp12 + tmp13;
	       tmp15 = X[3 * iostride];
	       tmp16 = X[7 * iostride];
	       tmp21 = tmp15 + tmp16;
	       tmp14 = tmp12 - tmp13;
	       tmp28 = K559016994 * (tmp22 + tmp21);
	       tmp23 = tmp21 - tmp22;
	       tmp17 = tmp15 - tmp16;
	  }
	  X[2 * iostride] = tmp1 + tmp8;
	  {
	       fftw_real tmp18;
	       fftw_real tmp20;
	       fftw_real tmp11;
	       fftw_real tmp19;
	       fftw_real tmp9;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp18 = (K587785252 * tmp14) - (K951056516 * tmp17);
	       tmp20 = (K951056516 * tmp14) + (K587785252 * tmp17);
	       tmp9 = tmp1 - (K250000000 * tmp8);
	       tmp11 = tmp9 - tmp10;
	       tmp19 = tmp10 + tmp9;
	       X[3 * iostride] = tmp11 - tmp18;
	       X[iostride] = tmp11 + tmp18;
	       X[4 * iostride] = tmp19 - tmp20;
	       X[0] = tmp19 + tmp20;
	  }
	  Y[-2 * iostride] = tmp23 - tmp24;
	  {
	       fftw_real tmp27;
	       fftw_real tmp32;
	       fftw_real tmp30;
	       fftw_real tmp31;
	       fftw_real tmp29;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp27 = (K951056516 * tmp25) + (K587785252 * tmp26);
	       tmp32 = (K951056516 * tmp26) - (K587785252 * tmp25);
	       tmp29 = (K250000000 * tmp23) + tmp24;
	       tmp30 = tmp28 + tmp29;
	       tmp31 = tmp29 - tmp28;
	       Y[0] = -(tmp27 + tmp30);
	       Y[-4 * iostride] = tmp27 - tmp30;
	       Y[-iostride] = tmp31 - tmp32;
	       Y[-3 * iostride] = tmp32 + tmp31;
	  }
     }
}

static const int twiddle_order[] =
{1, 2, 3, 4, 5, 6, 7, 8, 9};
fftw_codelet_desc fftw_hc2hc_forward_10_desc =
{
     "fftw_hc2hc_forward_10",
     (void (*)()) fftw_hc2hc_forward_10,
     10,
     FFTW_FORWARD,
     FFTW_HC2HC,
     223,
     9,
     twiddle_order,
};
