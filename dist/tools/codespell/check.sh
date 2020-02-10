#!/usr/bin/env bash
exec codespell -h

CODESPELL_CMD="codespell"

CODESPELL_OPT=" --enable-colors"
CODESPELL_OPT+=" --quiet-level 2"
CODESPELL_OPTS+=" --check-hidden"

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
