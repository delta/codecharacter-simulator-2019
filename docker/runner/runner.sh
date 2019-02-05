cd /root/codecharacter/build

# Copy input libraries
cp /root/input_libs/libplayer_1_code.so /root/simulator/lib/
cp /root/input_libs/libplayer_2_code.so /root/simulator/lib/

# Copy map and key files
cp /root/input_libs/map.txt /root/simulator/bin
cp /root/input_libs/key.txt /root/simulator/bin

# Run simulator
cd /root/simulator/bin
./main

# Copy game log to output dir
if [ -f /root/simulator/bin/game.log  ]; then
	cp /root/simulator/bin/game.log /root/output_log
fi

# Copy debug logs to output dir
if [ -f /root/simulator/bin/player_1.dlog  ]; then
	cp /root/simulator/bin/player_1.dlog /root/output_log
fi

if [ -f /root/simulator/bin/player_2.dlog  ]; then
	cp /root/simulator/bin/player_2.dlog /root/output_log
fi
