#!/bin/bash

exec codespell -h
exec codespell --version

CODESPELL_CMD="codespell -c -q 2"

ERRORS=$(${CODESPELL_CMD})

if [ -n "${ERRORS}" ]
then
    echo "${ERRORS}"
    printf "Found some misspellings words, please fix it before continue.\n"
    exit 1
else
    printf "Codespell return no errors.\n"
    exit 0
fi
