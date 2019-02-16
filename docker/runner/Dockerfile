# Base image
FROM deltanitt/codecharacter-base-2019:latest

# Copy simulator source
COPY ./ /root/codecharacter

WORKDIR /root/codecharacter/build

# Build simulator
RUN cmake .. -DBUILD_PROJECT=no_tests -DCMAKE_INSTALL_PREFIX="/root/simulator" \
&& make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Delete Simulator source
WORKDIR /root
RUN rm -rf /root/codecharacter

# Copy runner script
COPY docker/runner/runner.sh /root

# Make output dir and delete simulator source
RUN mkdir /root/output_log && rm -rf /root/codecharacter

CMD ["/bin/bash", "/root/runner.sh"]
