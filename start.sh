#!/bin/sh
mkdir -p "bin"
make clean;
make all || exit;
cd bin;
./gestore;
