#include <iostream>

#include <coeur.h>

int main() {
	// File test
	{
		CoeurObject file = coeur_open("test.html", 0);

		//std::cout << coeur_get_file_contents(file) << "\n";

		CoeurObject rootNode = coeur_parse_file(file, 0);
		if (!rootNode) {
			//std::cout << coeur_get_last_error() << "\n";
		}

		coeur_destroy(rootNode);

		coeur_destroy(file);
	}

	// Url test
	{
		CoeurObject url = coeur_create_url("http://192.168.1.251:80/test.html", 0);

		CoeurObject root = coeur_parse_from_url(url, 0);
		if (!root) {
			std::cout << coeur_get_last_error() << "\n";
		}

		coeur_destroy(root);

		coeur_destroy(url);
	}

	return 0;
}
