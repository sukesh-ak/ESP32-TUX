#! /bin/bash
set -e
. activate.sh
echo 
echo Stop monitor by pressing Ctrl+]
echo
sleep 2
idf.py monitor
