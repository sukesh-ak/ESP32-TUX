#! /bin/bash

# if _IN_ENVIRONMENT is not set
if [ -z "$_IN_ENVIRONMENT" ]; then
    # Set environmental variable _IN_ENVIRONMENT = 1
    . ~/esp/esp-idf/export.sh
    # Set the current directory to the directory of the script
    cd "$(dirname "${BASH_SOURCE[0]}")"
    export _IN_ENVIRONMENT=1
fi
