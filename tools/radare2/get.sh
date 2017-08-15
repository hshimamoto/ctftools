#!/bin/bash

scriptdir=$(dirname $0)

cd $scriptdir
# shallow clone
git clone --depth 1 https://github.com/radare/radare2
cd radare2
# install to user bin
./sys/user.sh
