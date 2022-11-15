#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <map>


#define DELEXER_VERSION "0.3a"

class FileWriter
{
public:
	FileWriter()
	{
		_indentDepth = 0;
		_parenthesisDepth = 0;
		_squareBracketDepth = 0;
		_switchCaseIndent = false;
		_defaultCaseIndent = false;
		_stringQuoteOpen = false;
	}

	~FileWriter()
	{
		_indentDepth = 0;
	}

	void Stylize( std::stringstream& ss );

	void WriteToFile( std::string dir );

private:
	bool _switchCaseIndent;
	bool _defaultCaseIndent;
	bool _indentNext;
	bool _indentCurrent;
	bool _stringQuoteOpen;
	int32_t _indentDepth;
	int32_t _parenthesisDepth;
	int32_t _squareBracketDepth;

	int32_t _currentLine;
	int32_t _currentLineDepth;

	std::stringstream& GetStream()
	{
		return _stream;
	}

	void Tokenize( std::stringstream& ss );

	std::string GetNextToken()
	{
		if( ( tokenIter + 1 ) == tokenList.end() )
		{
			return "";
		}

		return *( tokenIter + 1 );
	}

	std::string GetLastToken()
	{
		if( tokenIter == tokenList.begin() )
		{
			return (*tokenList.begin());
		}

		return *( tokenIter-1 );
	}

	void Indentation()
	{
		for( uint8_t i = 0; i < _indentDepth; i++ )
		{
			_stream << '\t';
		}
	}

	bool ProcessGetPut()
	{
		if( GetNextToken() != "(" ) return false;
		if( GetLastToken() != "." ) return false;

		for( ; tokenIter != tokenList.end(); ++tokenIter )
		{
			uint32_t asInteger = std::atoi( ( *tokenIter ).c_str() );
			if( std::in_range< uint8_t >( asInteger ) && std::isalpha( asInteger ) )
			{
				switch( asInteger )
				{
					case 'l':	//	int32
					case 'i':	//	uint32
					case 's':	//	int16
					case 'w':	//	uint16
					case 'c':	//	int8
					case 'b':	//	uint8
					case 'f':	//	float
					case 'd':	//	double
					case 't':	//	string
					{
						char szChar[ 8 ];
						sprintf_s( szChar, 8, "\'%c\'", asInteger );
						_stream << std::string( szChar );
					} break;

					default:
					{
						_stream << std::to_string( asInteger );
					} break;
				}
			}
			else
			{
				if( ( *tokenIter ) == ")" || ( *tokenIter ) == ">>" )
				{
					_stream << " ";
				}

				_stream << ( *tokenIter );

				if( ( *tokenIter ) == "," || ( *tokenIter ) == "(" || ( *tokenIter ) == ">>" )
				{
					_stream << " ";
				}
			}

			if( ( *tokenIter ) == ";" )
			{
				break;
			}
		}

		return true;
	}

	bool DoStringLogic();
	bool DoParenthesisLogic();
	bool DoScopeLogic();
	bool DoBracketLogic();
	bool DoSwitchLogic();

	void DoSpacingLogic();

	bool IsArithmeticOperator( std::string op )
	{
		return ( 
			op == "--" ||
			op == "++" ||
			op == "-" ||
			op == "+" ||
			op == "*" ||
			op == "%" );
	}

	bool IsComparisonOperator( std::string op )
	{
		return (
			op == ">" ||
			op == "<" ||
			op == "==" ||
			op == "!=" );
	}

	bool IsNumber( std::string s )
	{
		return !s.empty() &&
			std::find_if
			(
				s.begin(),
				s.end(), []( unsigned char c )
				{
					return !std::isdigit( c );
				}
			) == s.end();
	}

	void LazyCleanUp()
	{
		std::string str = _stream.str();

		str = std::regex_replace( str, std::regex( R"(@~ESCDQUOTE~@)" ), "\\\"" );
		str = std::regex_replace( str, std::regex( R"(@~ESCFORMFEED~@)" ), "\\f" );
		str = std::regex_replace( str, std::regex( R"(@~ESCNEWLINE~@)" ), "\\n" );
		str = std::regex_replace( str, std::regex( R"(@~ESCRETURN~@)" ), "\\r" );
		str = std::regex_replace( str, std::regex( R"(@~ESCHTAB~@)" ), "\\t" );
		str = std::regex_replace( str, std::regex( R"(@~ESCVTAB~@)" ), "\\v" );

		str = std::regex_replace( str, std::regex( R"(@~ESCBACKSLASH~@)" ), "\\\\" );

		while( ( *str.begin() ) == '\n' )
		{
			str.erase( str.begin() );
		}

		_stream.str( str );
	}

	std::vector< std::string >::const_iterator tokenIter;
	std::vector< std::string > tokenList;
	std::stringstream _stream;

	std::map< uint8_t, bool >	_parenthesisDepthState;
	std::map< uint8_t, bool >	_bracketDepthState;
};

