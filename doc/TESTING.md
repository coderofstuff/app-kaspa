# Instruction for locally testing with UI and Emulator

## Requirements
- Docker
- NodeJS (16+)

## Initial setup
1. Create a new directory under which you will clone all the relevant projects
```
mkdir kaspa-ledger
```
2. Change into that directory. You will use this as the base:
```
cd kaspa-ledger
```

## Setting up the Companion app UI
1. Clone the repo `https://github.com/coderofstuff/kaspa-ledger-webapp.git` and install dependencies then run the UI:
```
git clone https://github.com/coderofstuff/kaspa-ledger-webapp.git
cd kaspa-ledger-webapp

npm install
npm run start
```

## Compiling Embedded app
1. Go back to the base folder, clone the `app-kaspa` repo and `cd` into it
```
cd .. // To go back to 
git clone https://github.com/coderofstuff/app-kaspa.git
cd app-kaspa
```

4. Pull the Ledger official builder image and docker run it:
```
docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
docker run --rm -it -v $(realpath .):/app --privileged ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
```

5. You should now be in a terminal inside the container. Compile the embedded app from within the container:
```
BOLOS_SDK=$NANOSP_SDK make DEBUG=1
```

After it compiles, exit the container:
```
exit
```

6.  You now have a built `app.elf` ready for use by speculos in `app-kaspa/bin/app.elf`.

## Setup the emulator
1. While in the directory `kaspa-ledger/app-kaspa`, docker pull the Ledger official speculos image:
```
docker pull ghcr.io/ledgerhq/speculos:latest
```

3. [Highly Recommended] Generate a completely new 24-word seed with https://iancoleman.io/bip39/#english

4. Change the `CHANGE_THIS_TO_SEED_FROM_2` text below. `cd` in the `app-kaspa` directory and run:
```
docker run --rm -it -v $(pwd)/bin:/speculos/apps -p 5000:5000 ghcr.io/ledgerhq/speculos:latest --model nanosp ./apps/app.elf --display headless --seed "CHANGE_THIS_TO_SEED_FROM_2"
```

5. Your emulator is now running and is accessible at `http://127.0.0.1:5000`

## End state
1. You have the UI running in `http://127.0.0.1:1234`
2. You have compiled the embedded app
3. You have the emulator that loads the compiled app, running in `http://127.0.0.1:5000`
4. You are now ready to test!
5. Check back into this wiki regularly for any updates.

## Reporting issues
- Embedded app issues: https://github.com/coderofstuff/app-kaspa
- Companion app UI issues: https://github.com/lAmeR1/kaspa-ledger-webapp/issues

## Installing on a Ledger Device

### Required
- Python3 installation
- Python `virtualenv` installed
- Make sure you have the latest firmware for your device by updating using Ledger Live

For Linux:
```
sudo apt-get install python3-pip
pip3 install virtualenv
```

For Mac:
```
// Assuming you have homebrew
brew install python3
pip3 install virtualenv
```

### Reference

The complete instructions can be found in:
- Mac: https://developers.ledger.com/docs/embedded-app/load-mac/
- Linux/WSL2: https://developers.ledger.com/docs/embedded-app/load-linux/

Below is a summary of the Mac instructions which should also work for Linux

```
virtualenv ledger
source ledger/bin/activate
pip install ledgerblue
```

### Install on Nano S Plus

```
python3 -m ledgerblue.loadApp --curve secp256k1 --appFlags 0x000 --path "44'/111111'" --tlv --targetId 0x33100004 --fileName bin/app.hex --appName Kaspa --appVersion 0.0.1 --dataSize $((0x`cat debug/app.map |grep _envram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'` - 0x`cat debug/app.map |grep _nvram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'`)) --icon "0100000000ffffff00000000807ff03fce9c27e7c3f9709e9c33c7ffe01f00000000" --apiLevel 1 --delete
```

### Install on Nano S (untested)

When building the embedded app, use this command in the `build container`:
```
BOLOS_SDK=$NANOS_SDK make DEBUG=1
```

Then run this command in your `virtualenv`:

```
python3 -m ledgerblue.loadApp --curve secp256k1 --appFlags 0x000 --path "44'/111111'" --tlv --targetId 0x31100004 --fileName bin/app.hex --appName Kaspa --appVersion 0.0.1 --dataSize $((0x`cat debug/app.map |grep _envram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'` - 0x`cat debug/app.map |grep _nvram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'`)) --icon "0100000000ffffff00000000807ff03fce9c27e7c3f9709e9c33c7ffe01f00000000" --delete
```

The commands for `Nano S` is untested. Please let me know when you try this out and if it works for you.