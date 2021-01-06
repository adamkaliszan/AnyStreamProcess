#!/bin/bash
cd runtimeDocker
./build.sh
cd ..
sudo docker build runtimeDocker/ -t akaliszan/distribution_calculator:1.0.2
