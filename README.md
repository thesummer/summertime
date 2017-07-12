# summertime

Make sure to clone the repository with all submodules recursively.

`git clone --recursive git@github.com:thesummer/summertime.git`


## Build instructions ESP toolchain

  1. Build the compiler toolchain as described in esp-open-sdk. Use `make STANDALONE=n`
  2. Add the generated compiler to your PATH, e.g. export PATH=$(pwd)/esp-open-sdk/xtensa-lx106-elf/bin:$PATH

## Setup the SDK

Unfortunately, in the official SDK several function like `printf` and `putchar` are
duplicated in libcirom.a and libmain.a which produces linker errors. In order to avoid
that remove the duplicates from libcirom.a

`
cd ESP8266_RTOS_SDK/lib
cp libcirom.a libcirom.a.save
xtensa-lx106-elf-objcopy -N printf -N putchar -N puts libcirom.a
`

## Build instructions ESP-outpost examples

Make sure to have the xtensa compiler available in your PATH and that you removed the
duplicates in the SDK.

Building the outpost-minimal example should be as easy as

`
cd examples/outpost-minimal/
scons
`

## Deploying binary to target

TODO
