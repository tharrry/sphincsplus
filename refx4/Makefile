PARAMS = sphincs-sha256-128f
THASH = robust

CC=/usr/bin/gcc
CFLAGS=-Wall -Wextra -Wpedantic -O3 -std=c99 -DPARAMS=$(PARAMS) $(EXTRA_CFLAGS) -flax-vector-conversions -march=native#+nosimd #-fsanitize=address -g

SOURCES =          address.c randombytes.c merkle.c wots.c          utils.c utilsx4.c fors.c sign.c #utilsx1.c wotsx1.c
HEADERS = params.h address.h randombytes.h merkle.h wots.h wotsx4.h utils.h utilsx4.h fors.h api.h  hash.h thash.h  hashx4.h thashx4.h #wotsx1.h utilsx1.h

ifneq (,$(findstring sha256,$(PARAMS)))
	SOURCES += sha256.c hash_sha256.c hash_sha256x4.c thash_sha256_$(THASH).c  thash_sha256_$(THASH)x4.c
	HEADERS += sha256.h
endif

DET_SOURCES = $(SOURCES:randombytes.%=rng.%)
DET_HEADERS = $(HEADERS:randombytes.%=rng.%)

TESTS =         test/fors \
		test/spx \

BENCHMARK = test/benchmark

.PHONY: clean test benchmark

default: PQCgenKAT_sign

all: PQCgenKAT_sign tests benchmarks

tests: $(TESTS)

test: $(TESTS:=.exec)

benchmarks: $(BENCHMARK)

benchmark: $(BENCHMARK:=.exec)

PQCgenKAT_sign: PQCgenKAT_sign.c $(DET_SOURCES) $(DET_HEADERS)
	$(CC) $(CFLAGS) -o $@ $(DET_SOURCES) $< -lcrypto

test/%: test/%.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $< $(LDLIBS)

test/%.exec: test/%
	@$<

clean:
	-$(RM) $(TESTS)
	-$(RM) $(BENCHMARK)
	-$(RM) PQCgenKAT_sign
	-$(RM) PQCsignKAT_*.rsp
	-$(RM) PQCsignKAT_*.req
