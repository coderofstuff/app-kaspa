FROM ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest

COPY ./tests/requirements.txt /tmp/requirements.txt

RUN apk update
RUN apk add python3-dev build-base libc-dev linux-headers libexecinfo-dev llvm12 compiler-rt
RUN pip install --extra-index-url https://test.pypi.org/simple/ -r /tmp/requirements.txt
