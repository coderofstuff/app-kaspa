#!/bin/bash -eu

# build fuzzers

pushd fuzzing
cmake -DBOLOS_SDK=../BOLOS_SDK -Bbuild -H.
make -C build
mv ./build/fuzz_tx_parser $OUT
mv ./build/fuzz_txin_parser $OUT
mv ./build/fuzz_txout_parser $OUT
popd