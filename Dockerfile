FROM dec1/qt-base:5.15.0_ubuntu20.04 MAINTAINER Declan dec1@geoaze.com (@dec1)

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends git cmake build-essential libgl1-mesa-dev libpulse-dev libfontconfig1 \
libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
libxcb-randr0 libxcb-render-util0 libxcb-shape0-dev libxcb-xinerama0 libxcb-xkb1 libxkbcommon-x11-0 xvfb xfwm4 python3 python3-venv psmisc
RUN apt-get install qt5-default qt5-qmake

