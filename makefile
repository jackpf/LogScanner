log_scanner: log_scanner.c
	gcc -Wall -std=c99 config.c log_scanner.c -I /opt/local/include -L /opt/local/lib -lpcre -lyaml -o log_scanner
