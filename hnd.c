#include "mongoose.h"
#include "jsmn.h"
#include "dm_str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


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

