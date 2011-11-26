/******************************************************************************
UTILS - Common routines used trough WHISPER project
Copyright (C) 2008  Iulian Popa

Address: Str Olimp nr. 6
         Pantelimon Ilfov,
         Romania
Phone:   +40721939650
e-mail:  popaiulian@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <assert.h>
#include <string.h>

#include "whisper.h"

#include "list.h"

struct UList *
init_list (struct UList *list, size_t item_size)
{
  struct UList *const res = list;

  if (res == NULL)
    {
      return NULL;
    }
  memset (res, 0, sizeof (res[0]));

  res->item_size = item_size;

  res->sentinel.next = &res->sentinel;
  res->sentinel.prev = &res->sentinel;

  return res;
}

struct UListLink *
add_link_after (struct UList *list, struct UListLink *link, void *data)
{
  struct UListLink *new_link = NULL;
  int mem_size = list->item_size - sizeof (new_link->data);

  new_link = (struct UListLink *) mem_alloc (sizeof (new_link[0]) - mem_size);

  link->next->prev = new_link;
  new_link->next = link->next;
  new_link->prev = link;
  link->next = new_link;

  memcpy (link->data, data, list->item_size);

  return new_link;;
}

struct UListLink *
add_link_before (struct UList *list, struct UListLink *link, void *data)
{
  return add_link_after (list, link->prev, data);
}

struct UListLink *
add_link_front (struct UList *list, void *data)
{
  return add_link_after (list, &list->sentinel, data);
}

struct UListLink *
add_link_tail (struct UList *list, void *data)
{
  return add_link_after (list, list->sentinel.prev, data);
}

struct UList *
free_link (struct UList *list, struct UListLink *link)
{

  assert (link != &list->sentinel);
  link->prev->next = link->next;
  link->next->prev = link->prev;

  mem_free (link);

  return list;
}

struct UListLink *
first_link (struct UList *list)
{
  struct UListLink *const result = list->sentinel.next;
  if (result == &list->sentinel)
    {
      return NULL;
    }
  /* else */
  return result;
}

struct UListLink *
last_link (struct UList *list)
{
  struct UListLink *const result = list->sentinel.prev;
  if (result == &list->sentinel)
    {
      return NULL;
    }
  /* else */
  return result;
}

struct UListLink *
next_link (struct UList *list, struct UListLink *link)
{
  struct UListLink *const result = link->next;
  if (result == &list->sentinel)
    {
      return NULL;
    }
  /* else */
  return result;
}

struct UListLink *
prev_link (struct UList *list, struct UListLink *link)
{
  struct UListLink *const result = link->prev;
  if (result == &list->sentinel)
    {
      return NULL;
    }
  /* else */
  return result;
}

void
destroy_list (struct UList *list)
{
  struct UListLink *link = first_link (list);
  while (link != NULL)
    {
      struct UListLink *tmp = next_link (list, link);
      free_link (list, link);
      link = tmp;
    }
}
