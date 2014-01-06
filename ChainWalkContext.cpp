#include "ChainWalkContext.h"

uint64_t   ChainWalkContext::m_plainText     = 0x305532286D6F295A;
/*uint64_t   ChainWalkContext::m_keySpaceTotal = (1ull << 56) - 1;*/
uint64_t   ChainWalkContext::m_keySpaceTotal = (1ull << 20) - 1;
uint64_t   ChainWalkContext::m_chainLen;
uint64_t   ChainWalkContext::m_chainCount;
unsigned char ChainWalkContext::m_dplainText[8] = {0x30,0x55,0x32,0x28,0x6D,0x6F,0x29,0x5A};

ChainWalkContext::ChainWalkContext()
{
}

ChainWalkContext::~ChainWalkContext()
{
}

void ChainWalkContext::SetChainInfo(uint64_t chainLen, uint64_t chainCount)
{
	m_chainLen   = chainLen;
	m_chainCount = chainCount;
}

uint64_t ChainWalkContext::GetRandomKey()
{
	RAND_bytes((unsigned char*)&m_nIndex,8);
	m_nIndex = m_nIndex & m_keySpaceTotal;
	return m_nIndex;
}

/**
    des_cblock: typedef unsigned char DES_cblock[8];
**/
/**
typedef struct DES_ks
{
    union
	{
		DES_cblock cblock;
		DES_LONG deslong[2];
	} ks[16];
} DES_key_schedule;
DES_LONG is 'unsigned int'
**/

void ChainWalkContext::KeyToCipher()
{
	des_key_schedule ks;unsigned char out[8];
	SetupDESKey(m_nIndex,ks); memset(out,0,8);
	des_ecb_encrypt(&m_dplainText,&out,ks,DES_ENCRYPT);
	CipherToKey(out);
}

void ChainWalkContext::CipherToKey(unsigned char * out)
{
	Arr7ToU56(out, m_nIndex);
}

/**
	Still exist the same problem
**/
void ChainWalkContext::KeyReduction(int nPos)
{
	m_nIndex = (m_nIndex + nPos) & m_keySpaceTotal;
}

uint64_t ChainWalkContext::GetKey()
{
	return m_nIndex;
}

void 	 ChainWalkContext::SetKey(uint64_t key)
{
	m_nIndex = key;
}

uint64_t ChainWalkContext::Crypt(uint64_t key)
{
	des_key_schedule ks;unsigned char out[8];
	SetupDESKey(key,ks); memset(out,0,8);
	des_ecb_encrypt(&m_dplainText,&out,ks,DES_ENCRYPT);
	Arr7ToU56(out, key);
	return key;
}