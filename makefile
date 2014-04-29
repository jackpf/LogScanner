debug = -g

log_scanner: log_scanner.c filesystem.c filesystem.h config.c config.h pushover.c pushover.h common.c common.h
	gcc $(debug) -Wall -std=c99 common.c filesystem.c config.c pushover.c log_scanner.c -I/opt/local/include -L/opt/local/lib -lpcre -lyaml `pkg-config --cflags --libs glib-2.0` `curl-config --libs` -o ./bin/log-scanner
