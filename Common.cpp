#include "Common.h"
#include <sys/sysinfo.h>

unsigned int GetFileLen(FILE* file)
{
    unsigned int pos = ftell(file);
    fseek(file, 0, SEEK_END);
    unsigned int len = ftell(file);
    fseek(file, pos, SEEK_SET);

    return len;
}

void Logo()
{
	printf("DESRainbowCuda 1.0 - Make an implementation of DES Time-and-Memory Tradeoff Technology\n");
	printf("by Tian Yulong(mathetian@gmail.com)\n");
}

unsigned int GetAvailPhysMemorySize()
{   
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram;
}

void U56ToArr7(const uint64_t & key56, unsigned char * key_56)
{	
	int mask = (1<<8) - 1;
	
	key_56[0] = key56 & mask;
	key_56[1] = (key56 >>  8) & mask;
	key_56[2] = (key56 >> 16) & mask;
	key_56[3] = (key56 >> 24) & mask;
	key_56[4] = (key56 >> 32) & mask;
	key_56[5] = (key56 >> 40) & mask;
	key_56[6] = (key56 >> 48) & mask;
}

void Arr7ToU56(const unsigned char * key_56, uint64_t & key56)
{
	int index; key56 = 0;
	for(index = 0;index < 7;index++)
		key56 |= (key_56[index] << (8*index));
}

void SetupDESKey(const uint64_t&key56,des_key_schedule &ks)
{
	des_cblock key, key_56;
	
	U56ToArr7(key56,key_56);

	key[0]=key_56[0];
	key[1]=(key_56[0]<<7)|(key_56[1]>>1);
	key[2]=(key_56[1]<<6)|(key_56[2]>>2);
	key[3]=(key_56[2]<<5)|(key_56[3]>>3);
	key[4]=(key_56[3]<<4)|(key_56[4]>>4);
	key[5]=(key_56[4]<<3)|(key_56[5]>>5);
	key[6]=(key_56[5]<<2)|(key_56[6]>>6);
	key[7]=(key_56[6<<1]);

	des_set_key(&key,ks);
}

SortedSegment::SortedSegment()
{
}

SortedSegment::~SortedSegment()
{
}

void SortedSegment::setProperty(int offset,int length,int curOffset)
{
	this -> offset 	  = offset;
	this -> length 	  = length;
	this -> curOffset = curOffset;
}

int SortedSegment::getLength()
{
	return length;
}

RainbowChain * SortedSegment::getFirst()
{
	fseek(file,offset+curOffset,SEEK_SET);
	fread(chains,sizeof(RainbowChain),1,file);
	return chains;
}

RainbowChain * SortedSegment::getAll()
{
	fseek(file,offset,SEEK_SET);
	fread(chains,sizeof(RainbowChain),length,file);
	return chains;
}

RainbowChain * SortedSegment::getNext()
{
	if(curOffset==length*sizeof(RainbowChain))
		return NULL;
	fread(chains,sizeof(RainbowChain),1,tmpFile);
	curOffset+=sizeof(RainbowChain);
}