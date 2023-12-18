FROM kitware/intelcompiler:2024.0.0-el8
MAINTAINER Christos Tsolakis <christos.tsolakis@kitware.com>

COPY setup_intel_compilers.sh /root/setup_intel_compilers.sh

COPY install_deps.sh /root/install_deps.sh
RUN sh /root/install_deps.sh

COPY install_conduit.sh /root/install_conduit.sh
RUN sh /root/install_conduit.sh
