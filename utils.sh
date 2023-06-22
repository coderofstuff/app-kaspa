#!/bin/bash

set -e

COMMAND=$1
PARAM1=$2
PARAM2=$3

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

PARAM_SCP=""

# If you've set up a keypair for your device, you can run
# SCP_PRIVKEY=<yourkeypair_privkey> bash utils.sh load nanosp
if [ "$SCP_PRIVKEY" ]; then
    PARAM_SCP="--rootPrivateKey $SCP_PRIVKEY"
fi

echo "$PARAM_SCP"

if [ -z "$COMMAND" ]; then
    echo "Usage: ./utils.sh command [param1] [param2]"
    echo "Valid commands and params:"
    echo "- load => [nanos, nanosp]"
    echo "- build => [nanos, nanosp, nanox, stax]"
    echo "- init-emulator => [nanos, nanosp, nanox, stax], \"SEED_PHRASE\""
    echo "- init-venv"
    echo "- init-companion"
    return 1
fi

function build_app() {
    DEVICE=$1
    BUILD_COMMAND=""

    if [ "$DEVICE" == "nanos" ]; then
        BUILD_COMMAND="BOLOS_SDK=\$NANOS_SDK make"
    elif [ "$DEVICE" == "nanosp" ]; then
        BUILD_COMMAND="BOLOS_SDK=\$NANOSP_SDK make"
        echo "$DEVICE"
    elif [ "$DEVICE" == "nanox" ]; then
        BUILD_COMMAND="BOLOS_SDK=\$NANOX_SDK make"
    elif [ "$DEVICE" == "stax" ]; then
        BUILD_COMMAND="BOLOS_SDK=\$STAX_SDK make"
    else
        echo "Unsupported device $DEVICE"
        return 1
    fi

    docker run \
        --rm \
        -v $(realpath .):/app \
        --privileged \
        --entrypoint bash ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest \
        -c "$BUILD_COMMAND"

}

function init_companion() {
    if [ -d "kaspa-ledger-webapp" ]; then
        echo "kaspa-ledger-webapp already exists $(pwd)"
    else
        git clone https://github.com/coderofstuff/kaspa-ledger-webapp.git
    fi

    cd kaspa-ledger-webapp

    npm install
    npm run start
}

function init_emulator() {
    DEVICE=$1
    SEED_PHRASE=$2

    if [ "$DEVICE" != "nanos" ] && [ "$DEVICE" != "nanosp" ] && [ "$DEVICE" != "nanox" ] && [ "$DEVICE" != "stax" ]; then
        echo "Unsupported device $DEVICE"
        return 1
    fi

    docker run --rm -it -v $(pwd)/bin:/speculos/apps -p 5000:5000 ghcr.io/ledgerhq/speculos:latest \
        --model $DEVICE ./apps/app.elf \
        --display headless \
        --seed "$SEED_PHRASE"
}

function init_virtualenv() {
    if [ -z "$VIRTUAL_ENV" ]; then
        if [ -d "ledger" ]; then
            echo "ledger virtualenv already exists"
        else
            virtualenv ledger
        fi
        source ledger/bin/activate
    else
        echo "Already in virtualenv: $VIRTUAL_ENV"
    fi

    pip install --extra-index-url https://test.pypi.org/simple/ -r tests/requirements.txt
    pip install ledgerblue
}

function load_to_device() {
    if [ -z "$VIRTUAL_ENV" ]; then
        init_virtualenv
    fi

    DEVICE=$1
    DATA_SIZE=$((0x`cat debug/app.map |grep _envram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'` - 0x`cat debug/app.map |grep _nvram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'`))

    build_app $DEVICE

    if [ "$DEVICE" == "nanosp" ]; then
        python3 -m ledgerblue.loadApp \
            --curve secp256k1 \
            --appFlags 0x000 \
            --path "44'/111111'" \
            --tlv \
            --targetId 0x33100004 \
            --fileName bin/app.hex \
            --appName Kaspa \
            --appVersion 1.0.0 \
            --dataSize $DATA_SIZE \
            --icon "0100000000ffffff000778001830e21c7807fc017e801ff007de8173048c01e0010e" \
            --apiLevel 1 \
            --delete \
            $PARAM_SCP
    elif [ "$DEVICE" == "nanos" ]; then
        python3 -m ledgerblue.loadApp \
            --curve secp256k1 \
            --appFlags 0x000 \
            --path "44'/111111'" \
            --tlv \
            --targetId 0x31100004 \
            --fileName bin/app.hex \
            --appName Kaspa \
            --appVersion 1.0.0 \
            --dataSize $DATA_SIZE \
            --icon "0100000000ffffff000000f00ffc3ffc398e710e711e703e703e701e700e718e71fc39fc3ff00f0000" \
            --delete \
            $PARAM_SCP
    else
        echo "No operation executed. Unsupported device: $DEVICE"
        echo "Usage: ./utils.sh load [nanos|nanosp]"
        exit 1
    fi
}

pushd $SCRIPT_DIR

if [ "$COMMAND" == "load" ]; then
    load_to_device $PARAM1
elif [ "$COMMAND" == "build" ]; then
    build_app $PARAM1
elif [ "$COMMAND" == "init-venv" ]; then
    init_virtualenv
elif [ "$COMMAND" == "init-emulator" ]; then
    init_emulator $PARAM1 $PARAM2
elif [ "$COMMAND" == "init-companion" ]; then
    popd
    init_companion
    pushd $SCRIPT_DIR
else
    echo "Unsupported command"
fi

popd