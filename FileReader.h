#ifndef FILEREADER_H
#define FILEREADER_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
//#include <sstream>

class FileReader
{
public:
	std::string currentFileName;
	std::string* textFile;
	std::vector<std::ifstream> streams;
	std::map<std::string, std::string > storage;

public:
	FileReader();

	bool runCustomCommands();
	bool openFile(std::string fileName);
	bool switchCurrentFile(std::string fileName);
	bool closeFile(std::string fileName);
	bool closeCurrentFile();

	std::string currentFile();
	int lineCount();
	std::string getLine(int line);
	std::vector<std::string> getLines(int start, int end);
	std::vector<std::string> getElement(std::string elemType);
	std::vector<std::string> getElement(std::string elemType, int id);
	std::vector<std::string> getNextElement(std::string elemType);
	void resetElement(std::string elemType);

	std::string getElementAttribute(std::string elemType, std::string attrib);
	std::string getNextElementAttribute(std::string elemType, std::string attrib);
	std::string getElementAttribute(std::string elemType, int id, std::string attrib);
	std::string getNextElementAttribute(std::string elemType, int id, std::string attrib);
	void resetElementAttribute(std::string elemType, std::string attrib);
	void printDataToCMD(std::string* lines);
	bool overrideCurrentFile(std::string fileName, std::string* lines);


};
#endif