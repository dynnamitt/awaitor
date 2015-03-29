#include "mongoose.h"
#include "jsmn.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


// CONFIG 
typedef struct {
  int argc;
  char **argv;
  char *port_no;
  char *ips_allowed;
} config_t;

static int
event_handler(struct mg_connection *conn,
              enum mg_event ev)
{
  printf(" ---> threadid %d :" , (int)pthread_self());
  if (ev == MG_AUTH) {
    printf("In AUTH %s\n",conn->uri);
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/hello")) {
    //
    mg_printf_data(conn, "%s", "Hello world");
    char * cmd = null;
    for (int i=0 ; i< conn->server_param->argc; i++) {
      
    }
    printf("time to EXEC %s",cmd);
    puts("++++++++++++++");
    return MG_TRUE;   // Mark as processed
  } else {
    printf("NOOP. ev: %d , uri:%s\n",ev,conn->uri);
    return MG_FALSE;  // Rest of the events are not processed
  }
}

int
main(int argc, char *argv[]) {

  // grab CONFIG from args + env
  config_t config = {
    .argc = argc,
    .argv = argv,
    .port_no = getenv("PORT"),
    .ips_allowed = getenv("IPS_ALLOWED"),
  };

  // Set default CONF values if missing
  if (!config.port_no) { config.port_no = "8080"; }
  if (!config.ips_allowed) {
    config.ips_allowed = "127.0.0.1,";
  }

  // init www server
  struct mg_server *server = mg_create_server(&config, event_handler);
  mg_set_option(server, "listening_port", config.port_no );

  printf("server up on port %s, ", config.port_no );
  printf("threadid %d .\n" , (int)pthread_self());

  // no-buf-flush. To notify UNIX pipes asap
  setbuf(stdout, NULL);

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }
  mg_destroy_server(&server);

  return 0;
}
