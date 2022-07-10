#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"

base64 -d <<< "4pSP4pSB4pW44pSP4pSB4pST4pSP4pSB4pST4pSP4pSB4pST4pSP4pSB4pSTICAg4pW7IOKVu+KUj+KUgeKUk+KVuyDilbsgICDilI/ilIHilJPilbvilI/ilLPilJPilbsg4pW74pW7ICDilI/ilIHilJPilbrilLPilbjilI/ilIHilJPilI/ilIHilJMK4pSj4pW4IOKUo+KUs+KUm+KUo+KUs+KUm+KUgyDilIPilKPilLPilJsgICDilJfilIHilKvilIPilIPilIPilJfilIHilKsgICDilJfilIHilJPilIPilIPilIPilIPilIMg4pSD4pSDICDilKPilIHilKsg4pSDIOKUgyDilIPilKPilLPilJsK4pSX4pSB4pW44pW54pSX4pW44pW54pSX4pW44pSX4pSB4pSb4pW54pSX4pW4ICAgICDilbnilJfilIHilJsgIOKVuSAgIOKUl+KUgeKUm+KVueKVuSDilbnilJfilIHilJvilJfilIHilbjilbkg4pW5IOKVuSDilJfilIHilJvilbnilJfilbgK"

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
 
sudo make install

## custom permissions handling for MacOS
if [[ $(uname) == "Darwin" ]]; then
    sudo chmod 550 /usr/local/bin/simulatorplot
fi
