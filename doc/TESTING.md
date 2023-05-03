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
