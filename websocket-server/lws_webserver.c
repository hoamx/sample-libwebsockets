#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libwebsockets.h> 

struct reason_string {
	enum lws_callback_reasons reason;
	char *reason_string;	
};


static struct reason_string reason_strings[] = {
  {LWS_CALLBACK_ESTABLISHED, "LWS_CALLBACK_ESTABLISHED"},
  {LWS_CALLBACK_CLIENT_CONNECTION_ERROR,"LWS_CALLBACK_CLIENT_CONNECTION_ERROR"},
  {LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH,"LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH"},
  {LWS_CALLBACK_CLIENT_ESTABLISHED,"LWS_CALLBACK_CLIENT_ESTABLISHED"},
  {LWS_CALLBACK_CLOSED,"LWS_CALLBACK_CLOSED"},
  {LWS_CALLBACK_CLOSED_HTTP,"LWS_CALLBACK_CLOSED_HTTP"},
  {LWS_CALLBACK_RECEIVE,"LWS_CALLBACK_RECEIVE"},
  {LWS_CALLBACK_CLIENT_RECEIVE,"LWS_CALLBACK_CLIENT_RECEIVE"},
  {LWS_CALLBACK_CLIENT_RECEIVE_PONG,"LWS_CALLBACK_CLIENT_RECEIVE_PONG"},
  {LWS_CALLBACK_CLIENT_WRITEABLE,"LWS_CALLBACK_CLIENT_WRITEABLE"},
  {LWS_CALLBACK_SERVER_WRITEABLE,"LWS_CALLBACK_SERVER_WRITEABLE"},
  {LWS_CALLBACK_HTTP,"LWS_CALLBACK_HTTP"},
  {LWS_CALLBACK_HTTP_BODY,"LWS_CALLBACK_HTTP_BODY"},
  {LWS_CALLBACK_HTTP_BODY_COMPLETION,"LWS_CALLBACK_HTTP_BODY_COMPLETION"},
  {LWS_CALLBACK_HTTP_FILE_COMPLETION,"LWS_CALLBACK_HTTP_FILE_COMPLETION"},
  {LWS_CALLBACK_HTTP_WRITEABLE,"LWS_CALLBACK_HTTP_WRITEABLE"},
  {LWS_CALLBACK_FILTER_NETWORK_CONNECTION,"LWS_CALLBACK_FILTER_NETWORK_CONNECTION"},
  {LWS_CALLBACK_FILTER_HTTP_CONNECTION,"LWS_CALLBACK_FILTER_HTTP_CONNECTION"},
  {LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED,"LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED"},
  {LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION,"LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION"},
  {LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS,"LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS"},
  {LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS,"LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS"},
  {LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION,"LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION"},
  {LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER,"LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER"},
  {LWS_CALLBACK_CONFIRM_EXTENSION_OKAY,"LWS_CALLBACK_CONFIRM_EXTENSION_OKAY"},
  {LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED,"LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED"},
  {LWS_CALLBACK_PROTOCOL_INIT,"LWS_CALLBACK_PROTOCOL_INIT"},
  {LWS_CALLBACK_PROTOCOL_DESTROY,"WS_CALLBACK_PROTOCOL_DESTROY"},
  {LWS_CALLBACK_WSI_CREATE,"LWS_CALLBACK_WSI_CREATE"},
  {LWS_CALLBACK_WSI_DESTROY,"LWS_CALLBACK_WSI_DESTROY"},
  {LWS_CALLBACK_GET_THREAD_ID,"LWS_CALLBACK_GET_THREAD_ID"},
  {LWS_CALLBACK_ADD_POLL_FD,"LWS_CALLBACK_ADD_POLL_FD"},
  {LWS_CALLBACK_DEL_POLL_FD,"LWS_CALLBACK_DEL_POLL_FD"},
  {LWS_CALLBACK_CHANGE_MODE_POLL_FD,"LWS_CALLBACK_CHANGE_MODE_POLL_FD"},
  {LWS_CALLBACK_LOCK_POLL,"LWS_CALLBACK_LOCK_POLL"},
  {LWS_CALLBACK_UNLOCK_POLL,"LWS_CALLBACK_UNLOCK_POLL"},
  {LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY,"LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY"},
  {LWS_CALLBACK_USER,"LWS_CALLBACK_USE"}
};

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	return 0;
}

static int callback_chat(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	lwsl_notice("%s\n", reason_strings[reason].reason_string);
	switch (reason) {
		case LWS_CALLBACK_RECEIVE: {
			int i;
			char server_response[100];
			int string_len;
			printf("Received data: %s\n", (char *) in);
			printf("Reply client: ");

			fgets(server_response, 100, stdin);
			string_len = strlen(server_response);
			unsigned char *buf = (unsigned char *) malloc (LWS_SEND_BUFFER_PRE_PADDING + 100 + LWS_SEND_BUFFER_POST_PADDING);

			for ( i = 0; i < strlen(server_response); i++ ) {
				buf[LWS_SEND_BUFFER_PRE_PADDING + i] = server_response[i];
			}

			// Send response
			lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], string_len, LWS_WRITE_TEXT);
			// Release memory back into the wild.
			free(buf);
		}
			break;
		default:
			break;
	}

	return 0;
}

static struct lws_protocols protocols[] = {
	/* First protocol must always be HTTP handler */
	{
		"http-only",
		callback_http,
		0
	},
	{
		"chat",
		callback_chat,
		0
	},
	{
		NULL, NULL, 0	/* End of list */
	}
};

int main()
{
	// Server url will be http://localhost:9000
	int port = 9000;
	struct lws_context *context;
	struct lws_context_creation_info context_info = {
		.port 	= port,
		.iface 	= NULL,
		.protocols = protocols,
		.extensions = NULL,
		.ssl_cert_filepath = NULL,
		.ssl_private_key_filepath = NULL,
		.ssl_ca_filepath = NULL,
		.gid 	= -1,
		.uid 	= -1,
		.options = 0,
		NULL,
		.ka_time = 0,
		.ka_interval = 0
	};
	
	// Create lws context representing this server 
	context = lws_create_context(&context_info);

	if ( context == NULL  ) 
	{
		fprintf(stderr, "lws init failed\n");
		return -1;
	}

	printf("Starting server....\n");

	// infinite loop
	while (1)
	{
		lws_service(context, 50);
	}

	lws_context_destroy(context);

	return 0;
}
			
