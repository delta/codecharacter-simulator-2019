cd /root/codecharacter/build

# Make player libraries
# TODO: FIX THIS! We should be building player_code and not no_tests
cmake .. -DBUILD_PROJECT=no_tests
make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Copy output libraries
cp /root/simulator/lib/libplayer_1_code.so /root/output_libs
cp /root/simulator/lib/libplayer_2_code.so /root/output_libs
