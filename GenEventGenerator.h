#pragma once
#include <iostream>

class GenEventGenerator : public eventGenerator
{
public:
	GenEventGenerator();
	void parseEvents(std::string message);
private:

};

