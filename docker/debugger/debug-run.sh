cd /root/codecharacter/build

# Make player library1
cmake .. -DBUILD_PROJECT=player_code -DCMAKE_BUILD_TYPE="Debug"
make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

cp /root/simulator/lib/libplayer_1_code.so ~/

# Copy in player 2 code
cd /root/codecharacter/src/player_code/src
mv player_code.cpp.2 player_code.cpp

# Make player library2
cd /root/codecharacter/build
cmake .. -DBUILD_PROJECT=player_code -DCMAKE_BUILD_TYPE="Debug"
make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

cp ~/libplayer_1_code.so /root/simulator/lib

# Copy map and key files
cp /root/codecharacter/src/player_code/src/map.txt /root/simulator/bin
cp /root/codecharacter/src/player_code/src/key.txt /root/simulator/bin

# Run simulator debugger
cd /root/simulator/bin
gdb -batch -ex "set follow-fork-mode child" -ex "run" -ex "bt" ./main > dump.out

# Copy to output
cp dump.out /root/output_libs
