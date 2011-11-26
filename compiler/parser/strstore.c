/******************************************************************************
 WHISPERC - A compiler for whisper programs
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

#include "whisper.h"

#include "strstore.h"

#define DEFAULT_STR_SIZE  128

struct StoreLink
{
  struct StoreLink *next;
  D_UINT allocated;
  D_UINT unused;
  D_CHAR data[1];
};

static struct StoreLink *
alloc_link (const D_UINT size)
{
  struct StoreLink *link =
    mem_alloc (sizeof (struct StoreLink) + size * sizeof (D_CHAR));
  if (link != NULL)
    {
      link->next = NULL;
      link->unused = link->allocated = size;
    }

  return link;
}

StringStoreHnd
create_string_store ()
{
  return (StringStoreHnd) alloc_link (DEFAULT_STR_SIZE);
}

void
release_string_store (StringStoreHnd * handle)
{
  struct StoreLink *link = (struct StoreLink *) handle;
  while (link != NULL)
    {
      struct StoreLink *temp = link->next;
      mem_free (link);
      link = temp;
    }

}

D_CHAR *
alloc_str (StringStoreHnd handle, D_UINT lenght)
{
  D_CHAR *result;
  struct StoreLink *link = (struct StoreLink *) handle;

  while ((link->unused < lenght) && (link->next != NULL))
    link = link->next;

  if (link->unused > lenght)
    {
      result = (link->allocated - link->unused) + link->data;
      link->unused -= lenght;
    }
  else
    {
      link->next = alloc_link ((lenght < DEFAULT_STR_SIZE ?
				DEFAULT_STR_SIZE : lenght));
      link = link->next;
      if (link == NULL)
	result = NULL;
      else
	{
	  result = (link->allocated - link->unused) + link->data;
	  link->unused -= lenght;
	}
    }

  return result;
}
