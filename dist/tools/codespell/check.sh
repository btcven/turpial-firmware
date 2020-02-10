#!/usr/bin/env bash
CODESPELL_CMD="codespell"
CODESPELL_OPTS=" -q 2"
CODESPELL_OPTS+=" -c"
CODESPELL_OPTS+=" --ignore-words-list=ND,nd,wan,od,dout"

ERRORS=$(${CODESPELL_CMD} ${CODESPELL_OPTS})

if [ -n "${ERRORS}" ]
then
    echo "${ERRORS}"
    printf "Found some misspellings words, please fix it before continue.\n"
    exit 1
else
    printf "Codespell return no errors.\n"
    exit 0
fi
