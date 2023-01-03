#! /bin/bash
set -e
# Get the --port argument and set it to the ESPPORT so that it
# can be used by idf.py
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --port)
        # set the env variable ESPPORT environmental variable
        export ESPPORT="$2"
        shift # past argument
        shift # past value
        ;;
        *)    # unknown option
        shift # past argument
        ;;
    esac
done
. activate.sh && ./build.sh && idf.py flash monitor
