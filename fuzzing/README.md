# Fuzzing on transaction parser

## Setup

Change `~/ledger/app-kaspa` to wherever you actual `app-kaspa` folder is.

```
docker run --rm -it -v ~/ledger/app-kaspa:/app ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest bash
apk add build-base libc-dev linux-headers libexecinfo-dev compiler-rt
apk del llvm15

mkdir -p /usr/lib/clang/12.0.1/lib/linux/

cp /app/fuzzing/llvm-headers/libclang_rt.asan-x86_64.a /usr/lib/clang/12.0.1/lib/linux/libclang_rt.asan-x86_64.a
cp /app/fuzzing/llvm-headers/libclang_rt.fuzzer-x86_64.a /usr/lib/clang/12.0.1/lib/linux/libclang_rt.fuzzer-x86_64.a
```

## Compilation


In `fuzzing` folder

```
cmake -DBOLOS_SDK=$NANOSP_SDK -DCMAKE_C_COMPILER=/usr/bin/clang -Bbuild -H.
```

then

```
make -C build
```

## Run

```
./build/fuzz_tx_parser
./build/fuzz_txin_parser
./build/fuzz_txout_parser
```

## LLVM Compile

Use this if you want to build the LLVM from scratch.

A pre-compiled version of the ones this fuzzing needs is in `llvm-headers`.

Update your docker run to:
```
docker run -it -v ~/ledger/app-kaspa:/app -v ~/llvm-project:/tmp/llvm-project ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest bash
```

Clone the LLVM project from: https://github.com/llvm/llvm-project
```
git clone https://github.com/llvm/llvm-project ~/llvm-project
```

```
cd /tmp/llvm-project
mkdir build
cd build

cmake ../llvm/ -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/tmp/llvm-project/build/ -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=on -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" -DLLVM_ENABLE_BACKTRACES=OFF -DCOMPILER_RT_BUILD_GWP_ASAN=OFF 

make -j8

cp /tmp/llvm-project/build/lib/clang/16/lib/x86_64-unknown-linux-gnu/libclang_rt.asan.a /usr/lib/clang/12.0.1/lib/linux/libclang_rt.asan-x86_64.a
cp /tmp/llvm-project/build/lib/clang/16/lib/x86_64-unknown-linux-gnu/libclang_rt.fuzzer.a /usr/lib/clang/12.0.1/lib/linux/libclang_rt.fuzzer-x86_64.a
```
