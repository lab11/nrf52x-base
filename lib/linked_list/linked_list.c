#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

ret_code_t ll_push(ll_node** head, void *data, size_t data_size) {
  // create new node
  ll_node *new_node = malloc(sizeof(ll_node));
  if (!new_node) return NRF_ERROR_NO_MEM;

  // allocate space for node data
  new_node->data = malloc(data_size);
  if (!new_node->data) {
    free(new_node);
    return NRF_ERROR_NO_MEM;
  }

  // copy data to node data
  memcpy(new_node->data, data, data_size);

  // set node data size
  new_node->data_size = data_size;

  // add node to head of list
  new_node->next = *head;
  (*head) = new_node;

  return NRF_SUCCESS;
}

void ll_remove(ll_node** head, ll_node* node) {
  // if we are removing the first node
  if (*head == node) {
    (*head) = (*head)->next;
    free(node->data);
    free(node);
    return;
  }

  // otherwise search for it
  ll_node *i = (*head);
  ll_node *pr = NULL;
  while (i && i != node) {
    pr = i;
    i = i->next;
  }
  if (i) {
    pr->next = node->next;
    free(node->data);
    free(node);
  }
}
