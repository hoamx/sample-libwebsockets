.PHONY:clean
	
all:webserver

webserver:webserver.c
	gcc webserver.c -o webserver -lwebsockets

clean:
	-rm webserver