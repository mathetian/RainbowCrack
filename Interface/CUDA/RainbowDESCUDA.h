// Copyright (c) 2014 The RainbowCrack Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _DES_CUDA_H
#define _DES_CUDA_H

#include "RainbowCUDA.h"

namespace rainbowcrack
{

__device__ uint32_t plRight = 0x5A296F6D;
__device__ uint32_t plLeft  = 0x28325530;

/// __device__ uint64_t totalSpace_DES = (1ull << 43) - 2 - (1ull << 8) - (1ull << 16) - (1ull << 24) - (1ull << 32) - (1ull << 40);
/// uint64_t totalSpace_Global_DES     = (1ull << 43) - 2 - (1ull << 8) - (1ull << 16) - (1ull << 24) - (1ull << 32) - (1ull << 40);
__device__ uint64_t totalSpace_DES = (1ull << 34) - 2 - (1ull << 8) - (1ull << 16) - (1ull << 24);
uint64_t totalSpace_Global_DES     = (1ull << 34) - 2 - (1ull << 8) - (1ull << 16) - (1ull << 24);

__device__ uint32_t des_d_sp_c[8][64]=
{
    {
        /* nibble 0 */
        0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
        0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
        0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
        0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,
        0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,
        0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,
        0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,
        0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,
        0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,
        0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,
        0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,
        0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,
        0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,
        0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
        0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
        0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
    },{
        /* nibble 1 */
        0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
        0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
        0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
        0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
        0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
        0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
        0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
        0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
        0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
        0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
        0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
        0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
        0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
        0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
        0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
        0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
    },{
        /* nibble 2 */
        0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
        0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
        0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
        0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
        0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
        0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,
        0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,
        0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,
        0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,
        0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,
        0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,
        0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,
        0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,
        0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
        0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
        0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
    },{
        /* nibble 3 */
        0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
        0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,
        0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,
        0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,
        0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,
        0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,
        0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,
        0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,
        0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,
        0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
        0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
        0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
        0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
        0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
        0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
        0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
    },{
        /* nibble 4 */
        0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,
        0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,
        0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,
        0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,
        0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,
        0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,
        0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,
        0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,
        0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
        0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
        0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
        0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
        0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
        0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
        0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
        0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
    },{
        /* nibble 5 */
        0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
        0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
        0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,
        0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,
        0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,
        0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,
        0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,
        0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,
        0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,
        0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,
        0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
        0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
        0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
        0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
        0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
        0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
    },{
        /* nibble 6 */
        0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
        0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
        0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
        0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
        0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
        0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
        0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
        0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
        0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
        0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
        0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
        0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
        0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
        0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
        0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
        0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
    },{
        /* nibble 7 */
        0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
        0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
        0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
        0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
        0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
        0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
        0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
        0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
        0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
        0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
        0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
        0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
        0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
        0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
        0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
        0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
    }
};

__shared__ unsigned char des_SP[2048];

#define IP(left,right) { \
	register uint32_t tt; \
	PERM_OP(right,left,tt, 4,0x0f0f0f0fL); \
	PERM_OP(left,right,tt,16,0x0000ffffL); \
	PERM_OP(right,left,tt, 2,0x33333333L); \
	PERM_OP(left,right,tt, 8,0x00ff00ffL); \
	PERM_OP(right,left,tt, 1,0x55555555L); \
}

#define FP(left,right) { \
	register uint32_t tt; \
	PERM_OP(left,right,tt, 1,0x55555555L); \
	PERM_OP(right,left,tt, 8,0x00ff00ffL); \
	PERM_OP(left,right,tt, 2,0x33333333L); \
	PERM_OP(right,left,tt,16,0x0000ffffL); \
	PERM_OP(left,right,tt, 4,0x0f0f0f0fL); \
}

#define	ROTATE(a,n)	(((a)>>(n))|((a)<<(32-(n))))

#define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
	(b)^=(t),\
	(a)^=((t)<<(n)))

#define D_ENCRYPT(LL,R,S) { \
	register uint32_t t,u; \
	u=R^roundKeys[S]; \
	t=R^roundKeys[S]>>32; \
	t=ROTATE(t,4); \
	LL ^= *(uint32_t *)(des_SP      +((u     )&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x100+((t     )&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x200+((u>> 8L)&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x300+((t>> 8L)&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x400+((u>>16L)&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x500+((t>>16L)&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x600+((u>>24L)&0xfc)); \
	LL ^= *(uint32_t *)(des_SP+0x700+((t>>24L)&0xfc)); \
}

#define c2l(c,l)	(l =((unsigned long)(*((c)++)))    , \
			 l|=((unsigned long)(*((c)++)))<< 8L, \
			 l|=((unsigned long)(*((c)++)))<<16L, \
			 l|=((unsigned long)(*((c)++)))<<24L)

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
	(a)=(a)^(t)^(t>>(16-(n))))

__device__ uint32_t des_skb[8][64]=
{
    {
        /* for C bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
        0x00000000L,0x00000010L,0x20000000L,0x20000010L,
        0x00010000L,0x00010010L,0x20010000L,0x20010010L,
        0x00000800L,0x00000810L,0x20000800L,0x20000810L,
        0x00010800L,0x00010810L,0x20010800L,0x20010810L,
        0x00000020L,0x00000030L,0x20000020L,0x20000030L,
        0x00010020L,0x00010030L,0x20010020L,0x20010030L,
        0x00000820L,0x00000830L,0x20000820L,0x20000830L,
        0x00010820L,0x00010830L,0x20010820L,0x20010830L,
        0x00080000L,0x00080010L,0x20080000L,0x20080010L,
        0x00090000L,0x00090010L,0x20090000L,0x20090010L,
        0x00080800L,0x00080810L,0x20080800L,0x20080810L,
        0x00090800L,0x00090810L,0x20090800L,0x20090810L,
        0x00080020L,0x00080030L,0x20080020L,0x20080030L,
        0x00090020L,0x00090030L,0x20090020L,0x20090030L,
        0x00080820L,0x00080830L,0x20080820L,0x20080830L,
        0x00090820L,0x00090830L,0x20090820L,0x20090830L,
    },{
        /* for C bits (numbered as per FIPS 46) 7 8 10 11 12 13 */
        0x00000000L,0x02000000L,0x00002000L,0x02002000L,
        0x00200000L,0x02200000L,0x00202000L,0x02202000L,
        0x00000004L,0x02000004L,0x00002004L,0x02002004L,
        0x00200004L,0x02200004L,0x00202004L,0x02202004L,
        0x00000400L,0x02000400L,0x00002400L,0x02002400L,
        0x00200400L,0x02200400L,0x00202400L,0x02202400L,
        0x00000404L,0x02000404L,0x00002404L,0x02002404L,
        0x00200404L,0x02200404L,0x00202404L,0x02202404L,
        0x10000000L,0x12000000L,0x10002000L,0x12002000L,
        0x10200000L,0x12200000L,0x10202000L,0x12202000L,
        0x10000004L,0x12000004L,0x10002004L,0x12002004L,
        0x10200004L,0x12200004L,0x10202004L,0x12202004L,
        0x10000400L,0x12000400L,0x10002400L,0x12002400L,
        0x10200400L,0x12200400L,0x10202400L,0x12202400L,
        0x10000404L,0x12000404L,0x10002404L,0x12002404L,
        0x10200404L,0x12200404L,0x10202404L,0x12202404L,
    },{
        /* for C bits (numbered as per FIPS 46) 14 15 16 17 19 20 */
        0x00000000L,0x00000001L,0x00040000L,0x00040001L,
        0x01000000L,0x01000001L,0x01040000L,0x01040001L,
        0x00000002L,0x00000003L,0x00040002L,0x00040003L,
        0x01000002L,0x01000003L,0x01040002L,0x01040003L,
        0x00000200L,0x00000201L,0x00040200L,0x00040201L,
        0x01000200L,0x01000201L,0x01040200L,0x01040201L,
        0x00000202L,0x00000203L,0x00040202L,0x00040203L,
        0x01000202L,0x01000203L,0x01040202L,0x01040203L,
        0x08000000L,0x08000001L,0x08040000L,0x08040001L,
        0x09000000L,0x09000001L,0x09040000L,0x09040001L,
        0x08000002L,0x08000003L,0x08040002L,0x08040003L,
        0x09000002L,0x09000003L,0x09040002L,0x09040003L,
        0x08000200L,0x08000201L,0x08040200L,0x08040201L,
        0x09000200L,0x09000201L,0x09040200L,0x09040201L,
        0x08000202L,0x08000203L,0x08040202L,0x08040203L,
        0x09000202L,0x09000203L,0x09040202L,0x09040203L,
    },{
        /* for C bits (numbered as per FIPS 46) 21 23 24 26 27 28 */
        0x00000000L,0x00100000L,0x00000100L,0x00100100L,
        0x00000008L,0x00100008L,0x00000108L,0x00100108L,
        0x00001000L,0x00101000L,0x00001100L,0x00101100L,
        0x00001008L,0x00101008L,0x00001108L,0x00101108L,
        0x04000000L,0x04100000L,0x04000100L,0x04100100L,
        0x04000008L,0x04100008L,0x04000108L,0x04100108L,
        0x04001000L,0x04101000L,0x04001100L,0x04101100L,
        0x04001008L,0x04101008L,0x04001108L,0x04101108L,
        0x00020000L,0x00120000L,0x00020100L,0x00120100L,
        0x00020008L,0x00120008L,0x00020108L,0x00120108L,
        0x00021000L,0x00121000L,0x00021100L,0x00121100L,
        0x00021008L,0x00121008L,0x00021108L,0x00121108L,
        0x04020000L,0x04120000L,0x04020100L,0x04120100L,
        0x04020008L,0x04120008L,0x04020108L,0x04120108L,
        0x04021000L,0x04121000L,0x04021100L,0x04121100L,
        0x04021008L,0x04121008L,0x04021108L,0x04121108L,
    },{
        /* for D bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
        0x00000000L,0x10000000L,0x00010000L,0x10010000L,
        0x00000004L,0x10000004L,0x00010004L,0x10010004L,
        0x20000000L,0x30000000L,0x20010000L,0x30010000L,
        0x20000004L,0x30000004L,0x20010004L,0x30010004L,
        0x00100000L,0x10100000L,0x00110000L,0x10110000L,
        0x00100004L,0x10100004L,0x00110004L,0x10110004L,
        0x20100000L,0x30100000L,0x20110000L,0x30110000L,
        0x20100004L,0x30100004L,0x20110004L,0x30110004L,
        0x00001000L,0x10001000L,0x00011000L,0x10011000L,
        0x00001004L,0x10001004L,0x00011004L,0x10011004L,
        0x20001000L,0x30001000L,0x20011000L,0x30011000L,
        0x20001004L,0x30001004L,0x20011004L,0x30011004L,
        0x00101000L,0x10101000L,0x00111000L,0x10111000L,
        0x00101004L,0x10101004L,0x00111004L,0x10111004L,
        0x20101000L,0x30101000L,0x20111000L,0x30111000L,
        0x20101004L,0x30101004L,0x20111004L,0x30111004L,
    },{
        /* for D bits (numbered as per FIPS 46) 8 9 11 12 13 14 */
        0x00000000L,0x08000000L,0x00000008L,0x08000008L,
        0x00000400L,0x08000400L,0x00000408L,0x08000408L,
        0x00020000L,0x08020000L,0x00020008L,0x08020008L,
        0x00020400L,0x08020400L,0x00020408L,0x08020408L,
        0x00000001L,0x08000001L,0x00000009L,0x08000009L,
        0x00000401L,0x08000401L,0x00000409L,0x08000409L,
        0x00020001L,0x08020001L,0x00020009L,0x08020009L,
        0x00020401L,0x08020401L,0x00020409L,0x08020409L,
        0x02000000L,0x0A000000L,0x02000008L,0x0A000008L,
        0x02000400L,0x0A000400L,0x02000408L,0x0A000408L,
        0x02020000L,0x0A020000L,0x02020008L,0x0A020008L,
        0x02020400L,0x0A020400L,0x02020408L,0x0A020408L,
        0x02000001L,0x0A000001L,0x02000009L,0x0A000009L,
        0x02000401L,0x0A000401L,0x02000409L,0x0A000409L,
        0x02020001L,0x0A020001L,0x02020009L,0x0A020009L,
        0x02020401L,0x0A020401L,0x02020409L,0x0A020409L,
    },{
        /* for D bits (numbered as per FIPS 46) 16 17 18 19 20 21 */
        0x00000000L,0x00000100L,0x00080000L,0x00080100L,
        0x01000000L,0x01000100L,0x01080000L,0x01080100L,
        0x00000010L,0x00000110L,0x00080010L,0x00080110L,
        0x01000010L,0x01000110L,0x01080010L,0x01080110L,
        0x00200000L,0x00200100L,0x00280000L,0x00280100L,
        0x01200000L,0x01200100L,0x01280000L,0x01280100L,
        0x00200010L,0x00200110L,0x00280010L,0x00280110L,
        0x01200010L,0x01200110L,0x01280010L,0x01280110L,
        0x00000200L,0x00000300L,0x00080200L,0x00080300L,
        0x01000200L,0x01000300L,0x01080200L,0x01080300L,
        0x00000210L,0x00000310L,0x00080210L,0x00080310L,
        0x01000210L,0x01000310L,0x01080210L,0x01080310L,
        0x00200200L,0x00200300L,0x00280200L,0x00280300L,
        0x01200200L,0x01200300L,0x01280200L,0x01280300L,
        0x00200210L,0x00200310L,0x00280210L,0x00280310L,
        0x01200210L,0x01200310L,0x01280210L,0x01280310L,
    },{
        /* for D bits (numbered as per FIPS 46) 22 23 24 25 27 28 */
        0x00000000L,0x04000000L,0x00040000L,0x04040000L,
        0x00000002L,0x04000002L,0x00040002L,0x04040002L,
        0x00002000L,0x04002000L,0x00042000L,0x04042000L,
        0x00002002L,0x04002002L,0x00042002L,0x04042002L,
        0x00000020L,0x04000020L,0x00040020L,0x04040020L,
        0x00000022L,0x04000022L,0x00040022L,0x04040022L,
        0x00002020L,0x04002020L,0x00042020L,0x04042020L,
        0x00002022L,0x04002022L,0x00042022L,0x04042022L,
        0x00000800L,0x04000800L,0x00040800L,0x04040800L,
        0x00000802L,0x04000802L,0x00040802L,0x04040802L,
        0x00002800L,0x04002800L,0x00042800L,0x04042800L,
        0x00002802L,0x04002802L,0x00042802L,0x04042802L,
        0x00000820L,0x04000820L,0x00040820L,0x04040820L,
        0x00000822L,0x04000822L,0x00040822L,0x04040822L,
        0x00002820L,0x04002820L,0x00042820L,0x04042820L,
        0x00002822L,0x04002822L,0x00042822L,0x04042822L,
    }
};

__device__ int shifts2[16]= {0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0};

#define RoundKey0(S) { \
	c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L));\
	c&=0x0fffffffL;d&=0x0fffffffL;\
	s=	des_skb[0][ (c    )&0x3f                ]|\
		des_skb[1][((c>> 6L)&0x03)|((c>> 7L)&0x3c)]|\
		des_skb[2][((c>>13L)&0x0f)|((c>>14L)&0x30)]|\
		des_skb[3][((c>>20L)&0x01)|((c>>21L)&0x06) |\
					  ((c>>22L)&0x38)];\
	t=	des_skb[4][ (d    )&0x3f                ]|\
		des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|\
		des_skb[6][ (d>>15L)&0x3f                ]|\
		des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];\
	t2=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;\
	store[S]  = ROTATE(t2,30)&0xffffffffL;\
	t2=((s>>16L)|(t&0xffff0000L));\
	tmp=(ROTATE(t2,26)&0xffffffffL);\
	store[S] |= (tmp << 32);\
}

#define RoundKey1(S) { \
	c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L));\
	c&=0x0fffffffL;d&=0x0fffffffL;\
	s=	des_skb[0][ (c    )&0x3f                ]|\
		des_skb[1][((c>> 6L)&0x03)|((c>> 7L)&0x3c)]|\
		des_skb[2][((c>>13L)&0x0f)|((c>>14L)&0x30)]|\
		des_skb[3][((c>>20L)&0x01)|((c>>21L)&0x06) |\
					  ((c>>22L)&0x38)];\
	t=	des_skb[4][ (d    )&0x3f                ]|\
		des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|\
		des_skb[6][ (d>>15L)&0x3f                ]|\
		des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];\
	t2=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;\
	store[S]  = ROTATE(t2,30)&0xffffffffL;\
	t2=((s>>16L)|(t&0xffff0000L));\
	tmp=(ROTATE(t2,26)&0xffffffffL);\
	store[S] |= (tmp << 32);\
}

__device__ int GenerateKey(uint64_t key, uint64_t *store)
{
    uint32_t c, d, t, s, t2;
    uint64_t tmp;
    c = ((1ull << 32) - 1) & key;
    d = (key >> 32);

    PERM_OP (d,c,t,4,0x0f0f0f0fL);
    HPERM_OP(c,t, -2,0xcccc0000L);
    HPERM_OP(d,t, -2,0xcccc0000L);
    PERM_OP (d,c,t,1,0x55555555L);
    PERM_OP (c,d,t,8,0x00ff00ffL);
    PERM_OP (d,c,t,1,0x55555555L);

    d = (((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
         ((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
    c&=0x0fffffffL;

    RoundKey0(0);
    RoundKey0(1);
    RoundKey1(2);
    RoundKey1(3);
    RoundKey1(4);
    RoundKey1(5);
    RoundKey1(6);
    RoundKey1(7);
    RoundKey0(8);
    RoundKey1(9);
    RoundKey1(10);
    RoundKey1(11);
    RoundKey1(12);
    RoundKey1(13);
    RoundKey1(14);
    RoundKey0(15);

    return 0;
}

__device__ uint64_t DESOneTime(uint64_t *roundKeys)
{
    uint64_t rs;
    uint32_t right = plRight, left = plLeft;

    IP(right, left);

    left  = ROTATE(left,29)&0xffffffffL;
    right = ROTATE(right,29)&0xffffffffL;

    D_ENCRYPT(left,right, 0);
    D_ENCRYPT(right,left, 1);
    D_ENCRYPT(left,right, 2);
    D_ENCRYPT(right,left, 3);
    D_ENCRYPT(left,right, 4);
    D_ENCRYPT(right,left, 5);
    D_ENCRYPT(left,right, 6);
    D_ENCRYPT(right,left, 7);
    D_ENCRYPT(left,right, 8);
    D_ENCRYPT(right,left, 9);
    D_ENCRYPT(left,right,10);
    D_ENCRYPT(right,left,11);
    D_ENCRYPT(left,right,12);
    D_ENCRYPT(right,left,13);
    D_ENCRYPT(left,right,14);
    D_ENCRYPT(right,left,15);

    left  = ROTATE(left,3)&0xffffffffL;
    right = ROTATE(right,3)&0xffffffffL;

    FP(right, left);

    rs=(((uint64_t)right)<<32) | left;

    return rs;
}

__device__ uint64_t Key2Ciper_DES(uint64_t key)
{
    uint64_t         roundKeys[16];
    GenerateKey(key, roundKeys);
    key  = DESOneTime(roundKeys);
    return key;
}

__device__ uint64_t Cipher2Key_DES(uint64_t key, int nPos)
{
    key &= totalSpace_DES;
    if(nPos >= 1300)
    {
        key = (key + nPos) & totalSpace_DES;
        key = (key + (nPos << 8)) & totalSpace_DES;
        key = (key + ((nPos << 8) << 8)) & totalSpace_DES;
    }

    return key;
}

uint64_t Convert(uint64_t num, int time)
{
    assert(time < 8);

    uint64_t rs = 0, tmp = 0;

    for(int i = 0; i < time; i++)
    {
        tmp = num & ((1ull << 7) - 1);
        tmp <<= 1;
        tmp <<= (8*i);
        rs |= tmp;
        num >>= 7;
    }

    return rs;
}

__global__ void DESCUDA(uint64_t *data)
{
    for(int i=0; i < 256; i++)
        ((uint64_t *)des_SP)[i] = ((uint64_t *)des_d_sp_c)[i];

    __syncthreads();

    uint64_t key = data[TX];
    for(int nPos = 0; nPos < CHAINLEN; nPos++)
        key = Cipher2Key_DES(Key2Ciper_DES(key), nPos);
    data[TX] = key;

    __syncthreads();
}

__global__ void  DESCrackCUDA(uint64_t *data)
{
    for(int i=0; i < 256; i++)
        ((uint64_t *)des_SP)[i] = ((uint64_t *)des_d_sp_c)[i];

    __syncthreads();

    uint64_t ix  = TX, key = data[ix];
    for(int nPos = (ix % 4096) + 1; nPos < 4096; nPos++)
        key = Cipher2Key_DES(Key2Ciper_DES(key), nPos);
    data[ix] = key;

    ix = (1 << 19) - 1 - TX; key = data[ix];
    for(int nPos = (ix % 4096) + 1; nPos < 4096; nPos++)
        key = Cipher2Key_DES(Key2Ciper_DES(key), nPos);
    data[ix] = key;

    __syncthreads();
}

};

#endif
