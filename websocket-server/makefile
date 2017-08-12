.PHONY:clean
	
all:lws_webserver

lws_webserver:lws_webserver.c
	gcc lws_webserver.c -o lws_webserver -lwebsockets

clean:
	-rm lws_webserver
