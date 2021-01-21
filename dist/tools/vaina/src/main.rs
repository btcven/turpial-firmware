#[macro_use]
extern crate clap;

use clap::App;
use snafu::Snafu;

mod client;
mod msg;
mod nib;
mod rcs;

#[derive(Debug, Snafu)]
pub enum Error {
    #[snafu(display("Could not create socket for VAINA: {}", source))]
    VainaSocket { source: nix::Error },
    #[snafu(display("Could not send data to VAINA: {}", source))]
    FailedSend { source: std::io::Error },
}

fn main() {
    let yaml = load_yaml!("cli.yml");
    let matches = App::from_yaml(yaml).get_matches();

    let result = match matches.subcommand() {
        ("rcs", Some(rcs_matches)) => rcs::handle_matches(rcs_matches),
        ("nib", Some(nib_matches)) => nib::handle_matches(nib_matches),
        _ => {
            println!("{}", matches.usage());
            Ok(())
        }
    };

    match result {
        Ok(()) => (),
        Err(e) => println!("{}", e),
    }
}
