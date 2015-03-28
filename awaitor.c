#include "mongoose.h"
#include "jsmn.h"
#include <string.h>

static int
event_handler(struct mg_connection *conn,
              enum mg_event ev) {
  if (ev == MG_AUTH) {
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/hello")) {
    //
    mg_printf_data(conn, "%s", "Hello world");
    puts("Bingo");
    return MG_TRUE;   // Mark as processed
  } else {
    printf("skippin this ev: %d , uri:%s\n",ev,conn->uri);
    return MG_FALSE;  // Rest of the events are not processed
  }
}

int
main(void) {

  char *port = "8080";

  struct mg_server *server = mg_create_server(NULL, event_handler);
  mg_set_option(server, "document_root", ".");
  mg_set_option(server, "listening_port", port );

  printf("server up on port %s.\n",port);

  setbuf(stdout, NULL);
  

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }
  mg_destroy_server(&server);

  return 0;
}
