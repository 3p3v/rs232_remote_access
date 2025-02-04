FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

RUN apt-get update
RUN apt-get -y install \
    git wget tar \
    # Building libraries
    build-essential gcc make cmake cmake-gui cmake-curses-gui \
    # fakeroot devscripts dh-make lsb-release \
    libssl-dev \
    # Docs
    doxygen graphviz


# Install libraries
# paho.mqtt.c
RUN git clone --depth 1 --branch v1.3.13 https://github.com/eclipse-paho/paho.mqtt.c ~/paho.mqtt.c
RUN mkdir /tmp/build.paho.c ; cd /tmp/build.paho.c && \
    cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_STATIC=TRUE \
        ~/paho.mqtt.c && \
    cmake --build . --target install

# paho.mqtt.cpp
RUN git clone https://github.com/3p3v/paho.mqtt.cpp.unique ~/paho.mqtt.cpp.unique
RUN mkdir /tmp/build.paho.cpp ; cd /tmp/build.paho.cpp && \
    cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_STATIC=TRUE \
        ~/paho.mqtt.cpp.unique && \
    cmake --build . --target install

# gtest
RUN git clone https://github.com/google/googletest.git -b v1.15.2 ~/googletest
RUN cd ~/googletest && \
    mkdir build ; cd build && \
    cmake .. && \
    make install

# boost
RUN cd ~ && \
    wget https://archives.boost.io/release/1.82.0/source/boost_1_82_0.tar.gz && \
    tar -xvzf ./boost_1_82_0.tar.gz
RUN cd ~/boost_1_82_0 && \
    ./bootstrap.sh && \
    ./b2 install