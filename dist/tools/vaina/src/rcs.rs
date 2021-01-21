use std::ffi::OsString;
use std::net::Ipv6Addr;

use clap::{value_t, ArgMatches};

use crate::client::VainaClient;
use crate::msg::Message;
use crate::Error;

/// Router Client Set sub command
pub fn handle_matches(matches: &ArgMatches) -> Result<(), Error> {
    match matches.subcommand() {
        ("add", Some(add_matches)) => add(add_matches)?,
        ("del", Some(del_matches)) => del(del_matches)?,
        _ => println!("{}", matches.usage()),
    };

    Ok(())
}

fn add(matches: &ArgMatches) -> Result<(), Error> {
    let prefix = value_t!(matches, "prefix", u8).unwrap_or_else(|e| e.exit());
    let ip = value_t!(matches, "IP", Ipv6Addr).unwrap_or_else(|e| e.exit());
    let interface = value_t!(matches, "interface", String).unwrap_or_else(|e| e.exit());
    let interface = OsString::from(interface);

    let mut client = VainaClient::new(&interface)?;

    let msg = Message::RcsAdd {
        seqno: client.craft_seqno(),
        prefix,
        ip,
    };
    client.send_message(&msg)?;

    Ok(())
}

fn del(matches: &ArgMatches) -> Result<(), Error> {
    let prefix = value_t!(matches, "prefix", u8).unwrap_or_else(|e| e.exit());
    let ip = value_t!(matches, "IP", Ipv6Addr).unwrap_or_else(|e| e.exit());
    let interface = value_t!(matches, "interface", String).unwrap_or_else(|e| e.exit());
    let interface = OsString::from(interface);

    let mut client = VainaClient::new(&interface)?;

    let msg = Message::RcsDel {
        seqno: client.craft_seqno(),
        prefix,
        ip,
    };
    client.send_message(&msg)?;

    Ok(())
}
