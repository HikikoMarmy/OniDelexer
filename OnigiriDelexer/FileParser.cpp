
#include "FileParser.h"
#include <regex>

namespace fs = std::filesystem;

bool FileParser::Read( std::string dir )
{
    _filePath = dir;
    _fileName = fs::path( dir ).filename().string();
    _basePath = fs::path( dir ).parent_path().string();

    // Load the file as binary
    std::fstream file_stream( _filePath, std::ios::binary | std::ios::in );

    if( !file_stream.is_open() )
    {
        printf( "[Failed to load file] %s\n", dir.c_str() );
        return false;
    }

    // Get the file size
    file_stream.seekg( 0, file_stream.end );
    _fileSize = static_cast< uint32_t >( file_stream.tellg() );
    file_stream.seekg( 0, file_stream.beg );

    // Allocate space on the buffer and read the contents
    _fileBuffer.resize( static_cast< uint32_t >( _fileSize ) );
    file_stream.read( reinterpret_cast< char* >( _fileBuffer.data() ), _fileSize );

    // Close the file stream
    file_stream.close();

    if( _fileSize < 8 ) return false;

    // Verify that the files header ident is correct
    if( PRELEX_IDENTIFIER != read_< uint16_t >() )
    {
        printf( "[File Identifier mismatch] %s\n", _fileName.c_str() );
        return false;
    }

    printf( "Processing File %s... \n", _fileName.c_str() );

    if( 0 == ( _numByteOp = read_< uint32_t >() ) ) return false;
    if( 0 == ( _numFunction = read_< uint16_t >() ) ) return false;

    // Copy the op codes into the buffer
    _ByteOpBuffer = std::vector< uint8_t >( ( _fileBuffer.begin() + 8 ), ( _fileBuffer.begin() + 8 + _numByteOp ) );

    // Read the function list
    _curPos = ( _numByteOp + 8 );
    for( uint32_t i = 0; i < _numFunction; i++ )
    {
        std::string ret = read_szStr();
        std::string s = "";

        // Clean up escape characters
        // It's important to do it this way due to the way the tokenizer works.
        ret = std::regex_replace( ret, std::regex( R"(\")" ), "@~ESCDQUOTE~@" );
        ret = std::regex_replace( ret, std::regex( R"(\f)" ), "@~ESCFORMFEED~@" );
        ret = std::regex_replace( ret, std::regex( R"(\n)" ), "@~ESCNEWLINE~@" );
        ret = std::regex_replace( ret, std::regex( R"(\r)" ), "@~ESCRETURN~@" );
        ret = std::regex_replace( ret, std::regex( R"(\t)" ), "@~ESCHTAB~@" );
        ret = std::regex_replace( ret, std::regex( R"(\v)" ), "@~ESCVTAB~@" );
        ret = std::regex_replace( ret, std::regex( R"(\\)" ), "@~ESCBACKSLASH~@" );

        // Verified and correct escape character.
        ret = std::regex_replace( ret, std::regex( "\x1B" ), "\\x1B");

        _FunctionList.push_back( ret );
    }

#ifndef _DEBUG
    // Make a backup of the original if it doesn't exist.
    fs::create_directory( "./backup" );
    char szBackupFile[ 256 ];
    sprintf_s( szBackupFile, 256, "./backup/%s", _fileName.c_str() );
    if( false == fs::exists( szBackupFile ) )
    {
        std::ofstream backupFile( szBackupFile, std::ios::binary | std::ios::out );

        if( false == backupFile.is_open() )
        {
            printf( "[Failed to create backup file] %s", szBackupFile );
            return false;
        }

        backupFile.write( (char*)_fileBuffer.data(), _fileSize );
        backupFile.close();
    }
#endif
    
    return true;
}