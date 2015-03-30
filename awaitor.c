#include "mongoose.h"
#include "jsmn.h"
#include "dm_str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


// CONFIG
typedef struct {
  char **argv;
  char *port_no;
  char *ips_allowed;
} config_t;

config_t config;

void
usage(char *arg0)
{
  puts("Usage:");
  printf("%s <command>\n",arg0);
  return;
}

static int
event_handler(struct mg_connection *conn,
              enum mg_event ev)
{

  if (ev == MG_AUTH) {
    printf("In AUTH %s\n",conn->uri);
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && starts_with(conn->uri, "/hello_")) {

    printf("    ");

    char **cmd = config.argv;
    while(*cmd){
      printf("%s ",*cmd);
      cmd++;
    }

    printf("\n");

    mg_printf_data(conn, "%s", "Hello world");
    return MG_TRUE;   // Mark as processed

  } else {
    return MG_FALSE;  // Rest of the events are not processed
  }
}

int
main(int argc, char *argv[])
{

  if (argc==1){
    usage(argv[0]);
    return 1;
  }

  // grab CONFIG from args + env
  config.argv = malloc(argc*sizeof(char *));
  config.argv = &argv[1];
  config.argv[argc-1] = NULL;

  config.port_no = getenv("PORT");
  config.ips_allowed = getenv("IPS_ALLOWED");

  // Set default CONF values if missing
  if (!config.port_no) { config.port_no = "8080"; }
  if (!config.ips_allowed) {
    config.ips_allowed = "127.0.0.1,";
  }

  // init www server
  struct mg_server *server = mg_create_server(NULL, event_handler);
  mg_set_option(server, "listening_port", config.port_no );
  /* mg_set_option(server, "access_log_file", "/dev/stdout"); */
  
  printf("Awaiting.. (on port %s)\n", config.port_no );

  // no-buf-flush. To notify UNIX pipes asap
  setbuf(stdout, NULL);

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }
  mg_destroy_server(&server);
  puts("quiting");
  return 0;
}
