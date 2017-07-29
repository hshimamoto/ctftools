#!/bin/bash

scriptdir=$(dirname $0)
bindir=$scriptdir/bin

mkdir $bindir
pushd $bindir
curl -L -O https://github.com/downloads/0vercl0k/rp/rp-lin-x86
chmod +x rp-lin-x86
curl -L -O https://github.com/downloads/0vercl0k/rp/rp-lin-x64
chmod +x rp-lin-x64
popd
