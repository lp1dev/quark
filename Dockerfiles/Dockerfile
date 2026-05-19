FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

ENV VITASDK=/usr/local/vitasdk
ENV PATH=${PATH}:${VITASDK}/bin

RUN apt-get update -y && apt-get upgrade -y

RUN apt-get install emacs-nox -y

RUN apt-get install zip tar curl cmake build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl1.2-dev git-core -y

RUN apt-get install -y wget xz-utils python3 apt-transport-https ca-certificates gnupg software-properties-common bzip2 sudo

RUN \
    git clone https://github.com/vitasdk/vdpm && \
    cd vdpm && \
    ./bootstrap-vitasdk.sh && \
    ./install-all.sh

WORKDIR /quark

COPY . .

RUN /quark/scripts/setup.sh && make -f Makefile.Linux && cp /quark/liblexbor.so /lib/liblexbor.so.2

ENTRYPOINT [ "bash", "/quark/scripts/build_project.sh" ]