#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <libgen.h>
#include <wait.h>
#include <stdbool.h>

#include "dbg.h"
#include "mongoose.h"
#include "jsmn.h"

/// globals
#define SERVER_ERROR 500
#define DEF_PORT "8080"
#define DEF_LOG_DIR_PREFIX "/var/log/"
#define POLL_TIMEOUT 1000
#define OPT_ACL "access_control_list"
#define OPT_PORT "listening_port"

char **ext_cmd;
char *log_dir;

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
starts_with(const char * restrict string,
            const char * restrict prefix)
{
  while(*prefix){
    if(*prefix++ != *string++)
      return false;
  }
  return true;
}

/// fork twice (avoiding zombie).
static bool
forkforkexec(char **argv)
{

  pid_t pid1 = fork();
  check(pid1 >= 0,"Fork failed!");

  if( pid1 == 0){
    // child1
    pid_t pid2 = fork();
    if (pid2 < 0 ) exit(EXIT_FAILURE);
    if( pid2 == 0 ){
      // child2
      execvp(argv[0],argv);
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }

  // parent
  // wait for 1st child
  wait(NULL);
  return true;

 error:
  return false;
}

/// the mongoose EV handler, THE SERVER basically.
static int
event_handler(struct mg_connection *conn,
              enum mg_event ev)
{

  if (ev == MG_AUTH) {
    log_info("In AUTH %s\n",conn->uri);
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST &&
             starts_with(conn->uri, "/hello_")) {

    bool s_code = forkforkexec(ext_cmd);

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

  // check input args on start
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

  log_dir = getenv("LOG_DIR");
  if (!log_dir) {
    char *app_name = basename(argv[0]);
    check_mem(asprintf(&log_dir, "%s%s/",
                       DEF_LOG_DIR_PREFIX, app_name));
  }

  log_info("logdir --> '%s'" , log_dir );

  // init www server
  struct mg_server *server = mg_create_server(NULL, event_handler);
  check_mem(server);
  const char *pn_err = mg_set_option(server, OPT_PORT, port_no );
  check(!pn_err,"Cannot start www server.\n%s",pn_err);
  const char *acl_err = mg_set_option(server, OPT_ACL, acl);
  check(!acl_err,"Cannot init ACL. %s",acl_err);

  printf("Awaiting... on %s\n", port_no);

  // no-buf n flush. To notify UNIX pipes asap
  // setbuf(stdout, NULL);

  for (;;) {
    mg_poll_server(server, POLL_TIMEOUT);
    // Infinite loop, Ctrl-C to stop
  }

  mg_destroy_server(&server);
  return 0;

 error:
  if (server) mg_destroy_server(&server);
  return -1;
}
