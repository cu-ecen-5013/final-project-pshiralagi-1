#! /bin/sh
case "$1" in
    start)
        echo "Starting accelerometer application"
        start-stop-daemon -S -n digital -a /usr/bin/digital -- -d
        ;;
    stop)
        echo "Stopping accelerometer application"
        start-stop-daemon -K -n digital
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac
exit 0
