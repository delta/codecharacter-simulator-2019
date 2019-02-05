# Base image
FROM deltanitt/codecharacter-base-2019:latest

# Copy simulator source
COPY ./ /root/codecharacter

WORKDIR /root/codecharacter/build

# Build simulator
RUN cmake .. -DBUILD_PROJECT=no_tests -DCMAKE_INSTALL_PREFIX="/root/simulator" \
&& make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Copy compiler script
COPY ./docker/compiler/compiler.sh /root

# Make output dirs
RUN mkdir /root/output_libs

CMD ["/bin/bash", "/root/compiler.sh"]
