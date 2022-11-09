#include <Server.hpp>
#include <FileParser.hpp>

int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	FileParser fileParser;
	fileParser.parse();

	return (0);
}