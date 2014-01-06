PROGS = generator verified sort crack test

LIB = -lrt -lssl -lcrypto -ldl -O0 -g

all: ${PROGS}

generator: Generate.cpp Common.cpp ChainWalkContext.cpp
	g++ $^ -o $@ ${LIB} 

verified: Verified.cpp Common.cpp ChainWalkContext.cpp
	g++ $^ -o $@ ${LIB}

sort: SortPreCalculate.cpp Common.cpp
	g++ $^ -o $@ ${LIB}
	
crack: DESCrack.cpp Common.cpp ChainWalkContext.cpp CipherSet.cpp CrackEngine.cpp MemoryPool.cpp
	g++ $^ -o $@ ${LIB}

test: Eest.cpp ChainWalkContext.cpp Common.cpp
	g++ $^ -o $@ ${LIB}

test2: Eest2.cpp Common.cpp
	g++ $^ -o $@ ${LIB}
	
clean:
	rm -f ${PROGS} DES_*