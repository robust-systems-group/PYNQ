#! /bin/bash

target=$1
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

sudo mkdir $target/usr/python
cat $script_dir/zc706_I2C.py | sudo tee -a $target/usr/python/zc706_I2C.py
cat $script_dir/LDOWizard.py | sudo tee -a $target/usr/python/LDOWizard.py
cat $script_dir/PCA9557.py | sudo tee -a $target/usr/python/PCA9557.py
cat $script_dir/smbus2.py | sudo tee -a $target/usr/python/smbus2.py
cat $script_dir/TCA6416.py | sudo tee -a $target/usr/python/TCA6416.py
