#!/bin/bash

# Works only inside the container
cd /app/unit-tests

cmake -Bbuild -H. && make -C build
CTEST_OUTPUT_ON_FAILURE=1 make -C build test

cd /app