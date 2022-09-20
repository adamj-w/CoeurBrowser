#include <iostream>

#include <coeur.h>

int main() {
	CoeurFile file = coeur_open("test.html", 0);

	//std::cout << coeur_get_file_contents(file) << "\n";

	auto rootNode = coeur_parse_file(file, 0);

	coeur_free_node(rootNode);

	coeur_close(file);
	return 0;
}