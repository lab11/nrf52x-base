#include <stdint.h>
#include "sdk_errors.h"

typedef struct node {

  void *data;
  size_t data_size;

  struct node *next;
} ll_node;

ret_code_t ll_push(ll_node** head, void *data, size_t data_size);
void ll_remove(ll_node** node, ll_node* previous);

