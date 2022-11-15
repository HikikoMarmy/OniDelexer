#pragma once

#include <map>
#include <string>
#include "FileParser.h"
#include "FileWriter.h"

class LexiconInterpreter : public FileParser
{
public:
	LexiconInterpreter()
	{
	}

	~LexiconInterpreter()
	{
	}

	bool ProcessFile();
	std::stringstream& GetStream()
	{
		return _sstream;
	}

private:

	template <typename T>
	void WriteToStream( T value )
	{
		_sstream << value << " ";
	}

	void WriteToStream( std::string value )
	{
		_sstream << value << " ";
	}

	std::stringstream _sstream;

	std::vector< uint8_t >::const_iterator OpCodeIter;

	void __func_lookup_function();
	void __func_lookup_string();
	void __func_read_integer();
	void __func_read_float();
	void __func_newline_mult();
	void __func_constructor();
};

static std::map< uint8_t, std::string >	LEXICON_SYNTAX_LIST =
{
	{ 0x84, "delegate " },
	{ 0x85, "delete " },
	{ 0x86, "==" },
	{ 0x87, "!=" },
	{ 0x88, "<=" },
	{ 0x89, ">=" },
	{ 0x8A, "switch" },

	{ 0x8C, "&&" },
	{ 0x8D, "||" },
	{ 0x8E, "if" },
	{ 0x8F, "else" },
	{ 0x90, "while" },
	{ 0x91, "break" },
	{ 0x92, "for" },
	{ 0x93, "do" },
	{ 0x94, "null" },
	{ 0x95, "foreach" },
	{ 0x96, "in" },
	{ 0x97, "<-" },

	{ 0x99, "local" },
	{ 0x9A, "clone" },
	{ 0x9B, "function" },
	{ 0x9C, "return" },
	{ 0x9D, "typeof" },

	{ 0x9F, "+=" },
	{ 0xA0, "-=" },
	{ 0xA1, "continue" },
	{ 0xA2, "yield" },
	{ 0xA3, "try" },
	{ 0xA4, "catch" },
	{ 0xA5, "throw" },
	{ 0xA6, "<<" },
	{ 0xA7, ">>" },
	{ 0xA8, "resume" },
	{ 0xA9, "::" },
	{ 0xAA, "case" },
	{ 0xAB, "default" },
	{ 0xAC, "this" },
	{ 0xAD, "++" },
	{ 0xAE, "--" },
	{ 0xAF, "parent" },
	{ 0xB0, ">>>" },
	{ 0xB1, "class" },
	{ 0xB2, "extends" },

	{ 0xB5, "instanceof" },
	{ 0xB6, "..." },
	{ 0xB7, "vargc" },
	{ 0xB8, "vargv" },
	{ 0xB9, "true" },
	{ 0xBA, "false" },
	{ 0xBB, "*=" },
	{ 0xBC, "/=" },
	{ 0xBD, "%=" },

	{ 0xC8, "</" },
	{ 0xC9, "/>" },
	{ 0xCA, "static" },
	{ 0xCB, "enum" },
	{ 0xCC, "const" },
	{ 0xCD, "super" },
	{ 0xCE, "|=" },
	{ 0xCF, "&=" },
	{ 0xD0, "strict" },
	{ 0xD1, "removei" },

	{ 0xFD, "\n" }
};