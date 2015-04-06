/*
 * test_field_variable_values.cpp
 *
 *  Created on: Jan 08, 2015
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"

#include "../pastra/ps_table.h"
#include "../pastra/ps_serializer.h"

using namespace whais;
using namespace pastra;

const char db_name[] = "t_baza_date_1";
const char tb_name[] = "t_test_tab";


static char text1[]     = "Test_1";
static char text1_m[]   = "Test_2";

static char text2[]     = "A simple one entry text_1!";
static char text2_m[]   = "A simple one entry text_2!";

static char text3[]     = "A somehow more complex text sample, with some how more than one entry test_1!";
static char text3_m[]   = "A somehow more complex text sample, with some how more than one entry test_2!";

static char text4[]     = "A somehow more complex text sample, with some how more than one entry test_1!1";
static char text4_m[]   = "X somehow more complex text sample, with some how more than one entry test_2!2";


bool operator== (const DArray& a1, const DArray& a2)
{
  if (a1.Count () != a2.Count ())
    return false;

  for (uint_t i = 0; i < a1.Count (); i++)
    {
      DInt64 v1, v2;

      a1.Get (i, v1);
      a2.Get (i, v2);

      if (v1 != v2)
        return false;
    }

  return true;
}

bool operator!= (const DArray& a1, const DArray& a2)
{
  return ! (a1 == a2);
}

static bool
test_text_table (IDBSHandler& h)
{
  std::cout << "Testing row text values ... ";

  DBSFieldDescriptor fieldDesc = {"test_text", T_TEXT, false};

  ITable& table = h.CreateTempTable (1, &fieldDesc);
  ITable& tables = table.Spawn ();

  const FIELD_INDEX field = table.RetrieveField ("test_text");

  table.Set (0, field, DText (text3_m));
  table.Set (1, field, DText (text2_m));
  table.Set (2, field, DText (text1_m));
  table.Set (3, field, DText ());

  {
    DText fieldT1_m, fieldT2_m, fieldT3_m, fieldD;
    DText fieldT1, fieldT2, fieldT3;
    DText temp;

    table.Get (1, field, fieldT2_m);
    if (fieldT2_m != DText (text2_m))
       return false;

    table.Get (3, field, fieldD);
    if (!fieldD.IsNull ())
       return false;

    table.Get (2, field, fieldT1_m);
    if (fieldT1_m != DText (text1_m))
      return false;

    table.Get (0, field, fieldT3_m);
    if (fieldT3_m != DText (text3_m))
      return false;

    table.Set (0, field, DText (text3));
    table.Set (1, field, DText (text2));
    table.Set (2, field, DText (text1));

    table.Get (1, field, fieldT2);
    if (fieldT2 != DText (text2))
       return false;

    table.Get (3, field, fieldD);
    if (!fieldD.IsNull ())
       return false;

    table.Get (2, field, fieldT1);
    if (fieldT1 != DText (text1))
      return false;

    table.Get (0, field, fieldT3);
    if (fieldT3 != DText (text3))
      return false;

    if (fieldT1_m != DText (text1_m))
      return false;

    if (fieldT2_m != DText (text2_m))
      return false;

    if (fieldT3_m != DText (text3_m))
      return false;

    table.Set (0, field, fieldD);
    table.Set (1, field, fieldT1);
    table.Set (2, field, fieldT2);
    table.Set (3, field, fieldT3);
    temp = fieldT3;
    temp.Append (DChar ('1'));
    table.Set (4, field, temp);

    tables.Set (0, field, fieldD);
    tables.Set (1, field, fieldT1);
    tables.Set (2, field, fieldT2);
    tables.Set (3, field, fieldT3);

    tables.Get (3, field, fieldT3);
    fieldT3.Append (DChar ('1'));
    tables.Set (4, field, fieldT3);

    tables.Set (5, field, fieldT1_m);
    tables.Set (6, field, fieldT2_m);
    tables.Set (7, field, fieldT3_m);

    tables.Get (7, field, fieldT3_m);
    fieldT3_m.Append (DChar ('2'));
    fieldT3_m.CharAt(0, DChar ('X'));
    tables.Set (8, field, fieldT3_m);

    fieldT2_m.Append (DChar ('t'));
    fieldT3_m.CharAt (0, DChar (132));

    tables.Get (6, field, fieldT2_m);
    fieldT2_m.Append (DChar ('t'));

    tables.Get (7, field, fieldT3_m);
    fieldT3_m.CharAt (0, DChar (132));

    tables.Get (3, field, fieldT1);
    fieldT1.CharAt (1, DChar (120));
  }

  table.Set (5, field, DText (text1_m));
  table.Set (6, field, DText (text2_m));
  table.Set (7, field, DText (text3_m));
  table.Set (8, field, DText (text4_m));

  DText nt, f1, f2, f3, f4, f1_m, f2_m, f3_m, f4_m;
  DText snt, sf1, sf2, sf3, sf4, sf1_m, sf2_m, sf3_m, sf4_m;

  table.Get (0, field, nt);
  table.Get (1, field, f1);
  table.Get (2, field, f2);
  table.Get (3, field, f3);
  table.Get (4, field, f4);
  table.Get (5, field, f1_m);
  table.Get (6, field, f2_m);
  table.Get (7, field, f3_m);
  table.Get (8, field, f4_m);

  tables.Get (0, field, snt);
  tables.Get (1, field, sf1);
  tables.Get (2, field, sf2);
  tables.Get (3, field, sf3);
  tables.Get (4, field, sf4);
  tables.Get (5, field, sf1_m);
  tables.Get (6, field, sf2_m);
  tables.Get (7, field, sf3_m);
  tables.Get (8, field, sf4_m);

  if ((nt != snt) || ( ! nt.IsNull ()))
    return false;

  if ((f1 != sf1) || (f1 != DText (text1)))
    return false;

  if ((f1_m != sf1_m) || (f1_m != DText (text1_m)))
    return false;

  if ((f2 != sf2) || (f2 != DText (text2)))
    return false;

  if ((f2_m != sf2_m) || (f2_m != DText (text2_m)))
    return false;

  if ((f3 != sf3) || (f3 != DText (text3)))
    return false;

  if ((f3_m != sf3_m) || (f3_m != DText (text3_m)))
    return false;

  if ((f4 != sf4) || (f4 != DText (text4)))
    return false;

  if ((f4_m != sf4_m) || (f4_m != DText (text4_m)))
    return false;

  h.ReleaseTable (table);
  h.ReleaseTable (tables);

  std::cout << "OK\n";
  return true;
}

static bool
test_array_table (IDBSHandler& h)
{
  std::cout << "Testing row text values ... ";

  DBSFieldDescriptor fieldDesc = {"test_array", T_INT64, true};

  ITable& table = h.CreateTempTable (1, &fieldDesc);
  ITable& tables = table.Spawn ();

  const FIELD_INDEX field = table.RetrieveField ("test_array");

  DArray ran, ra1, ra2, ra3, ra4, ra1_m, ra2_m, ra3_m, ra4_m, rsan;

  ra1.Add (DInt64 (-1));
  ra1_m.Add (DInt64 (10));

  ra2.Add (DInt64 (-1));
  ra2.Add (DInt64 (-100));
  ra2.Add (DInt64 (-1000));
  ra2_m.Add (DInt64 (10));
  ra2_m.Add (DInt64 (1000));
  ra2_m.Add (DInt64 (10000));

  ra3.Add (DInt64 (-3));
  ra3.Add (DInt64 (-30));
  ra3.Add (DInt64 (-300));
  ra3.Add (DInt64 (-3000));
  ra3.Add (DInt64 (-30000));
  ra3.Add (DInt64 (-300000));
  ra3.Add (DInt64 (-3000000));
  ra3.Add (DInt64 (-30000000));
  ra3.Add (DInt64 (-300000000));
  ra3_m.Add (DInt64 (30));
  ra3_m.Add (DInt64 (300));
  ra3_m.Add (DInt64 (3000));
  ra3_m.Add (DInt64 (30000));
  ra3_m.Add (DInt64 (300000));
  ra3_m.Add (DInt64 (3000000));
  ra3_m.Add (DInt64 (30000000));
  ra3_m.Add (DInt64 (300000000));
  ra3_m.Add (DInt64 (3000000000));

  ra4 = ra3;
  ra4.Add (DInt64 (5));

  ra4_m = ra3_m;
  ra4_m.Set (0, DInt64 (1));

  table.Set (0, field, ra3_m);
  table.Set (1, field, ra2_m);
  table.Set (2, field, ra1_m);
  table.Set (3, field, rsan);

  {
    DArray fieldA1_m, fieldA2_m, fieldA3_m, fieldD;
    DArray fieldA1, fieldA2, fieldA3;
    DArray temp;

    table.Get (1, field, fieldA2_m);
    if (fieldA2_m != ra2_m)
       return false;

    table.Get (3, field, fieldD);
    if (!fieldD.IsNull ())
       return false;

    table.Get (2, field, fieldA1_m);
    if (fieldA1_m != ra1_m)
      return false;

    table.Get (0, field, fieldA3_m);
    if (fieldA3_m != ra3_m)
      return false;

    table.Set (0, field, ra3);
    table.Set (1, field, ra2);
    table.Set (2, field, ra1);

    table.Get (1, field, fieldA2);
    if (fieldA2 != ra2)
       return false;

    table.Get (3, field, fieldD);
    if (!fieldD.IsNull ())
       return false;

    table.Get (2, field, fieldA1);
    if (fieldA1 != ra1)
      return false;

    table.Get (0, field, fieldA3);
    if (fieldA3 != ra3)
      return false;

    if (fieldA1_m != ra1_m)
      return false;

    if (fieldA2_m != ra2_m)
      return false;

    if (fieldA3_m != ra3_m)
      return false;

    table.Set (0, field, fieldD);
    table.Set (1, field, fieldA1);
    table.Set (2, field, fieldA2);
    table.Set (3, field, fieldA3);

    temp = fieldA3;
    temp.Add (DInt64 (5));
    table.Set (4, field, temp);

    tables.Set (0, field, fieldD);
    tables.Set (1, field, fieldA1);
    tables.Set (2, field, fieldA2);
    tables.Set (3, field, fieldA3);

    fieldA3.Add (DInt64 (5));
    tables.Set (4, field, fieldA3);

    tables.Set (5, field, fieldA1_m);
    tables.Set (6, field, fieldA2_m);
    tables.Set (7, field, fieldA3_m);

    tables.Get (7, field, fieldA3_m);
    fieldA3_m.Set (0, DInt64 (1));
    tables.Set (8, field, fieldA3_m);

    tables.Get (2, field, fieldA2);
    fieldA2.Add (DInt64 (22));

    tables.Get (3, field, fieldA3);
    fieldA3.Set (1, DInt64 (33));
  }

  table.Set (5, field, ra1_m);
  table.Set (6, field, ra2_m);
  table.Set (7, field, ra3_m);
  table.Set (8, field, ra4_m);

  DArray nt, f1, f2, f3, f4, f1_m, f2_m, f3_m, f4_m;
  DArray snt, sf1, sf2, sf3, sf4, sf1_m, sf2_m, sf3_m, sf4_m;

  table.Get (0, field, nt);
  table.Get (1, field, f1);
  table.Get (2, field, f2);
  table.Get (3, field, f3);
  table.Get (4, field, f4);
  table.Get (5, field, f1_m);
  table.Get (6, field, f2_m);
  table.Get (7, field, f3_m);
  table.Get (8, field, f4_m);

  tables.Get (0, field, snt);
  tables.Get (1, field, sf1);
  tables.Get (2, field, sf2);
  tables.Get (3, field, sf3);
  tables.Get (4, field, sf4);
  tables.Get (5, field, sf1_m);
  tables.Get (6, field, sf2_m);
  tables.Get (7, field, sf3_m);
  tables.Get (8, field, sf4_m);

  if ((nt != snt) || ( ! nt.IsNull ()))
    return false;

  if ((f1 != sf1) || (f1 != ra1))
    return false;

  if ((f1_m != sf1_m) || (f1_m != ra1_m))
    return false;

  if ((f2 != sf2) || (f2 != ra2))
    return false;

  if ((f2_m != sf2_m) || (f2_m != ra2_m))
    return false;

  if ((f3 != sf3) || (f3 != ra3))
    return false;

  if ((f3_m != sf3_m) || (f3_m != ra3_m))
    return false;

  if ((f4 != sf4) || (f4 != ra4))
    return false;

  if ((f4_m != sf4_m) || (f4_m != ra4_m))
    return false;

  h.ReleaseTable (table);
  h.ReleaseTable (tables);

  std::cout << "OK\n";
  return true;
}


int
main ()
{
  bool success = true;
  {
    DBSInit (DBSSettings ());
    DBSCreateDatabase (db_name);
  }

  IDBSHandler& handler = DBSRetrieveDatabase (db_name);

  success &= test_text_table (handler);
  success &= test_array_table (handler);

  DBSReleaseDatabase (handler);
  DBSRemoveDatabase (db_name);
  DBSShoutdown ();


  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
