#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;

struct BasicValueEntry
{
  const D_UINT  type;
  const D_CHAR* value;
};

BasicValueEntry _values[] =
    {
        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},
        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},
        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"},

        /* Repeat */

        {WFT_BOOL, "0"},
        {WFT_CHAR, "h"},
        {WFT_DATE, "1201/12/10"},
        {WFT_DATETIME, "2091/10/11 14:23:12"},
        {WFT_HIRESTIME, "7812/2/3 14:31:12.90124"},
        {WFT_INT8, "-67"},
        {WFT_INT16, "-10234"},
        {WFT_INT32, "-34125"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "245"},
        {WFT_UINT16, "45678"},
        {WFT_UINT32, "3099012231"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-122.123"},
        {WFT_RICHREAL, "538.154454223"},
        {WFT_INT8, "123"},
        {WFT_INT16, "561"},
        {WFT_INT32, "134512334"},
        {WFT_INT64, "923341124456320012"},
        {WFT_REAL, "0.1"},
        {WFT_REAL, "1"},
        {WFT_RICHREAL, "-0.1"},
        {WFT_RICHREAL, "-1"}
    };

static const D_UINT _valuesCount = sizeof (_values) / sizeof (_values[0]);

static bool
test_step_update (W_CONNECTOR_HND hnd)
{

  cout << "Testing basic values step update ... ";

  for (D_UINT i = 0; i < _valuesCount; ++i)
    {
      D_UINT        type;
      const D_CHAR* value;

      unsigned long long rowsCount, elemesCount;

      if ((WPushStackValue (hnd, _values[i].type, 0, NULL) != WCS_OK)
          || (WUpdateStackFlush (hnd) != WCS_OK))
        {
          goto test_step_update_err;
        }

      if ((WDescribeStackTop (hnd, &type) != WCS_OK)
          || (type != _values[i].type))
        {
          goto test_step_update_err;
        }

      if ((WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WGetStackValueRowsCount (hnd, &rowsCount) != WCS_TYPE_MISMATCH)
          || (WGetStackValueEntry (hnd, "some_field", WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_FIELD)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_ROW)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, 0, WIGNORE_OFF, &value) != WCS_TYPE_MISMATCH)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, 0, &value) != WCS_TYPE_MISMATCH)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, NULL) != WCS_INVALID_ARGS))
        {
          goto test_step_update_err;
        }

      if ((WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_OK)
          || (strlen (value) != 0))
        {
          goto test_step_update_err;
        }

      if ((WUpdateStackValue (hnd, _values[i].type, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, _values[i].value) != WCS_OK)
          || (WUpdateStackFlush (hnd) != WCS_OK))
        {
          goto test_step_update_err;
        }

      if ((WDescribeStackTop (hnd, &type) != WCS_OK)
          || (type != _values[i].type))
        {
          goto test_step_update_err;
        }

      if ((WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, &elemesCount) != WCS_TYPE_MISMATCH)
          || (WGetStackValueRowsCount (hnd, &rowsCount) != WCS_TYPE_MISMATCH)
          || (WGetStackValueEntry (hnd, "some_field", WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_FIELD)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_INVALID_ROW)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, 0, WIGNORE_OFF, &value) != WCS_TYPE_MISMATCH)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, 0, &value) != WCS_TYPE_MISMATCH)
          || (WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, NULL) != WCS_INVALID_ARGS))
        {
          goto test_step_update_err;
        }

      if ((WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_OK)
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
test_bulk_update (W_CONNECTOR_HND hnd)
{

  cout << "Testing basic values bulk update ... ";

  for (D_UINT i = 0; i < _valuesCount; ++i)
    {
      if ((WPushStackValue (hnd, _values[i].type, 0, NULL) != WCS_OK)
          || (WUpdateStackValue (hnd,
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

  if (WUpdateStackFlush (hnd) != WCS_OK)
    goto test_bulk_update_err;

  for (D_INT i = _valuesCount - 1; i >= 0; --i)
    {
      D_UINT        type;
      const D_CHAR* value;

      if ((WDescribeStackTop (hnd, &type) != WCS_OK)
          || (type != _values[i].type))
        {
          goto test_bulk_update_err;
        }

      if ((WGetStackValueEntry (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, WIGNORE_OFF, &value) != WCS_OK)
          || (strcmp (value, _values[i].value) != 0))
        {
          goto test_bulk_update_err;
        }

      if ((WPopStackValues (hnd, 1) != WCS_OK)
          || (WUpdateStackFlush (hnd) != WCS_OK))
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
test_for_errors (W_CONNECTOR_HND hnd)
{
  unsigned long long count;

  cout << "Testing against error conditions ... ";

  if ((WPushStackValue (hnd, WFT_BOOL, 0, NULL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK) //Just for fun!
      || (WGetStackValueRowsCount (NULL, NULL) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (NULL, &count) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (hnd, NULL) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (hnd, &count) != WCS_TYPE_MISMATCH)
      || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, &count) != WCS_TYPE_MISMATCH)
      || (WGetStackArrayElementsCount (hnd, "some_f", WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, 0, &count) != WCS_INVALID_ROW)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_TYPE_MISMATCH)
      || (WGetStackTextLengthCount (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_OFF, 0, &count) != WCS_TYPE_MISMATCH)
      || (WPopStackValues (hnd, WPOP_ALL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
    {
      goto test_for_errors_fail;
    }

  cout << "OK\n";
  return true;

test_for_errors_fail :
  cout << "FAIL\n";
  return false;
}

const D_CHAR*
DefaultDatabaseName ()
{
  return "test_list_db";
}

const D_UINT
DefaultUserId ()
{
  return 1;
}

const D_CHAR*
DefaultUserPassword ()
{
  return "test_password";
}

int
main (int argc, const char** argv)
{
  W_CONNECTOR_HND hnd        = NULL;

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
