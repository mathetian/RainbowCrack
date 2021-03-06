// Copyright (c) 2014 The DESCrack Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "TimeStamp.h"
using namespace utils;

#include "DESCrackEngine.h"

namespace descrack
{

MemoryPool DESCrackEngine::mp;

DESCrackEngine::DESCrackEngine() : m_totalChains(0), m_falseAlarms(0)
{
    m_diskTime.tv_sec   = 0;
    m_diskTime.tv_usec  = 0;
    m_totalTime.tv_sec  = 0;
    m_totalTime.tv_usec = 0;
    p_cs = DESCipherSet::GetInstance();
}

uint64_t DESCrackEngine::BinarySearch(RainbowChain * pChain, uint64_t pChainCount, uint64_t nIndex)
{
    uint64_t low = 0, high = pChainCount;

    while(low < high)
    {
        uint64_t mid = (low+high)/2;

        if(pChain[mid].nEndKey == nIndex)
            return mid;
        else if(pChain[mid].nEndKey < nIndex)
            low = mid + 1;
        else
            high=mid;
    }

    return low;
}

void DESCrackEngine::GetIndexRange(RainbowChain * pChain,uint64_t pChainCount, uint64_t nChainIndex, uint64_t &nChainIndexFrom, uint64_t &nChainIndexTo)
{
    nChainIndexFrom = nChainIndex;
    nChainIndexTo   = nChainIndex;

    while(nChainIndexFrom>0)
    {
        if(pChain[nChainIndexFrom - 1].nEndKey == pChain[nChainIndex].nEndKey)
            nChainIndexFrom--;
        else
            break;
    }

    while(nChainIndexTo < pChainCount)
    {
        if(pChain[nChainIndexTo+1].nEndKey == pChain[nChainIndex].nEndKey)
            nChainIndexTo++;
        else
            break;
    }
}

bool DESCrackEngine::CheckAlarm(RainbowChain *pChain, uint64_t nGuessPos, uint64_t testV)
{
    DESChainWalkContext cwc;

    uint64_t nPos = 0, old = pChain -> nStartKey;

    cwc.SetKey(pChain -> nStartKey);

    for(; nPos <= nGuessPos; nPos++)
    {
        old = cwc.GetKey();
        cwc.KeyToCipher();
        cwc.KeyReduction(nPos);
    }

    if(cwc.GetKey() == pVerified[nGuessPos])
    {
        printf("plaintext of %lld is %lld\n", (long long)cwc.GetKey(), (long long)old);
        p_cs -> AddResult(p_cs -> GetLeftKey(), old);

        return true;
    }

    return false;
}

void DESCrackEngine::SearchRainbowTable(const char *fileName)
{
    char str[256];

    uint64_t fileLen, nAllocateSize, nDataRead;
    FILE *file;
    RainbowChain *pChain;

    if((file = fopen(fileName,"rb")) == NULL)
    {
        printf("SearchRainbowTable: fopen error\n");
        return;
    }

    fileLen = GetFileLen(file);

    assert(fileLen % 16 == 0);

    cout << DESChainWalkContext::m_chainCount << " " << fileLen << endl;

    if(fileLen % 16 != 0 || DESChainWalkContext::m_chainCount*16 != fileLen)
    {
        printf("file length check error\n");
        return;
    }

    if((pChain = (RainbowChain*)mp.Allocate(fileLen, nAllocateSize)) == NULL)
    {
        printf("SearchRainbowTable: allocate error\n");
        return;
    }

    nAllocateSize = nAllocateSize / 16 * 16;
    fseek(file, 0, SEEK_SET);

    while(true)
    {
        if(fileLen == (uint64_t)ftell(file)) break;

        TimeStamp tmps;
        tmps.StartTime();

        nDataRead = fread(pChain, 1, nAllocateSize, file);

        if(nDataRead != nAllocateSize)
        {
            printf("Warning nDataRead: %lld, nAllocateSize: %lld\n", (long long)nDataRead, (long long)nAllocateSize);
        }

        sprintf(str,"%lld bytes read, disk access time:", (long long)nAllocateSize);

        tmps.StopTime(str);
        tmps.AddTime(m_diskTime);

        tmps.StartTime();

        SearchTableChunk(pChain, nDataRead >> 4);

        sprintf(str,"cryptanalysis time: ");

        tmps.StopTime(str);
        tmps.AddTime(m_totalTime);

        if(p_cs -> Solved()) break;
    }

    p_cs -> Done(p_cs->GetLeftKey());

    fclose(file);
}

void DESCrackEngine::SearchTableChunk(RainbowChain *pChain, int pChainCount)
{
    uint64_t nFalseAlarm, nIndex, nGuessPos;
    uint64_t key = p_cs -> GetLeftKey();

    printf("Searching for key: %lld...\n", (long long)key);

    nFalseAlarm  = 0;

    for(nGuessPos = 0; nGuessPos < DESChainWalkContext::m_chainLen; nGuessPos++)
    {
        uint64_t nMathingIndexE = BinarySearch(pChain, pChainCount, pEndKeys[nGuessPos]);

        if(pChain[nMathingIndexE].nEndKey == pEndKeys[nGuessPos])
        {
            uint64_t nMathingIndexEFrom, nMathingIndexETo;
            GetIndexRange(pChain, pChainCount, nMathingIndexE,nMathingIndexEFrom,nMathingIndexETo);

            for(nIndex = nMathingIndexEFrom; nIndex <= nMathingIndexETo; nIndex++)
            {
                if(CheckAlarm(pChain + nIndex, nGuessPos, pEndKeys[nGuessPos]))
                    goto NEXT_HASH;
                else
                    nFalseAlarm++;
            }
        }

        if(nGuessPos % 100 == 0) printf("nGuessPos %lld\n", (long long)nGuessPos);
    }
NEXT_HASH:
    ;
    m_totalChains += pChainCount;
    m_falseAlarms += nFalseAlarm;
}

void DESCrackEngine::Run(const char *fileName)
{
    uint64_t nChainLen, nChainCount;

    if(AnylysisFileName(fileName, nChainLen, nChainCount) == false)
    {
        printf("fileName format error\n");
        return;
    }

    printf("\nnChainLen: %lld, nChainCount: %lld\n", (long long)nChainLen, (long long)nChainCount);

    DESChainWalkContext::SetChainInfo(nChainLen, nChainCount);

    int index = 0;

    while(p_cs -> AnyKeyLeft())
    {
        printf("-------------------------------------------------------\n");
        printf("Time: %d, key: %lld\n\n",index++,(long long)p_cs -> GetLeftKey());

        TimeStamp tmps;
        tmps.StartTime();

        InitEndKeys(p_cs -> GetLeftKey());

        tmps.StopTime("Init Time: ");

        SearchRainbowTable(fileName);

        printf("-------------------------------------------------------\n");
    }
}

struct timeval DESCrackEngine::GetDiskTime()
{
    return m_diskTime;
}

struct timeval DESCrackEngine::GetTotalTime()
{
    return m_totalTime;
}

uint64_t DESCrackEngine::GetTotalChains()
{
    return m_totalChains;
}

uint64_t DESCrackEngine::GetFalseAlarms()
{
    return m_falseAlarms;
}

void DESCrackEngine::InitEndKeys(uint64_t key)
{
    pEndKeys  = vector<uint64_t>(DESChainWalkContext::m_chainLen, 0);
    pVerified = vector<uint64_t>(DESChainWalkContext::m_chainLen, 0);

    for(uint32_t nGuessPos = 0; nGuessPos < DESChainWalkContext::m_chainLen; nGuessPos++)
    {
        m_cwc.SetKey(key);
        m_cwc.KeyReduction(nGuessPos);

        pVerified[nGuessPos] = m_cwc.GetKey();

        for(uint32_t nIndex = nGuessPos + 1; nIndex < DESChainWalkContext::m_chainLen; nIndex++)
        {
            m_cwc.KeyToCipher();
            m_cwc.KeyReduction(nIndex);
        }

        pEndKeys[nGuessPos] = m_cwc.GetKey();
    }
}

};