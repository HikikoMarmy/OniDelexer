#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include "math.h"

#define PRELEX_IDENTIFIER	0xFBFB

// Lexed file parser
class FileParser
{
public:
	FileParser()
	{
		_filePath.clear();
		_fileName.clear();

		_numByteOp = 0;
		_numFunction = 0;

		_curPos = 0;
		_fileSize = 0;
		_fileBuffer.clear();
	}

	~FileParser()
	{

	}

	bool Read( std::string file_path );


protected:

	// Internal Variables
	std::string _filePath;
	std::string _fileName;
	std::string _basePath;

	uint32_t _numByteOp, _numFunction;
	std::vector< uint8_t > _ByteOpBuffer;
	std::vector< std::string > _FunctionList;

	uint32_t _curPos, _fileSize;
	std::vector< uint8_t >	_fileBuffer;

	// Buffer Parsing
	template< typename R >
	R read_()
	{
		R ret = *( R* )& _fileBuffer.data()[ _curPos ];
		_curPos += sizeof( R );

		return byteswap( ret );
	}

	std::string read_szStr()
	{
		std::vector< uint8_t >::const_iterator start = _fileBuffer.begin() + static_cast< int >( _curPos );
		std::vector< uint8_t >::const_iterator end = start;

		while( *( end ) != 0 && end != _fileBuffer.end() )
		{
			end++;
		} 

		_curPos += std::distance( start, end ) + 1;

		return std::string( start, end );
	}
};

