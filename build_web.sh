#!/bin/bash
# Compile le moteur C en WebAssembly. Prérequis : source ~/emsdk/emsdk_env.sh
set -e
cd "$(dirname "$0")"
mkdir -p web
emcc moteur/moteur.c moteur/pont.c -Imoteur -O2 -Wall -Wextra \
  -sMODULARIZE -sEXPORT_NAME=creerSonote \
  -sEXPORTED_RUNTIME_METHODS=cwrap,UTF8ToString,FS \
  -o web/sonote.js
cp moteur/structure.txt web/structure.txt
echo "OK : web/sonote.js + web/sonote.wasm"
