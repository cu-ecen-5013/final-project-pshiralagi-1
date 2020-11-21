#! /bin/sh
case "$1" in
    start)
        echo "Starting server application"
        start-stop-daemon -S -n server -a /usr/bin/server -- -d
        ;;
    stop)
        echo "Stopping server application"
        start-stop-daemon -K -n server
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac
exit 0
