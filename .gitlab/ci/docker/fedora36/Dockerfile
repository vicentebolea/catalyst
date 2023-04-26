FROM fedora:36
MAINTAINER Ben Boeckel <ben.boeckel@kitware.com>

COPY install_deps.sh /root/install_deps.sh
RUN sh /root/install_deps.sh

COPY install_conduit.sh /root/install_conduit.sh
RUN sh /root/install_conduit.sh
