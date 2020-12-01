#!/bin/sh

SLIPTTY_DIR="$(cd "$(dirname "$0")" && pwd -P)/../../../RIOT/dist/tools/sliptty"
TUN=sl0
CREATED_IFACE=0
START_SLIP=1

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

create_tun() {
    case "${PLATFORM}" in
        Linux)
            if ! ip link show ${TUN} 2>&1 > /dev/null; then
                ${SUDO} ip tuntap add ${TUN} mode tun user ${USER} || exit 1
                CREATED_IFACE=1
            fi
            ${SUDO} sysctl -w net.ipv6.conf.all.forwarding=1
            echo "Activated forwarding for all interfaces." >&2
            echo "Deactivate with" >&2
            echo "    ${SUDO} sysctl -w net.ipv6.conf.all.forwarding=0" >&2
            echo "when not desired without this script" >&2
            ${SUDO} sysctl -w net.ipv6.conf.${TUN}.accept_ra=2
            ${SUDO} ip link set ${TUN} up || exit 1
            ${SUDO} ip address add fe80::1/64 dev ${TUN}
            ${SUDO} ip neigh add fe80::2 dev ${TUN}
            ${SUDO} ip route add ${PREFIX} via fe80::2 dev ${TUN}
            ;;
        OSX)
            # TUN interface in OSX needs to be called tunX
            TUN="tun$(echo "${TUN}" | grep -o '[0-9]\+$')"
            ${SUDO} chown ${USER} /dev/${TUN} || exit 1
            ${SUDO} sysctl -w net.inet6.ip6.forwarding=1
            echo "Activated forwarding for all interfaces." >&2
            echo "Deactivate with" >&2
            echo "    ${SUDO} sysctl -w net.inet6.ip6.forwarding=0" >&2
            echo "when not desired without this script" >&2
            echo "start RIOT instance for ${TUN} now and hit enter"
            read _
            ${SUDO} ifconfig ${TUN} up || exit 1
            ${SUDO} ifconfig ${TUN} inet6 fe80::1 prefixlen 64
            ${SUDO} route -n add -interface ${TUN} -inet6 -prefixlen 64 \
                    ${PREFIX} fe80::2 || exit 1
            ;;
    esac
    return 0
}

remove_tun() {
    case "${PLATFORM}" in
        Linux)
            ${SUDO} ip tuntap del ${TUN} mode tun
            ;;
        OSX)
            ${SUDO} route delete -inet6 ${PREFIX} -prefixlen 64 fe80::2
            ;;
    esac
}

cleanup() {
    if [ 1 -eq "${CREATED_IFACE}" ]; then
        remove_tun
    fi
    trap "" INT QUIT TERM EXIT
}

usage() {
    echo "usage: $1 [-I <sl0>] [-d] [-e] [-g <addr>/<prefix_len>] <prefix> serial [baudrate]"
    echo "usage: $1 [-I <sl0>] [-d] [-e] [-g <addr>/<prefix_len>] <prefix> tcp:host [port]"
}

trap "cleanup" INT QUIT TERM EXIT

while getopts dehI: opt; do
    case ${opt} in
        I)  TUN=${OPTARG}; shift 2;;
        g)  TUN_GLB=${OPTARG}; shift 2;;
        h)  usage $0; exit 0;;
    esac
done

PREFIX="$1"
if [ -z "$PREFIX" ]; then
    usage $0
    exit 1
fi
shift

create_tun

START_SLIP=0

[ ${START_SLIP} -eq 0 ] && "${SLIPTTY_DIR}"/sliptty -I "${TUN}" "$@"
