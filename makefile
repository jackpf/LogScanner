log_scanner: log_scanner.c
	gcc -Wall log_scanner.c -I /opt/local/include -L /opt/local/lib -lpcre -std=c99 -o log_scanner
