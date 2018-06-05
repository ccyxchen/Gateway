source ~/.bash_profile
gcc -o server server.c `mysql_config --cflags` `mysql_config --libs`
arm-linux-gcc End_Device.c serial.c socket.c  -o End_Device -lpthread