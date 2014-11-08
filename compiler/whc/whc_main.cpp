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

#include <iostream>
#include <sstream>
#include <assert.h>

#include "whais.h"

#include "compiler/compiledunit.h"

#include "utils/wfile.h"
#include "utils/endianness.h"
#include "utils/woutstream.h"
#include "utils/auto_array.h"

#include "msglog.h"
#include "whc_cmdline.h"
#include "whc_preprocess.h"
#include "wo_format.h"



using namespace std;
using namespace whais;
using namespace whais::whc;


uint8_t wh_header[WHC_TABLE_SIZE] = { 0, };


static void
fill_globals_table( WIFunctionalUnit&     unit,
                    struct WOutputStream* symbols,
                    struct WOutputStream* glblsTable)
{
  const uint_t globals_count = unit.GlobalsCount( );

  for (uint_t glbIt = 0; glbIt < globals_count; ++glbIt)
    {
      uint32_t glbTypeIndex = unit.GlobalTypeOff( glbIt);
      uint32_t glbNameIndex = wh_ostream_size( symbols);

      if (unit.IsGlobalExternal( glbIt))
        glbTypeIndex |= EXTERN_MASK;

      if ((wh_ostream_wint32 (glblsTable, glbTypeIndex) == NULL)
          || (wh_ostream_wint32 (glblsTable, glbNameIndex) == NULL)
          || (wh_ostream_write( symbols,
                                _RC (const uint8_t *,
                                     unit.RetriveGlobalName( glbIt)),
                                unit.GlobalNameLength( glbIt)) == NULL)
          || (wh_ostream_wint8 (symbols, 0) == NULL))
        {
          throw bad_alloc( );
        }
    }
}


static void
process_procedures_table( WIFunctionalUnit&   unit,
                          File&               destFile,
                          WOutputStream*      symbols,
                          WOutputStream*      procTable)
{
  const uint_t proc_count = unit.ProceduresCount( );

  for (uint_t procIt = 0; procIt < proc_count; ++procIt)
    {
      const uint16_t localsCount  = unit.ProcLocalsCount( procIt);
      const uint16_t paramsCound  = unit.ProcParametersCount( procIt);
      const uint32_t procOff      = destFile.Tell( );
      const uint32_t procCodeSize = unit.ProcCodeAreaSize( procIt);
      uint32_t       procRetType  = unit.GetProcReturnTypeOff( procIt);

      assert( localsCount >= paramsCound);

      for (uint_t localIt = 0; localIt < localsCount; ++localIt)
        {
          const uint32_t localTypeOff = unit.GetProcLocalTypeOff( procIt,
                                                                     localIt);
          uint8_t offset[sizeof( localTypeOff)];
          store_le_int32 (localTypeOff, offset);

          destFile.Write( offset, sizeof offset);
        }

      if (unit.IsProcExternal( procIt))
        procRetType |= EXTERN_MASK;
      else
        {
          uint8_t n_sync_stmts = unit.ProcSyncStatementsCount( procIt);
          destFile.Write( _RC (uint8_t *, &n_sync_stmts),
                           sizeof n_sync_stmts);
          destFile.Write( unit.RetriveProcCodeArea( procIt),
                           unit.ProcCodeAreaSize( procIt));
        }

      if ((wh_ostream_wint32 (procTable, wh_ostream_size( symbols)) == NULL)
          || (wh_ostream_wint32 (procTable, procOff) == NULL)
          || (wh_ostream_wint32 (procTable, procRetType) == NULL)
          || (wh_ostream_wint16 (procTable, localsCount) == NULL)
          || (wh_ostream_wint16 (procTable, paramsCound) == NULL)
          || (wh_ostream_wint32 (procTable, procCodeSize) == NULL))
        {
          throw bad_alloc( );
        }

      if ((wh_ostream_write( symbols,
                             _RC (const uint8_t *,
                                  unit.RetriveProcName( procIt)),
                             unit.GetProcNameSize( procIt)) == NULL)
          || (wh_ostream_wint8 (symbols, 0) == NULL))
        {
          throw bad_alloc( );
        }
    }
}


static void
create_object_file( const char* const                  outFile,
                    const string&                      sourceCode,
                    const vector<SourceCodeMark>&      codeMarks)
{
  uint_t        langVerMaj;
  uint_t        langVerMin;

  WOutputStream symbolsStream;
  WOutputStream glbsTableStream;
  WOutputStream procsTableStream;

  wh_compiler_language_ver( &langVerMaj, &langVerMin);

  wh_ostream_init( OUTSTREAM_INCREMENT_SIZE, &symbolsStream);
  wh_ostream_init( OUTSTREAM_INCREMENT_SIZE, &glbsTableStream);
  wh_ostream_init( OUTSTREAM_INCREMENT_SIZE, &procsTableStream);

  try
  {
    WHC_MESSAGE_CTX ctx (codeMarks, sourceCode.c_str( ));

    CompiledBufferUnit unit( _RC (const uint8_t*, sourceCode.c_str( )),
                             sourceCode.size( ),
                             whc_messenger,
                             &ctx);

    File outputObject( outFile, WH_FILEWRITE | WH_FILECREATE);

    outputObject.Size( 0);
    outputObject.Seek( 0, WH_SEEK_BEGIN);

    //reserve space for header file
    outputObject.Write( wh_header, sizeof wh_header);

    process_procedures_table( unit,
                              outputObject,
                              &symbolsStream,
                              &procsTableStream);
    fill_globals_table( unit, &symbolsStream, &glbsTableStream);

    store_le_int32 (wh_ostream_size( &glbsTableStream) / WHC_GLOBAL_ENTRY_SIZE,
                    wh_header + WHC_GLOBS_COUNT_OFF);

    assert( (wh_ostream_size( &glbsTableStream) % WHC_GLOBAL_ENTRY_SIZE) == 0);


    store_le_int32 (wh_ostream_size( &procsTableStream) / WHC_PROC_ENTRY_SIZE,
                    wh_header + WHC_PROCS_COUNT_OFF);

    assert( (wh_ostream_size( &procsTableStream) % WHC_PROC_ENTRY_SIZE) == 0);

    store_le_int32 (outputObject.Tell( ), wh_header + WHC_TYPEINFO_START_OFF);
    store_le_int32 (unit.TypeAreaSize( ),
                    wh_header + WHC_TYPEINFO_SIZE_OFF);

    outputObject.Write( unit.RetriveTypeArea( ),
                      unit.TypeAreaSize( ));

    store_le_int32 (outputObject.Tell( ), wh_header + WHC_SYMTABLE_START_OFF);
    store_le_int32 (wh_ostream_size( &symbolsStream),
                    wh_header + WHC_SYMTABLE_SIZE_OFF);

    outputObject.Write( wh_ostream_data( &symbolsStream),
                      wh_ostream_size( &symbolsStream));

    store_le_int32 (outputObject.Tell( ), wh_header + WHC_CONSTAREA_START_OFF);
    store_le_int32 (unit.ConstsAreaSize( ),
                    wh_header + WHC_CONSTAREA_SIZE_OFF);

    outputObject.Write( unit.RetrieveConstArea( ),
                      unit.ConstsAreaSize( ));
    outputObject.Write( wh_ostream_data( &glbsTableStream),
                      wh_ostream_size( &glbsTableStream));
    outputObject.Write( wh_ostream_data( &procsTableStream),
                      wh_ostream_size( &procsTableStream));

    wh_header[WHC_SIGNATURE_OFF]     = WH_SIGNATURE[0];
    wh_header[WHC_SIGNATURE_OFF + 1] = WH_SIGNATURE[1];
    wh_header[WHC_FORMATMMAJ_OFF]    = WH_FFVER_MAJ;
    wh_header[WHC_FORMATMIN_OFF]     = WH_FFVER_MIN;
    wh_header[WHC_LANGVER_MAJ_OFF]   = langVerMaj;
    wh_header[WHC_LANGVER_MIN_OFF]   = langVerMin;

    outputObject.Seek( 0, WH_SEEK_BEGIN);
    outputObject.Write( wh_header, sizeof wh_header);
    outputObject.Sync( );
  }
  catch( ...)
  {
      wh_ostream_clean( &symbolsStream);
      wh_ostream_clean( &glbsTableStream);
      wh_ostream_clean( &procsTableStream);

      throw ;
  }

  wh_ostream_clean( &symbolsStream);
  wh_ostream_clean( &glbsTableStream);
  wh_ostream_clean( &procsTableStream);
}


int
main( int argc, char **argv)
{
  int                        retCode  = 0;

  ostringstream              buffer;
  vector<SourceCodeMark>     codeMarks;
  vector<string>             usedFiles;

  try
  {
    CmdLineParser args( argc, argv);

    if (! preprocess_source( args.SourceFile( ),
                             args.InclusionPaths( ),
                             args.ReplacementTags( ),
                             buffer,
                             codeMarks,
                             usedFiles))
      {
        //In case of an error encountered during the processing stage its
        //corresponding message was already displayed.
        //Just return the error code here.
        return -1;
      }

    if (args.JustPreprocess( ))
      {
        cout << endl << buffer.str () << endl;

        return 0;
      }
    else if (args.BuildDependencies( ))
      {
        assert( usedFiles.size( ) > 0);

        cout << args.OutputFile( ) << " : ";
        for (size_t i = 0; i < usedFiles.size( ); ++i)
          {
            cout << usedFiles[i];
            if (i < usedFiles.size( ) - 1)
              cout << " \\\n ";

            else
              cout << endl << endl;
          }

        return 0;
      }

    create_object_file( args.OutputFile( ),
                        buffer.str (),
                        codeMarks);
  }
  catch( FileException & e)
  {
    std::cerr << "File IO error: " << e.Code( );

    if ( ! e.Message( ).empty( ))
      std::cerr << ": " << e.Message( ) << std::endl;

    else
      std::cerr << '.' << std::endl;

    retCode = -1;
  }
  catch( FunctionalUnitException&)
  {
    retCode = -1;
  }
  catch( CmdLineException& e)
  {
    std::cerr << e.Message( ) << std::endl;
  }
  catch( Exception& e)
  {
    std::cerr << "error: " << e.Message( ) << std::endl;
    std::cerr << "file:  " << e.File( ) << " : " << e.Line( ) << std::endl;
    std::cerr << "extra: " << e.Code( ) << std::endl;

    retCode = -1;
  }
  catch( std::bad_alloc&)
  {
    std::cerr << "Memory allocation failed!" << std::endl;

    retCode = -1;
  }
  catch( ...)
  {
    std::cerr << "Unknown exception thrown!" << std::endl;

    retCode = -1;
  }

  return retCode;
}


#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "WHC";
#endif

