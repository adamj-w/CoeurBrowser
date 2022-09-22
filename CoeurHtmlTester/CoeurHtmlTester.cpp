#include <iostream>

#include <coeur.h>

int main() {
	// File test
	{
		CoeurFile file = coeur_open("test.html", 0);

		//std::cout << coeur_get_file_contents(file) << "\n";

		auto rootNode = coeur_parse_file(file, 0);
		if (!rootNode) {
			std::cout << coeur_get_last_error() << "\n";
		}

		coeur_free_node(rootNode);

		coeur_close(file);
	}

	// Url test
	{
		CoeurUrl url = coeur_create_url("http://192.168.1.251:80/test.html", 0);

		auto root = nullptr;// coeur_parse_from_url(url, 0);
		if (!root) {
			std::cout << coeur_get_last_error() << "\n";
		}

		coeur_free_node(root);

		coeur_destroy_url(url);
	}

	return 0;
}