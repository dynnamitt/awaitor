#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <stdbool.h>

#include "dbg.h"
#include "mongoose.h"
#include "jsmn.h"

/// globals
#define SERVER_ERROR 500
#define DEF_PORT "8080"
#define OPT_ACL "access_control_list"
#define OPT_PORT "listening_port"

char **ext_cmd;

/// usage msg on error
static void
usage(char *arg0)
{
  puts("Usage:");
  printf("%s <command>\n",arg0);
  return;
}

/// Checking if string in arg0 starts w prefix, arg1.
static bool
starts_with(const char *string,
            const char *prefix)
{
  while(*prefix){
    if(*prefix++ != *string++)
      return false;
  }
  return true;
}

/// exec-and-fork.
static bool
exec(void)
{
     printf("    ");

    char **cmd = ext_cmd;
    while(*cmd){
      printf("%s ",*cmd);
      cmd++;
    }

    printf("\n");

    return true;
}

/// the mongoose EV handler, THE SERVER basically.
static int
event_handler(struct mg_connection *conn,
              enum mg_event ev)
{

  if (ev == MG_AUTH) {
    printf("In AUTH %s\n",conn->uri);
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST &&
             starts_with(conn->uri, "/hello_")) {

    bool s_code = exec();

    if(s_code){
      mg_printf_data(conn, "%s", "'OK'");
    } else {
      mg_send_status(conn, SERVER_ERROR);
      mg_printf_data(conn, "%s", "'Failed to exec predefined CMD'");
    }
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

  ext_cmd = calloc(argc, sizeof(char *));
  check_mem(ext_cmd);
  ext_cmd = &argv[1];

  char *port_no = getenv("PORT");
  if (!port_no) port_no = DEF_PORT;

  char *acl = getenv("ACL");
  if (!acl) acl = "";

  // init www server
  struct mg_server *server = mg_create_server(NULL, event_handler);
  check_mem(server);
  const char *pn_err = mg_set_option(server, OPT_PORT, port_no );
  check(!pn_err,"Cannot start www server.\n%s",pn_err);
  const char *acl_err = mg_set_option(server, OPT_ACL, acl);
  check(!acl_err,"Cannot init ACL. %s",acl_err);

  printf("Awaiting... on %s\n",port_no);

  // no-buf n flush. To notify UNIX pipes asap
  // setbuf(stdout, NULL);

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }

  mg_destroy_server(&server);
  return 0;

 error:
  if (server) mg_destroy_server(&server);
  return -1;
}
