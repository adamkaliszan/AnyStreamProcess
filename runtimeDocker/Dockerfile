FROM ubuntu:20.10

RUN apt-get -y update

RUN apt-get -y install locales
RUN locale-gen en_US.UTF-8 && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LC_ALL en_US.UTF-8

RUN apt-get install -y qt5-default

ENV USER_NAME researcher

ARG host_uid=1000
ARG host_gid=1000

COPY ./console/console /usr/local/bin/distribution

RUN groupadd -g $host_gid $USER_NAME && useradd -g $host_gid -m -s /bin/bash -u $host_uid $USER_NAME
USER $USER_NAME

WORKDIR /public/Work
ENTRYPOINT ["/usr/local/bin/distribution"]

