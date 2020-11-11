#! /bin/sh


case "$1" in
start)
echo "Starting init script for I2C module"
modprobe i2c-dev
modprobe i2c-bcm2835
start-stop-daemon -S -n digital -a /usr/bin/tmp102 -- -d
;;
stop)
echo "Removing user modules"
start-stop-daemon -K -n tmp102
rmmod i2c-bcm2835
rmmo i2c-dev
;;
*)
echo "Usage: $0 {start|stop}"
exit 1
esac
exit 0
