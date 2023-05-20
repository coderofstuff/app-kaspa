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

## Compiling Embedded app
1. Go back to the base folder, clone the `app-kaspa` repo and `cd` into it
2. Use the utility script to `load` or `build` the app.

```
# -----------------------------
## If you want to build the app and then load it into your ledger device, run the command below

# For Nano S
bash utils.sh load nanos

# For Nano S+
bash utils.sh load nanosp

# -----------------------------
## If you want to only build the app then use it with the emulator, run the command below

# For Nano S
bash utils.sh build nanos

# For Nano S+
bash utils.sh build nanosp
```
3.  You now have a built `app.elf` ready for use by speculos in `app-kaspa/bin/app.elf`. If you chose to `load` the app, follow the instructions on your device to allow installing the app. See the `Installing on a Ledger Device` section below for more info.

## Setup the emulator
1. Open a new terminal session, and `cd` into your base directory (`kaspa-ledger`)
2. Generate a completely new 24-word seed with https://iancoleman.io/bip39/#english
3. Replace `[device]` with either `nanos` or `nanosp`. Replace the `CHANGE_THIS_TO_SEED_FROM_2` with your generated seed - keep the seed in between the double quotes. Then run:
```
bash app-kaspa/utils.sh init-emulator [device] "CHANGE_THIS_TO_SEED_FROM_2"
```
4. Your emulator is now running and is accessible at `http://127.0.0.1:5000`

## Setting up the Companion app UI
1. Open a new terminal session, and `cd` into your base directory (`kaspa-ledger`)
2. Install and run the companion app by running:
```
bash app-kaspa/utils.sh init-companion
```

## End state
1. You have the UI running in `http://127.0.0.1:3000`
2. You have compiled the embedded app
3. You have the emulator that loads the compiled app, running in `http://127.0.0.1:5000`
4. You are now ready to test!
5. Check back into this wiki regularly for any updates.

When testing with a real device, you will have to use a browser that supports Web HID (Chrome or Edge).

## Reporting issues
- Embedded app issues: https://github.com/coderofstuff/app-kaspa
- Companion app UI issues: https://github.com/lAmeR1/kaspa-ledger-webapp/issues


# Installing on a Ledger Device

When you install the app on the device, it will say `This app is not genuine` - this is normal for apps under development.

## Required
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

## Reference

The complete instructions can be found in:
- Mac: https://developers.ledger.com/docs/embedded-app/load-mac/
- Linux/WSL2: https://developers.ledger.com/docs/embedded-app/load-linux/

Below is a summary of the Mac instructions which should also work for Linux

## Install on Nano S Plus

```
bash utils.sh load nanosp
```

### Install on Nano S

When building the embedded app, use this command in the `build container`:
```
BOLOS_SDK=$NANOS_SDK make DEBUG=1
```

Then run this command in your `virtualenv`:

```
bash util.sh load nanos
```
