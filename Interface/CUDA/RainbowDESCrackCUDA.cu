// Copyright (c) 2014 The RainbowCrack Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "RainbowDESCUDA.h"
#include "RainbowCipherSet.h"
using namespace rainbowcrack;

void Usage()
{
    Logo();
    printf("Usage: descrackdua file chainLen encryptedFile \n\n");

    printf("example 1: descrackdua file chainLen encryptedFile\n\n");
}

__global__ void  DESGeneratorCUDA(uint64_t *data)
{
    for(int i=0; i<256; i++)
        ((uint64_t *)des_SP)[i] = ((uint64_t *)des_d_sp_c)[i];

    __syncthreads();

    uint64_t tx = TX / 4096;
    uint64_t st = tx*4096*2;
    uint64_t ix = st + (TX % 4096);

    register uint64_t m_nIndex = data[ix];

    uint64_t roundKeys[16];

    for(int nPos = (TX % 4096) + 1; nPos < 4096; nPos++)
    {
        GenerateKey(m_nIndex, roundKeys);
        m_nIndex  = DESOneTime(roundKeys);
        m_nIndex &= totalSpace;

        int nnpos = nPos;
        if(nPos < 1300) nnpos = 0;
        m_nIndex = (m_nIndex + nnpos) & totalSpace;
        m_nIndex = (m_nIndex + (nnpos << 8)) & totalSpace;
        m_nIndex = (m_nIndex + ((nnpos << 8) << 8)) & totalSpace;
    }

    data[ix] = m_nIndex;

    ix = st + 2*4096 - TX % 4096 - 1;
    st = st + 4096;

    m_nIndex = data[ix];

    for(int nPos = 4096 - (TX % 4096); nPos < 4096; nPos++)
    {
        GenerateKey(m_nIndex, roundKeys);
        m_nIndex  = DESOneTime(roundKeys);
        m_nIndex &= totalSpace;

        int nnpos = nPos;
        if(nPos < 1300) nnpos = 0;
        m_nIndex = (m_nIndex + nnpos) & totalSpace;
        m_nIndex = (m_nIndex + (nnpos << 8)) & totalSpace;
        m_nIndex = (m_nIndex + ((nnpos << 8) << 8)) & totalSpace;
    }

    data[ix] = m_nIndex;

    __syncthreads();
}

void REGenerator(RainbowCipherSet *p_cs, uint64_t chainLen)
{
    assert(chainLen == 4096);

    uint64_t starts[ALL*2], ends[ALL*2];

    uint64_t *cudaIn;
    int size = ALL*sizeof(uint64_t)*2;
    _CUDA(cudaMalloc((void**)&cudaIn , size));

    FILE *result = fopen("Result.txt", "wb+");
    assert(result);

    while(p_cs -> GetRemainCount() < 128)
    {
        uint64_t keys[128];

        for(int i = 0; i < 128; i++)
        {
            keys[i] = p_cs -> GetLastKey();
            p_cs -> Done();
        }

        for(int i = 0; i < 128; i++)
        {
            for(uint64_t nPos = 0; nPos < chainLen ; nPos++)
            {
                starts[i*chainLen + nPos] = (keys[i] & totalSpaceT);
                int nnpos = nPos;

                if(nPos < 1300) nnpos = 0;
                starts[i*chainLen + nnpos] = (starts[i*chainLen + nnpos] + nnpos) & totalSpaceT;
                starts[i*chainLen + nnpos] = (starts[i*chainLen + nnpos] + (nnpos << 8)) & totalSpaceT;
                starts[i*chainLen + nnpos] = (starts[i*chainLen + nnpos] + ((nnpos << 8) << 8)) & totalSpaceT;
            }
        }

        _CUDA(cudaMemcpy(cudaIn, starts, size, cudaMemcpyHostToDevice));

        DESGeneratorCUDA<<<BLOCK_LENGTH, MAX_THREAD>>>(cudaIn);

        _CUDA(cudaMemcpy(ends, cudaIn, size, cudaMemcpyDeviceToHost));

        for(int i = 0; i < 128; i++)
        {
            for(uint64_t pos = 0; pos < chainLen ; pos++)
            {
                int flag1 = fwrite((char*)&(ends[i*chainLen + pos]), sizeof(uint64_t), 1, result);
                assert(flag1 == 1);
            }
        }
    }

    fclose(result);
}

int main(int argc, char *argv[])
{
    uint64_t chainLen;
    RainbowCipherSet  *p_cs = RainbowCipherSet::GetInstance();

    if(argc != 4 || strcmp(argv[1],"file") != 0)
    {
        Usage();
        return 0;
    }

    chainLen = atoll(argv[2]);
    FILE *file = fopen(argv[3], "rb");
    assert(file);

    RainbowChain chain;
    while(fread((char*)&chain, sizeof(RainbowChain), 1, file))
        p_cs -> AddKey(chain.nEndKey);

    REGenerator(p_cs, chainLen);
    fclose(file);

    return 0;
}