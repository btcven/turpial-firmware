use bytes::{BufMut, Bytes, BytesMut};
use std::net::Ipv6Addr;

pub const VAINA_MSG_ACK: u8 = 0;
pub const VAINA_MSG_NACK: u8 = 1;
pub const VAINA_MSG_RCS_ADD: u8 = 2;
pub const VAINA_MSG_RCS_DEL: u8 = 3;
pub const VAINA_MSG_NIB_ADD: u8 = 4;
pub const VAINA_MSG_NIB_DEL: u8 = 5;

/// VAINA message
pub enum Message {
    /// ACK
    Ack {
        /// Sequence number
        seqno: u8,
    },
    /// NACK
    Nack {
        /// Sequence number
        seqno: u8,
    },
    /// Add entry to RCS
    RcsAdd {
        /// Sequence number
        seqno: u8,
        /// IPv6 address prefix
        prefix: u8,
        /// Entry IPv6 address
        ip: Ipv6Addr,
    },
    /// Remove entry from RCS
    RcsDel {
        /// Sequence number
        seqno: u8,
        /// IPv6 address prefix
        prefix: u8,
        /// Entry IPv6 address
        ip: Ipv6Addr,
    },
    /// Add entry to NIB
    NibAdd {
        seqno: u8,
        /// IPv6 address prefix
        prefix: u8,
        /// Entry IPv6 address
        ip: Ipv6Addr,
    },
    /// Delete entry from NIB
    NibDel {
        seqno: u8,
        /// IPv6 address prefix
        prefix: u8,
        /// Entry IPv6 address
        ip: Ipv6Addr,
    },
}

impl Message {
    pub fn seqno(&self) -> u8 {
        match *self {
            Message::Ack { seqno, .. } => seqno,
            Message::Nack { seqno, .. } => seqno,
            Message::RcsAdd { seqno, .. } => seqno,
            Message::RcsDel { seqno, .. } => seqno,
            Message::NibAdd { seqno, .. } => seqno,
            Message::NibDel { seqno, .. } => seqno,
        }
    }

    pub fn is_ack(&self) -> bool {
        match *self {
            Message::Ack { .. } | Message::Nack { .. } => true,
            _ => false,
        }
    }

    pub fn serialize(&self) -> Bytes {
        let mut buf = BytesMut::with_capacity(25);

        match *self {
            Message::Ack { .. } | Message::Nack { .. } => {}
            Message::RcsAdd { seqno, prefix, ref ip } => {
                buf.put_u8(VAINA_MSG_RCS_ADD);
                buf.put_u8(seqno);
                buf.put_u8(prefix);
                buf.put_slice(&ip.octets());
            }
            Message::RcsDel { seqno, prefix, ref ip } => {
                buf.put_u8(VAINA_MSG_RCS_DEL);
                buf.put_u8(seqno);
                buf.put_u8(prefix);
                buf.put_slice(&ip.octets());
            }
            Message::NibAdd {
                seqno,
                prefix,
                ref ip,
            } => {
                buf.put_u8(VAINA_MSG_NIB_ADD);
                buf.put_u8(seqno);
                buf.put_u8(prefix);
                buf.put_slice(&ip.octets());
            }
            Message::NibDel {
                seqno,
                prefix,
                ref ip,
            } => {
                buf.put_u8(VAINA_MSG_NIB_DEL);
                buf.put_u8(seqno);
                buf.put_u8(prefix);
                buf.put_slice(&ip.octets());
            }
        }

        buf.freeze()
    }
}
