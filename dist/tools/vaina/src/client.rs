use std::ffi::OsString;
use std::net::{Ipv6Addr, SocketAddr, UdpSocket};
use std::os::unix::io::FromRawFd;
use std::str::FromStr;

use nix::sys::socket::sockopt::{BindToDevice, Ipv6AddMembership};
use nix::sys::socket::{
    bind, setsockopt, socket, AddressFamily, InetAddr, Ipv6Addr as NixIpv6Addr,
    Ipv6MembershipRequest, SockAddr, SockFlag, SockProtocol, SockType,
};

use snafu::ResultExt;

use crate::msg::Message;
use crate::*;

/// VAINA multicast address
pub const VAINA_MCAST_ADDR: &str = "ff15::42";
pub const VAINA_PORT: u16 = 1337;

#[derive(Debug)]
pub struct VainaClient {
    seqno: u8,
    pending_acks: Vec<u8>,
    sock: UdpSocket,
    group: SocketAddr,
}

impl VainaClient {
    /// New `VainaClient`
    pub fn new(netif: &OsString) -> Result<VainaClient, Error> {
        let group = Ipv6Addr::from_str(VAINA_MCAST_ADDR).unwrap();
        let stdaddr = SocketAddr::new(group.clone().into(), VAINA_PORT);
        let sockaddr = SockAddr::new_inet(InetAddr::from_std(&stdaddr));
        let fd = socket(
            AddressFamily::Inet6,
            SockType::Datagram,
            SockFlag::empty(),
            SockProtocol::Udp,
        )
        .context(VainaSocket)?;

        setsockopt(fd, BindToDevice, netif).context(VainaSocket)?;
        bind(fd, &sockaddr).context(VainaSocket)?;

        let group_req = Ipv6MembershipRequest::new(NixIpv6Addr::from_std(&group));
        setsockopt(fd, Ipv6AddMembership, &group_req).context(VainaSocket)?;

        Ok(VainaClient {
            seqno: 0u8,
            pending_acks: Vec::new(),
            sock: unsafe { UdpSocket::from_raw_fd(fd) },
            group: stdaddr,
        })
    }

    /// Craft a new sequence number
    pub fn craft_seqno(&mut self) -> u8 {
        let seqno = self.seqno;
        self.seqno += 1;
        seqno
    }

    /// Send a message
    pub fn send_message(&mut self, msg: &Message) -> Result<(), Error> {
        // Verify if well wait for an ACK!
        if !msg.is_ack() {
            self.pending_acks.push(msg.seqno());
        }

        let bytes = msg.serialize();

        self.sock
            .send_to(bytes.as_ref(), &self.group)
            .context(FailedSend)?;

        Ok(())
    }
}
