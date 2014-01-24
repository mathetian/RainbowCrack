#include "Common.h"
#include "TimeStamp.h"
#include "ChainWalkContext.h"

#include <iostream>
using namespace std;

#include <assert.h>

void Usage()
{
	Logo();
	printf("Usage: generator chainLen chainCount suffix\n");
	printf("                 benchmark\n");
	printf("                 single startKey\n");
	printf("                 testrandom\n");
	printf("                 testnativerandom\n");
	printf("                 testkeyschedule\n");
	printf("                 testcasegenerator\n");

	printf("example 1: generator 1000 10000 suffix\n");
	printf("example 2: generator benchmark\n");
	printf("example 3: generator single 563109\n");
	printf("example 4: generator testrandom\n");
	printf("example 5: generator testnativerandom\n");
	printf("example 6: generator testkeyschedule\n");
	printf("example 7: generator testcasegenerator\n\n");
}

typedef long long ll;

void Benchmark()
{
	ChainWalkContext cwc;
	int index, nLoop = 1 << 21;	
	char str[256]; 
	memset(str, 0, sizeof(str));

	cwc.GetRandomKey();
	
	TimeStamp::StartTime();

	for(index = 0;index < nLoop;index++) 
		cwc.KeyToCipher();

	sprintf(str, "Benchmark: nLoop %d: keyToHash time:", nLoop);

	TimeStamp::StopTime(str);
	
	cwc.GetRandomKey();

	TimeStamp::StartTime();

	for(index = 0;index < nLoop;index++)
	{
		cwc.KeyToCipher();
		cwc.KeyReduction(index);
	}

	sprintf(str, "Benchmark: nLoop %d: total time:    ", nLoop);
	TimeStamp::StopTime(str);
}

void Single(int startKey)
{
	ChainWalkContext cwc; int index;
	cwc.SetKey(startKey); uint64_t key = cwc.GetKey();
	fwrite((char*)&key,sizeof(uint64_t),1,stdout);
	fflush(stdout);
	for(index = 0;index < 1024;index++)
	{
		cwc.KeyToCipher();
		cwc.KeyReduction(index);
		key = cwc.GetKey();
		fwrite((char*)&key,sizeof(uint64_t),1,stdout);
		fflush(stdout);
	}
}

void TestRandom()
{
	ChainWalkContext cwc; RainbowChain chain;
	
	FILE * file;

	if((file = fopen("TestRandom.txt","w")) == NULL)
	{
		fprintf(stderr,"TestRandom.txt open error\n");
		return;
	}
	
	printf("Begin TestRandom\n");
	
	for(int index = 0;index < (1 << 20);index++)
	{
		chain.nStartKey = cwc.GetRandomKey();
		chain.nEndKey   = cwc.Crypt(chain.nStartKey);
		fwrite((char*)&chain,sizeof(RainbowChain),1,file);
	}
	
	printf("End TestRandom\n");

	fclose(file);
}

int get(unsigned char cc)
{
	int a = cc; int f = 0;
	while(a)
	{
		if((a & 1) == 1)
			f++;
		a >>= 1;
	}
	if(f % 2 == 1)
		return 0;
	return 1;
}

void clear(unsigned char * key, int type)
{
	if(type ==  20)
	{
		key[0] &= ((1<<8) - 2);
		key[0] |= get(key[0]);
		key[1] &= ((1<<8) - 2);
		key[1] |= get(key[1]);
		key[2] &= ((1<<8) - 4);
		key[2] |= get(key[2]);

		for(int index = 3;index < 8;index++)
			key[index] = 1;
	}
	else if(type == 24)
	{
		for(int index = 3;index < 8;index++)
			key[index] = 0;
	}
	else if(type == 26)
	{
		//unsigned char rkey[8];
	}
	else if(type == 28)
	{
		/*int index   = 3;
		key[index] &= 15;
		for(index++;index < 8;index++)
			key[index] = 0;*/
	}
}

unsigned char plainText[8] = {0x6B,0x05,0x6E,0x18,0x75,0x9F,0x5C,0xCA};

void Generate(unsigned char * key, int type)
{
	if(type == 20)
	{
		int rr = rand() % (1 << 20);
		int ff = (1 << 7) - 1;
		key[0] = (rr & ff) << 1;
		rr >>= 7;
		key[1] = (rr & ff) << 1;
		rr >>= 7;
		key[2] = (rr) << 2;
		int index = 3;
		for(;index < 8;index++)
			key[index] = 0;
	}
}

void TestNativeRandom()
{
	unsigned char key[8], out[8]; des_key_schedule ks;
	RainbowChain chain; FILE * file;

	if((file = fopen("TestNativeRandom.txt","w")) == NULL)
	{
		printf("TestNativeRandom open error\n");
		return;
	}
	
	int type = 20; srand((uint32_t)time(0));
	for(int index = 0;index < (1<<10);index++)
	{
		Generate(key,20);
		chain.nStartKey = *(uint64_t*)key; 

		memset(out, 0, 8);

		DES_set_key_unchecked(&key, &ks);
		des_ecb_encrypt(&plainText,&out,ks,DES_ENCRYPT);

		clear(out, type);
		chain.nEndKey = *(uint64_t*)out;
		fwrite((char*)&chain, sizeof(RainbowChain), 1, file);

		if(index % 1000000 == 0)
			cout << index << endl;
	}

	fclose(file);
}

unsigned char keyData  [8] = {0x01,0x70,0xF1,0x75,0x46,0x8F,0xB5,0xE6};

void TestKeySchedule()
{
	des_key_schedule ks; FILE * file; int index = 0;
	DES_set_key_unchecked(&keyData, &ks);
	if((file = fopen("TestKeySchedule.txt","wb")) == NULL)
	{
		printf("TestKeySchedule fopen error\n");
		return;
	}
	
	for(;index < 16;index++)
	{
		fwrite(ks.ks[index].cblock,8,1,file);
	}
	fclose(file);
}

void TestCaseGenerator()
{
	FILE * file; RainbowChain chain;
	ChainWalkContext cwc;
	srand((uint32_t)time(0));

	file = fopen("TestCaseGenerator.txt","w");
	
	assert(file && "TestCaseGenerator fopen error\n");

	for(int index = 0;index < 100;index++)
	{
		chain.nStartKey = cwc.GetRandomKey();
		chain.nEndKey   = cwc.Crypt(chain.nStartKey);
		fwrite((char*)&chain, sizeof(RainbowChain), 1, file);
	}

	fclose(file);
}

int main(int argc,char*argv[])
{
	long long chainLen, chainCount, index;
	char suffix[256], szFileName[256];

	FILE * file; ChainWalkContext cwc;
	uint64_t nDatalen, nChainStart;
	RainbowChain chain;
		
	char str[256];

	if(argc == 2)
	{
		if(strcmp(argv[1],"benchmark") == 0)
			Benchmark();
		else if(strcmp(argv[1],"testrandom") == 0)
			TestRandom();
		else if(strcmp(argv[1],"testnativerandom") == 0)
			TestNativeRandom();
		else if(strcmp(argv[1],"testkeyschedule") == 0)
			TestKeySchedule();
		else if(strcmp(argv[1],"testcasegenerator") == 0)
			TestCaseGenerator();
		else  Usage();
		return 0;
	}
	else if(argc == 3)
	{
		if(strcmp(argv[1],"single") == 0)
			Single(atoll(argv[2]));
		else Usage();
		return 0;
	}

	if(argc != 4)
	{
		Usage();
		return 0;
	}
	
	chainLen   = atoll(argv[1]);
	chainCount = atoll(argv[2]);

	memcpy(suffix, argv[3], sizeof(argv[3]));
	sprintf(szFileName,"DES_%lld-%lld_%s", chainLen, chainCount,suffix);
	
	if((file = fopen(szFileName,"a+")) == NULL)
	{
		printf("failed to create %s\n",szFileName);
		return 0;
	}

	nDatalen = GetFileLen(file);
	nDatalen = (nDatalen >> 4) << 4;

	if(nDatalen == (chainCount << 4))
	{
		printf("precompute has finised\n");
		return 0;
	}

	if(nDatalen > 0)
	{
		printf("continuing from interrupted precomputing\n");
		printf("have computed %lld chains\n", (ll)(nDatalen >> 4));
	} 
	
	fseek(file, (long)nDatalen, SEEK_SET);
	nChainStart = (nDatalen >> 4);

	index = nDatalen >> 4;

	cwc.SetChainInfo(chainLen, chainCount);
	
	TimeStamp::StartTime();

	for(;index < chainCount;index++)
	{
		chain.nStartKey = cwc.GetRandomKey();
		int nPos;
		for(nPos = 0;nPos < chainLen;nPos++)
		{
			cwc.KeyToCipher();
			cwc.KeyReduction(nPos);
		}

		chain.nEndKey = cwc.GetKey();
		if(fwrite((char*)&chain, sizeof(RainbowChain), 1, file) != 1)
		{
			printf("disk write error\n");
			break;
		}

		if((index + 1)%10000 == 0||index + 1 == chainCount)
		{
			sprintf(str,"Generate: nChains: %lld, chainLen: %lld: total time:", index, chainLen);
			TimeStamp::StopTime(str);
			TimeStamp::StartTime();
		}
	}
	fclose(file);
	return 0;
}