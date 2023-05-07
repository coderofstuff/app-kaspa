# Fuzzing on transaction parser

## Compilation

In `fuzzing` folder

```
cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -Bbuild -H.
```

cmake -DCMAKE_C_COMPILER=/tmp/llvm-project/build/bin/clang \
-DCMAKE_CXX_COMPILER=/tmp/llvm-project/build/bin/clang++ \
-DGCC_INSTALL_PREFIX=/usr/lib/gcc/x86_64-alpine-linux-musl/10.3.1/ \
-Bbuild -H.

then

```
make -C build
```

## Run

```
./build/fuzz_tx_parser
```

## Extra installs

```
apk add build-base libc-dev linux-headers libexecinfo-dev llvm12 compiler-rt
```

## LLVM Compile

```
cd /tmp/llvm-project
mkdir build
cd build

cmake ../llvm/ -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/tmp/llvm-project/build/ -DBUILD_SHARED_LIBS=on -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" -DLLVM_ENABLE_BACKTRACES=OFF -DCOMPILER_RT_BUILD_GWP_ASAN=OFF

cp /tmp/llvm-project/build/lib/clang/15.0.0/lib/linux/libclang* /usr/lib/clang/12.0.1/lib/linux/
```