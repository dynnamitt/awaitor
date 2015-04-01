#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "dbg.h"
#include "mongoose.h"
#include "jsmn.h"


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


int
starts_with(const char *restrict string,
	    const char *restrict prefix)
{
    while(*prefix){
        if(*prefix++ != *string++)
            return 0;
    }
    return 1;
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
  config.argv = calloc(argc,sizeof(char *));
  check_mem(config.argv); // may fail
  config.argv = &argv[1];
  config.port_no = getenv("PORT");
  config.ips_allowed = getenv("IPS_ALLOWED");

  // Set default CONF values if missing
  if (!config.port_no) { config.port_no = "8080"; }
  if (!config.ips_allowed) {
    config.ips_allowed = "127.0.0.1,";
  }

  // init www server
  struct mg_server *server = mg_create_server(NULL, event_handler);
  check_mem(server);
  const char *err = mg_set_option(server, "listening_port", config.port_no );
  check(!err,"Cannot start www server.\n%s",err);
  /* mg_set_option(server, "access_log_file", "/dev/stdout"); */

  // make a pretty list of OPTIONS
  puts("www options set:");
  puts("----------------");
  const char **opts = mg_get_valid_option_names();
   for (int i = 0; opts[i] != NULL; i += 2) {
    printf( "%s = '%s'\n",
            opts[i],
	    mg_get_option(server,opts[i]));
  }

  puts("===========");
  puts("Awaiting...");

  // no-buf-flush. To notify UNIX pipes asap
  setbuf(stdout, NULL);

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }

  mg_destroy_server(&server);
  return 0;

 error:
  if (server) mg_destroy_server(&server);
  return -1;
}
