#!/bin/bash

set -x
set -e

target=$1
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

sudo cp $script_dir/eth0 $target/etc/network/interfaces.d
sudo chmod 777 $target/etc/systemd/resolved.conf
sudo cat $script_dir/resolved.conf >> $target/etc/systemd/resolved.conf