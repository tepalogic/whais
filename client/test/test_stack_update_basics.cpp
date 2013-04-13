#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;

struct BasicValueEntry
{
  const uint_t  type;
  const char* value;
};

BasicValueEntry _values[] =
    {
        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},
        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},
        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"},

        /* Repeat */

        {WHC_TYPE_BOOL, "0"},
        {WHC_TYPE_CHAR, "h"},
        {WHC_TYPE_DATE, "1201/12/10"},
        {WHC_TYPE_DATETIME, "2091/10/11 14:23:12"},
        {WHC_TYPE_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WHC_TYPE_INT8, "-67"},
        {WHC_TYPE_INT16, "-10234"},
        {WHC_TYPE_INT32, "-34125"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "245"},
        {WHC_TYPE_UINT16, "45678"},
        {WHC_TYPE_UINT32, "3099012231"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-122.123"},
        {WHC_TYPE_RICHREAL, "538.154454223"},
        {WHC_TYPE_INT8, "123"},
        {WHC_TYPE_INT16, "561"},
        {WHC_TYPE_INT32, "134512334"},
        {WHC_TYPE_INT64, "923341124456320012"},
        {WHC_TYPE_REAL, "0.1"},
        {WHC_TYPE_REAL, "1"},
        {WHC_TYPE_RICHREAL, "-0.1"},
        {WHC_TYPE_RICHREAL, "-1"}
    };

static const uint_t _valuesCount = sizeof (_values) / sizeof (_values[0]);

static bool
test_step_update (WH_CONNECTION hnd)
{

  cout << "Testing basic values step update ... ";

  for (uint_t i = 0; i < _valuesCount; ++i)
    {
      uint_t        type;
      const char* value;

      unsigned long long rowsCount, elemesCount;

      if ((WPushValue (hnd, _values[i].type, 0, NULL) != WCS_OK)
          || (WFlush (hnd) != WCS_OK))
        {
          goto test_step_update_err;
        }

      if ((WStackValueType (hnd, &type) != WCS_OK)
          || (type != _values[i].type))
        {
          goto test_step_update_err;
        }

      if ((WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WValueArraySize (hnd, WIGNORE_FIELD, WIGNORE_ROW, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WValueRowsCount (hnd, &rowsCount) != WCS_TYPE_MISMATCH)
          || (WValueEntry (hnd, "some_field", WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_FIELD)
          || (WValueEntry (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_ROW)
          || (WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, 0, WIGNORE_OFF, &value) != WCS_TYPE_MISMATCH)
          || (WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, 0, &value) != WCS_TYPE_MISMATCH)
          || (WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, NULL) != WCS_INVALID_ARGS))
        {
          goto test_step_update_err;
        }

      if ((WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_OK)
          || (strlen (value) != 0))
        {
          goto test_step_update_err;
        }

      if ((WUpdateValue (hnd, _values[i].type, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, _values[i].value) != WCS_OK)
          || (WFlush (hnd) != WCS_OK))
        {
          goto test_step_update_err;
        }

      if ((WStackValueType (hnd, &type) != WCS_OK)
          || (type != _values[i].type))
        {
          goto test_step_update_err;
        }

      if ((WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WValueArraySize (hnd, WIGNORE_FIELD, WIGNORE_ROW, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WValueRowsCount (hnd, &rowsCount) != WCS_TYPE_MISMATCH)
          || (WValueEntry (hnd, "some_field", WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_FIELD)
          || (WValueEntry (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_ROW)
          || (WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, 0, WIGNORE_OFF, &value) != WCS_TYPE_MISMATCH)
          || (WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, 0, &value) != WCS_TYPE_MISMATCH)
          || (WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, NULL) != WCS_INVALID_ARGS))
        {
          goto test_step_update_err;
        }

      if ((WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_OK)
          || (strcmp (value, _values[i].value) != 0))
        {
          goto test_step_update_err;
        }
    }

  cout << "OK\n";
  return true;

test_step_update_err:
  cout << "FAIL\n";
  return false;
}

static bool
test_bulk_update (WH_CONNECTION hnd)
{

  cout << "Testing basic values bulk update ... ";

  for (uint_t i = 0; i < _valuesCount; ++i)
    {
      if ((WPushValue (hnd, _values[i].type, 0, NULL) != WCS_OK)
          || (WUpdateValue (hnd,
                                 _values[i].type,
                                 WIGNORE_FIELD,
                                 WIGNORE_ROW,
                                 WIGNORE_OFF,
                                 WIGNORE_OFF,
                                 _values[i].value) != WCS_OK))
        {
          goto test_bulk_update_err;
        }
    }

  if (WFlush (hnd) != WCS_OK)
    goto test_bulk_update_err;

  for (int i = _valuesCount - 1; i >= 0; --i)
    {
      uint_t        type;
      const char* value;

      if ((WStackValueType (hnd, &type) != WCS_OK)
          || (type != _values[i].type))
        {
          goto test_bulk_update_err;
        }

      if ((WValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_OK)
          || (strcmp (value, _values[i].value) != 0))
        {
          goto test_bulk_update_err;
        }

      if ((WPopValues (hnd, 1) != WCS_OK)
          || (WFlush (hnd) != WCS_OK))
        {
          goto test_bulk_update_err;
        }
    }

  cout << "OK\n";
  return true;

test_bulk_update_err:
  cout << "FAIL\n";
  return false;
}



static bool
test_for_errors (WH_CONNECTION hnd)
{
  unsigned long long count;

  cout << "Testing against error conditions ... ";

  if ((WPushValue (hnd, WHC_TYPE_BOOL, 0, NULL) != WCS_OK)
      || (WFlush (hnd) != WCS_OK)
      || (WFlush (hnd) != WCS_OK) //Just for fun!
      || (WValueRowsCount (NULL, NULL) != WCS_INVALID_ARGS)
      || (WValueRowsCount (NULL, &count) != WCS_INVALID_ARGS)
      || (WValueRowsCount (hnd, NULL) != WCS_INVALID_ARGS)
      || (WValueRowsCount (hnd, &count) != WCS_TYPE_MISMATCH)
      || (WValueArraySize (hnd, WIGNORE_FIELD, WIGNORE_ROW, &count) != WCS_TYPE_MISMATCH)
      || (WValueArraySize (hnd, "some_f", WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WValueArraySize (hnd, WIGNORE_FIELD, 0, &count) != WCS_INVALID_ROW)
      || (WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_TYPE_MISMATCH)
      || (WValueTextLength (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WValueTextLength (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW)
      || (WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_OFF, 0, &count) != WCS_TYPE_MISMATCH)
      || (WPopValues (hnd, WPOP_ALL) != WCS_OK)
      || (WFlush (hnd) != WCS_OK))
    {
      goto test_for_errors_fail;
    }

  cout << "OK\n";
  return true;

test_for_errors_fail :
  cout << "FAIL\n";
  return false;
}

const char*
DefaultDatabaseName ()
{
  return "test_list_db";
}

const uint_t
DefaultUserId ()
{
  return 1;
}

const char*
DefaultUserPassword ()
{
  return "test_password";
}

int
main (int argc, const char** argv)
{
  WH_CONNECTION hnd        = NULL;

  bool success = tc_settup_connection (argc, argv, &hnd);

  success = success && test_for_errors (hnd);
  success = success && test_step_update (hnd);
  success = success && test_bulk_update (hnd);

  WClose (hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}
