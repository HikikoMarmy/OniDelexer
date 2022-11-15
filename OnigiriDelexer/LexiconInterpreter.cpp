#include "LexiconInterpreter.h"
#include "FileWriter.h"

bool LexiconInterpreter::ProcessFile()
{
	std::map< uint8_t, std::string >::const_iterator LexiconIter;

	for( OpCodeIter = _ByteOpBuffer.begin(); OpCodeIter != _ByteOpBuffer.end(); )
	{
		switch( ( *OpCodeIter ) )
		{
			// Edge case Operations
			case 0x80: __func_lookup_function(); break;
			case 0x81: __func_lookup_string(); break;
			case 0x82: __func_read_integer(); break;
			case 0x83: __func_read_float(); break;
			case 0xB4: __func_constructor(); break;
			case 0xFE: __func_newline_mult(); break;

			// General Syntax Operations
			default:
			{
				// Look up generic op codes
				if( LEXICON_SYNTAX_LIST.end() != ( LexiconIter = LEXICON_SYNTAX_LIST.find( ( *OpCodeIter ) ) ) )
				{
					WriteToStream( ( *LexiconIter ).second );
				}
				else
				{
					// Push the raw byte
					WriteToStream( ( *OpCodeIter ) );
				}

				OpCodeIter++;
			} break;
		}
	}

	return true;
}

void LexiconInterpreter::__func_lookup_function()
{
	OpCodeIter++;

	int16_t FunctionIndex = byteswap( *( int16_t* )&( *OpCodeIter ) );

	WriteToStream( _FunctionList[ FunctionIndex ] );

	OpCodeIter += 2;
}

void LexiconInterpreter::__func_lookup_string()
{
	OpCodeIter++;

	int16_t FunctionIndex = byteswap( *( int16_t* )&( *OpCodeIter ) );

	WriteToStream( '\"' + _FunctionList[ FunctionIndex ] + '\"' );

	OpCodeIter += 2;
}

void LexiconInterpreter::__func_read_integer()
{
	OpCodeIter++;

	union
	{
		int32_t asi32;
		uint8_t b[ 4 ];
	} ui;

	ui.asi32 = byteswap( *( int32_t* )&( *OpCodeIter ) );

	WriteToStream( std::to_string( ui.asi32 ) );

	OpCodeIter += 4;
}

void LexiconInterpreter::__func_read_float()
{
	OpCodeIter++;

	union
	{
		uint32_t asi32;
		float_t asFloat;
	} uf;

	uf.asi32 = byteswap( *( uint32_t* )&( *OpCodeIter ) );

	std::string str = std::to_string( uf.asFloat );

	//Trim trailing 0's in floating point precision
	if( str.find( '.' ) != std::string::npos )
	{
		str = str.substr( 0, str.find_last_not_of( '0' ) + 1 );

		if( str.find( '.' ) == str.size() - 1 )
		{
			str = str.substr( 0, str.size() - 1 );

			if( str.length() == 1 )
			{
				str += ".0";
			}
		}
	}

	WriteToStream( str );

	OpCodeIter += 4;
}

void LexiconInterpreter::__func_newline_mult()
{
	OpCodeIter++;
	uint8_t numNewLine = *( uint8_t* )&( *OpCodeIter );

	//Decided 2 was a good amount for readability.
	for( uint8_t i = 0; i < 2; i++ )
	{
		WriteToStream( '\n' );
	}
	OpCodeIter++;
}

void LexiconInterpreter::__func_constructor()
{
	WriteToStream( std::string( "constructor" ) );
	OpCodeIter += 3;	//Constructor op has 2 bytes of garbage following it.
}