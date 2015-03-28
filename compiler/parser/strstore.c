/******************************************************************************
 WHAISC - A compiler for whais programs
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
/*
 * strstore.c - Implements a string storage used to hold the parsed text.
 */

#include <assert.h>

#include "whais.h"

#include "strstore.h"

#define DEFAULT_STR_SIZE  128

struct StoreLink
{
  struct StoreLink*     next;
  uint_t                allocated;
  uint_t                unused;
  char                  data[1];
};

static struct StoreLink*
alloc_link (const uint_t size)
{
  struct StoreLink* const link = mem_alloc (sizeof( struct StoreLink) +
                                            size * sizeof (char));
  if (link != NULL)
    {
      link->next      = NULL;
      link->unused    = size;
      link->allocated = size;
    }

  return link;
}

StringStoreHnd
create_string_store()
{
  return (StringStoreHnd) alloc_link (DEFAULT_STR_SIZE);
}


void
release_string_store (StringStoreHnd* handle)
{
  struct StoreLink* link = (struct StoreLink*) handle;
  while (link != NULL)
    {
      struct StoreLink* const temp = link->next;

      mem_free (link);
      link = temp;
    }
}


char*
alloc_str (StringStoreHnd handle, uint_t length)
{
  struct StoreLink* link   = (struct StoreLink*) handle;
  char*             result = NULL;

  while ((link->unused < length) && (link->next != NULL))
    link = link->next;

  if (link->unused >= length)
    {
      result        = (link->allocated - link->unused) + link->data;
      link->unused -= length;
    }
  else
    {
      assert (link->next == NULL);

      link->next = alloc_link (MAX (length,  DEFAULT_STR_SIZE));
      link       = link->next;

      if (link == NULL)
        result = NULL;

      else
        {
          result        =  (link->allocated - link->unused) + link->data;
          link->unused -= length;
        }
    }

  return result;
}

