#include <iostream>
#include "TinyServer.h"


int main() {
	TinyServer tinyServer;
	if (!tinyServer.init(5, "127.0.0.1", 8888))
		return 0;

	return 1;
}