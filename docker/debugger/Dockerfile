# Base image
FROM deltanitt/codecharacter-base-2019:latest

# Install GDB
RUN apt-get update \
&& apt-get install -y --no-install-recommends \
gdb

# Copy simulator source
COPY ./ /root/codecharacter

WORKDIR /root/codecharacter/build

# Build simulator
RUN cmake .. -DBUILD_PROJECT=no_tests -DCMAKE_INSTALL_PREFIX="/root/simulator" \
&& make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Copy debugger script
COPY ./docker/debugger/debug-run.sh /root

# Make output dirs
RUN mkdir /root/output_libs

CMD ["/bin/bash", "/root/debug-run.sh"]
