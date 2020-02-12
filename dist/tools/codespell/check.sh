#!/bin/bash
CODESPELL_CMD="codespell"
CODESPELL_OPT=" -c"
CODESPELL_OPT+=" -q 2"
CODESPELL_OPT+=" --check-hidden"
CODESPELL_OPT+=" --ignore-words-list=ND,nd,wan,od,dout"

ERRORS=$(${CODESPELL_CMD} ${CODESPELL_OPT})

if [ -n "${ERRORS}" ]
then
    echo "${ERRORS}"
    printf "Found some misspellings words, please fix it before continue.\n"
    exit 1
else
    printf "Codespell return no errors.\n"
    exit 0
fi