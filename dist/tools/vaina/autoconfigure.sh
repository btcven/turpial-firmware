#!/bin/sh

TUN=sl0
PREFIX=64
TUN_GLB="fc00:db8::1" # Dirección global de prueba
SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")

SUDO=${SUDO:-sudo}

unsupported_platform() {
    echo "unsupported platform" >&2
    echo "(currently supported \`uname -s\` 'Darwin' and 'Linux')" >&2
}

case "$(uname -s)" in
    Darwin)
        PLATFORM="OSX";;
    Linux)
        PLATFORM="Linux";;
    *)
        unsupported_platform
        exit 1
        ;;
esac

add_addresses() {
    case "${PLATFORM}" in
        Linux)
            cargo build --release
            ${SUDO} ip address add ${TUN_GLB} dev ${TUN}
            ${SUDO} target/release/vaina rcs add ${TUN} ${PREFIX} ${TUN_GLB}
            ${SUDO} target/release/vaina nib add ${TUN} ${PREFIX} ${TUN_GLB}
            ;;
        OSX)
            cargo build --relesae
# TODO: add the IPV6 address to the interface!!!
            ${SUDO} target/release/vaina rcs add ${TUN} ${PREFIX} ${TUN_GLB}
            ${SUDO} target/release/vaina nib add ${TUN} ${PREFIX} ${TUN_GLB}
            unsupported_platform
            ;;
    esac
    return 0
}

add_addresses
