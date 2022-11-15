#include <iostream>
#include "LexiconInterpreter.h"

int main( int argc, char* argv[] )
{
	if( 1 == argc )
	{
		std::string directory = "./";
		for( std::filesystem::recursive_directory_iterator end, dir( directory ); dir != end; ++dir )
		{
			if( std::filesystem::is_regular_file( dir->path() ) )
			{
				std::string filename = dir->path().string();

				if( dir->path().string().find( "backup" ) != std::string::npos ) continue;

				if( dir->path().string().find( ".nut" ) != std::string::npos || ( dir->path().string().find( ".kwt" ) != std::string::npos ) || ( dir->path().string().find( ".tlks" ) != std::string::npos ) )
				{
					LexiconInterpreter Parser;
					FileWriter Writer;

					if( Parser.Read( dir->path().string() ) )
					{
						Parser.ProcessFile();

						Writer.Stylize( Parser.GetStream() );
						Writer.WriteToFile( dir->path().string() );
					}

				}
			}
		}
	}
	else
	{
		for( int i = 1; i < argc; i++ )
		{
			LexiconInterpreter Parser;
			FileWriter Writer;

			if( Parser.Read( argv[ i ] ) )
			{
				Parser.ProcessFile();

				Writer.Stylize( Parser.GetStream() );
				Writer.WriteToFile( argv[ i ] );
			}
		}
	}

	return 0;
}