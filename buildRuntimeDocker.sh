#!/bin/bash
cd runtimeDocker
./build.sh
cd ..
sudo docker build runtimeDocker/ -t akaliszan/distribution_calculator:0.9.3

