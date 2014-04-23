log_scanner: log_scanner.c config.c
	gcc -Wall -std=c99 config.c log_scanner.c -I /opt/local/include -L /opt/local/lib -lpcre -lyaml `pkg-config --cflags --libs glib-2.0` -o log_scanner
