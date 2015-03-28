#include "mongoose.h"
#include "jsmn.h"

#include <stdlib.h>

int count;

void initialize(void)
{
  count=0;
}

void main(void)
{
  /* Initialization. */
  initialize();

  jsmn_parser parser;
  jsmn_init(&parser);

  struct mg_server *server;

  return 0;

}
