#include "FileWriter.h"
#include <fstream>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

void FileWriter::Stylize( std::stringstream& ss )
{
	Tokenize( ss );

	for( tokenIter = tokenList.begin(); tokenIter != tokenList.end(); tokenIter++ )
	{
		if( _indentNext )
		{
			Indentation();
			_indentNext = false;
		}

		if( ( *tokenIter ) == "\"" )
		{
			_stringQuoteOpen = !_stringQuoteOpen;
		}

		// Spacing logic
		DoSpacingLogic();

		// Indentation depth
		if( DoParenthesisLogic() ) continue;
		if( DoScopeLogic() ) continue;
		if( DoBracketLogic() ) continue;
		
		// If new line, setup next line indentation.
		if( ( *tokenIter ) == "\n" )
		{
			if( _stringQuoteOpen )
			{
				_stream << "\\n";
				_indentNext = true;
				continue;
			}
			else if( GetLastToken() == "return" )
			{
				int dbg = 0;
				continue;
			}
			else if( GetLastToken() == "else" && GetNextToken() == "if" )
			{
				//_stream << " ";
				continue;
			}
			else
			{
				if( GetNextToken() != "}" )
				{
					_indentNext = true;
				}

				_currentLine++;
				_currentLineDepth = 0;
			}
		}

		// Move 'else' and 'catch' to a new line.
		if( ( *tokenIter ) == "else" || ( * tokenIter ) == "catch" )
		{
			if( GetNextToken() != "if" && GetNextToken() != "\n" )
			{
				_stream << std::endl;
				Indentation();
			}

			_stream << ( *tokenIter );

			continue;
		}

		if( ( *tokenIter ) == "get" || ( *tokenIter ) == "put" )
		{
			if( ProcessGetPut() )
			continue;
		}

		_stream << ( *tokenIter );

		_currentLineDepth++;
	}

	LazyCleanUp();
}

void FileWriter::WriteToFile( std::string dir )
{
	char time_str[ 26 ];
	std::time_t current_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	ctime_s( time_str, sizeof time_str, &current_time );

	std::ofstream outfile( dir, std::ios::out );

	outfile << "// This file was created on " << time_str << "// Using Onigiri Delexer version " << DELEXER_VERSION << std::endl << std::endl;
	outfile << _stream.rdbuf();

	outfile.close();
}

void FileWriter::Tokenize( std::stringstream& ss )
{
	std::string str = ss.str();

	std::string const delims{ " " };

	size_t beg, pos = 0;
	while( ( beg = str.find_first_not_of( delims, pos ) ) != std::string::npos )
	{
		pos = str.find_first_of( delims, beg + 1 );
		std::string splitString = str.substr( beg, pos - beg );
		std::string token;

		for( auto c : splitString )
		{
			if( c == '{' || c == '}' ||
				c == '(' || c == ')' ||
				//c == '[' || c == ']' ||
				c == '\"' )
			{
				if( token.length() > 0 )
				{
					tokenList.push_back( token );
					token.clear();
				}

				char szChar[ 8 ];
				sprintf_s( szChar, 8, "%c", c );
				tokenList.push_back( szChar );
			}
			else
			{
				token.push_back( c );
			}
		}

		if( token.length() > 0 )
			tokenList.push_back( token );
	}
}

bool FileWriter::DoParenthesisLogic()
{
	if( _stringQuoteOpen ) return false;

	// Scope Open
	if( ( *tokenIter ) == "(" )
	{
		if( GetNextToken() == "\n" )
		{
			_parenthesisDepthState[ _parenthesisDepth ] = true;
			_stream << std::endl;
			Indentation();

			_indentDepth++;

			_currentLine++;
			_currentLineDepth = 0;
			_indentNext = false;
		}
		else
		{
			_parenthesisDepthState[ _parenthesisDepth ] = false;
		}

		_parenthesisDepth++;

		_stream << "(";
		return true;
	}
	// Scope Close
	else if( ( *tokenIter ) == ")" )
	{
		if( _parenthesisDepth > 0 )
		{
			_parenthesisDepth--;

			if( true == _parenthesisDepthState[ _parenthesisDepth ] )
			{
				_parenthesisDepthState[ _parenthesisDepth ] = false;
				_indentDepth--;
			}
		}

		_stream << ")";

		return true;
	}

	return false;
}

bool FileWriter::DoScopeLogic()
{
	if( _stringQuoteOpen ) return false;

	// Scope Open
	if( ( *tokenIter ) == "{" )
	{
		if( GetNextToken() == "}" )
		{
			_stream << "{";
			return true;
		}
		else
		{
			//if( GetLastToken() != "\n" )
			//{
			//	_stream << std::endl;
			//}

			//_currentLine++;
			//_currentLineDepth = 0;
			//_indentNext = false;

			//Indentation();
			_stream << "{";
			_indentDepth++;
			if( GetNextToken() != "\n" )
			{
				_stream << std::endl;
				Indentation();
			}

			return true;
		}
	}
	// Scope Close
	else if( ( *tokenIter ) == "}" )
	{
		if( GetLastToken() != "\n" && GetLastToken() != "{" )
		{
			_stream << std::endl;
		}

		if( GetLastToken() != "{" )
		{
			_indentDepth--;
		}

		Indentation();
		_stream << "}";
		

		return true;
	}

	return false;
}

bool FileWriter::DoBracketLogic()
{
	if( _stringQuoteOpen ) return false;

	//if( ( *tokenIter != "[" ) ) return false;

	// Bracket Open
	if( ( *tokenIter ) == "[" )
	{
		if( GetNextToken() == "\n" )
		{
			_bracketDepthState[ _squareBracketDepth ] = true;
			_stream << std::endl;
			Indentation();

			_indentDepth++;

			_currentLine++;
			_currentLineDepth = 0;
			_indentNext = false;
		}
		else
		{
			_bracketDepthState[ _squareBracketDepth ] = false;
		}

		_squareBracketDepth++;

		_stream << "[";
		return true;
	}
	// Bracket Close
	else if( ( *tokenIter ) == "]" )
	{
		if( _squareBracketDepth > 0 )
		{
			_squareBracketDepth--;

			if( true == _bracketDepthState[ _squareBracketDepth ] )
			{
				_bracketDepthState[ _squareBracketDepth ] = false;
				_indentDepth--;
			}
		}

		_stream << "]";

		return true;
	}

	return false;
}

bool FileWriter::DoSwitchLogic()
{
	if( ( *tokenIter ) != "switch" ) return false;



}

void FileWriter::DoSpacingLogic()
{
	std::string CurrToken = ( *tokenIter );
	std::string NextToken = GetNextToken();
	std::string LastToken = GetLastToken();

	if( _currentLineDepth == 0 ) return;
	if( LastToken == "!" ) return;
	if( CurrToken == "\"" && false == _stringQuoteOpen ) return;
	if( LastToken == "\"" && true == _stringQuoteOpen ) return;
	if( CurrToken == "\\\"" || LastToken == "\\\"" ) return;
	if( CurrToken == "." || LastToken == "." ) return;
	if( CurrToken == "," ) return;
	if( CurrToken == ";" ) return;
	if( LastToken == "::" ) return;
	if( CurrToken == "(" )
	{
		if( LastToken != ":" && LastToken != "=" ) return;
	}


	if( CurrToken == "[" && NextToken != "]" ) return;
	if( CurrToken == "]" && LastToken == "[" ) return;
	if( IsArithmeticOperator( CurrToken ) && ( IsNumber( LastToken ) ) ) return;
	if( IsArithmeticOperator( LastToken ) && ( IsNumber( CurrToken ) ) ) return;

	if( LastToken != "\n" )
	{
		if( _currentLineDepth > 0 )
		{
			_stream << " ";
		}
	}
}