#! /bin/sh
case "$1" in
    start)
        echo "Starting accelerometer application"
        start-stop-daemon -S -n aesdsocket -a /usr/bin/adxl335 -- -d
        ;;
    stop)
        echo "Stopping accelerometer application"
        start-stop-daemon -K -n adxl335
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac
exit 0
