#! /bin/sh
case "$1" in
    start)
        echo "Starting accelerometer application"
        start-stop-daemon -S -n client -a /usr/bin/client -- -d
        ;;
    stop)
        echo "Stopping accelerometer application"
        start-stop-daemon -K -n client
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac
exit 0
