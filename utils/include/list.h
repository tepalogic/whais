/******************************************************************************
UTILS - Common routines used trough WHISPER project
Copyright (C) 2009  Iulian Popa

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

#ifndef ULIST_H
#define ULIST_H

#include "whisper.h"

struct UListLink
{
  struct UListLink *next;
  struct UListLink *prev;
  /* data should start from here */
  D_UINTMAX data[1];		/* force a good alignment, data will be stored
				   from here  */
};

struct UList
{
  /* the sentinel */
  struct UListLink sentinel;

  /* An item in the list has this size.
   * This field should not be modified once
   * the list is initialised */
  size_t item_size;
};

struct UList *init_list (struct UList *list, size_t item_size);

struct UListLink *add_link_before (struct UList *list,
				   struct UListLink *link, void *data);

struct UListLink *add_link_after (struct UList *list,
				  struct UListLink *link, void *data);

struct UListLink *add_link_front (struct UList *list, void *data);

struct UListLink *add_link_tail (struct UList *list, void *data);

struct UList *free_link (struct UList *list, struct UListLink *link);

struct UListLink *first_link (struct UList *list);

struct UListLink *last_link (struct UList *list);

struct UListLink *next_link (struct UList *list, struct UListLink *link);

struct UListLink *prev_link (struct UList *list, struct UListLink *link);

void destroy_list (struct UList *list);

#endif /* ULIST_H */
