#include "FileReader.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <sstream>

using namespace std;

bool FileReader::runCustomCommands()
{
	//FileReader >> Properties >> Debugging >> Command Arguments <std/adv> >> Run

	//Asks for user to input command
	cout << "Welcome to FileReader. Type a command to manipulate files. For information concerning commands, type 'help'.\n";
	string command;
	getline(cin, command);

	string commandDelimiter = " ";
	int position = 0;

	//Splits user input into command & any following arguments
	vector<string> commandArguments;

	while ((position = command.find(commandDelimiter)) != string::npos) //probs check for multiple spaces
	{
		commandArguments.push_back(command.substr(0, position)); //Add argument to command argument list (maybe use push_front)
		command.erase(0, position + commandDelimiter.length());
	}
	if (!commandArguments.empty()) //If commandArguments isn't empty, that means it has been used, but also that the last argument has yet to be added (as it doesn't end with a new line char)
	{
		commandArguments.push_back(command.substr(0));
	}
	else //no command arguments
	{
		if (command == "help")
		{
			cout << "Available commands:\nhelp | For information concerning commands.\nopenfiles <amount (int)> | Opens a dynamically specifiable amount of files of the same type.\nclosefiles | Closes any files that are currently open.\n";
		}
		else if (command == "closefiles")
		{
			//_fcloseall();
			cout << "Any opened files were closed.\n";
		}
		else if (command == "closefile")
		{
			bool isFileAlreadyOpen = false;
			for (pair<const string, const string> filePath : storage)
			{
				if (currentFileName == filePath.first) //comparing against key, not value
				{
					isFileAlreadyOpen = true;
					break;
				}
			}

			if (isFileAlreadyOpen)
			{
				closeCurrentFile();
				runCustomCommands();
				return false;
			}

			cout << "'closefile' Test\n";
		}
		else if (command == "outputspec")
		{

		}
		else if (command == "openfile")
		{
			"You did not provide a file path. In order to open a file, input a file path with the command 'openfile <filePath>'\n";
		}
		else if (command == "displayfile")
		{
			if (!streams.empty())
			{
				string textFileToModify = "";
				for (pair<const string, const string> filePath : storage)
				{
					if (currentFileName == filePath.first)
					{
						ostringstream stringStream;
						int fileStreamIndex = stoi(filePath.second);
						stringStream << streams[fileStreamIndex].rdbuf();

						string textFileToModify = stringStream.str(); //String variable of stream of text

						textFile = &textFileToModify; //Sets object var pointer to textFileToModify
						break;
					}
				}

				printDataToCMD(&textFileToModify);
			}
		}
	}
	vector<string>::iterator argument = next(commandArguments.begin(), 0);
	if (commandArguments.size() == 2) //command arguments present
	{
		if (*argument == "openfile")
		{
			argument++;
			string filePath = *argument;

			bool isFileAlreadyOpen = false;
			for (pair<const string, const string> filePath2 : storage)
			{
				if (filePath == filePath2.first) //comparing against key, not value
				{
					isFileAlreadyOpen = true;
				}
			}

			if (isFileAlreadyOpen)
			{
				cout << "File already open, so it cannot be opened again.\n";
				runCustomCommands();
				return false;
			}

			if (!filePath.empty())
			{
				cout << "FilePath test: " + filePath + "\n";

				string fileExtension = filePath;
				commandDelimiter = ".";
				if (position = fileExtension.find(commandDelimiter)) //Cuts off everything before & including the dot before file type
				{
					fileExtension.erase(0, position + commandDelimiter.length());
				}
				if (!fileExtension.empty())
				{
					if (fileExtension == "txt" || fileExtension == "obj" || fileExtension == "gltf" || fileExtension == "mtl" || fileExtension == "dae")
					{
						if (openFile(filePath) == true)
						{
							position = 0;
							string fileName = filePath;
							while ((position = fileName.find("\\")) != string::npos) //Gets file name (with extension) for user feedback
							{
								fileName.erase(0, position + commandDelimiter.length());
							}

							cout << "File '" + fileName + "' " + "open for future use.\n";
						}
					}
				}
				else
				{
					cout << "The file format '" + fileExtension + "' " + "is not an accepted file format in FileReader.\n";
				}
			}
		}
		else if (*argument == "closefile")
		{
			argument++;
			string filePath = *argument;

			bool isFileAlreadyOpen = false;
			for (pair<const string, const string> filePath2 : storage) //Loops through every file path until a match is found
			{
				if (filePath == filePath2.first) //comparing against key, not value
				{
					isFileAlreadyOpen = true;
					break;
				}
			}

			if (isFileAlreadyOpen) //Closes file if currently open
			{
				closeFile(filePath);
				runCustomCommands();
				return false;
			}
		}
		if (*argument == "switchfile")
		{
			argument++;
			string filePath = *argument;

			if (!switchCurrentFile(filePath))
			{
				runCustomCommands();
				return false;
			}
			runCustomCommands();
			return true;
		}
		else if (*argument == "displayfile")
		{
			argument++;

			if (*argument == "linecount")
			{
				for (pair<const string, const string> filePath : storage)
				{
					if (currentFileName == filePath.first)
					{
						int fileStreamIndex = stoi(filePath.second);
						
						if (streams[fileStreamIndex].is_open())
						{
							int linesCounted = lineCount();
							break;
						}
						else
						{
							cout << "No line count could be obtained since no file is currently open & selected.\n";
							runCustomCommands();
							return false;
						}
					}
				}
			}
		}
	}
	else if (commandArguments.size() == 3)
	{
		if (*argument == "displayfile")
		{
			argument++;
			if (*argument == "getline")
			{
				argument++;

				int lineSelected = stoi(*argument);
				string selectedLineContents = getLine(lineSelected);
				cout << "Line contents: " + selectedLineContents + "\n";
			}
		}
		else if (*argument == "inspect")
		{
			argument++;
			if (*argument == "element")
			{
				vector<string> element;
				
				argument++;
				if (*argument == "object")
				{
					element = getElement("object");
				}
				else
				{
					cout << "File type '" + *argument + "' unsupported.\n";
				}
			}
			else if (*argument == "nextelement")
			{
				vector<string> element;

				argument++;
				if (*argument == "object")
				{
					element = getNextElement("object");
				}
				else
				{
					cout << "File type '" + *argument + "' unsupported.\n";
				}
			}
		}
		else if (*argument == "reset")
		{
			argument++;
			if (*argument == "element")
			{
				argument++;
				if (*argument == "object")
				{
					resetElement("object");
				}
				else
				{
					cout << "File type '" + *argument + "' unsupported.\n";
				}
			}
		}
	}
	else if (commandArguments.size() == 4)
	{
		if (*argument == "displayfile")
		{
			argument++;
			if (*argument == "getlines")
			{
				int lineStart;
				int lineEnd;
				bool lineBoundsInputSucceeded = false;
				try
				{
					argument++;
					lineStart = stoi(*argument);
					argument++;
					lineEnd = stoi(*argument);
					lineBoundsInputSucceeded = true;
				}
				catch (exception e)
				{
					cout << "At least one of the line bounds was not a valid input, so could not perform operation.\n";
				}
				if (lineBoundsInputSucceeded)
				{
					getLines(lineStart, lineEnd);
				}
			}
		}
		else if (*argument == "inspect")
		{
			argument++;
			if (*argument == "elementattribute")
			{
				string attribute;

				argument++;
				if (*argument == "object")
				{
					argument++;
					if (*argument == "vertex")
					{
						attribute = getElementAttribute("object", "vertex");
					}
				}
				else if (*argument == "vertex")
				{
					argument++;
					if (*argument == "vertex-texture")
					{
						attribute = getElementAttribute("vertex", "vertex-texture");
					}
					else if (*argument == "vertex-normal")
					{
						attribute = getElementAttribute("vertex", "vertex-normal");
					}
				}
				else
				{
					cout << "File type '" + *argument + "' unsupported.\n";
				}
			}
			else if (*argument == "nextelementattribute")
			{
				string attribute;

				argument++;
				if (*argument == "object")
				{
					argument++;
					if (*argument == "vertex")
					{
						attribute = getNextElementAttribute("object", "vertex");
					}
				}
				else if (*argument == "vertex")
				{
					argument++;
					if (*argument == "vertex-texture")
					{
						attribute = getNextElementAttribute("vertex", "vertex-texture");
					}
					else if (*argument == "vertex-normal")
					{
						attribute = getNextElementAttribute("vertex", "vertex-normal");
					}
				}
				else
				{
					cout << "File type '" + *argument + "' unsupported.\n";
				}
			}
		}
		else if (*argument == "reset")
		{
			argument++;
			if (*argument == "elementattribute")
			{
				argument++;

				if (*argument == "object")
				{
					argument++;
					if (*argument == "vertex")
					{
						resetElementAttribute("object", "vertex");
					}
				}
				else if (*argument == "vertex")
				{
					argument++;
					if (*argument == "vertex-texture")
					{
						resetElementAttribute("vertex", "vertex-texture");
					}
					else if (*argument == "vertex-normal")
					{
						resetElementAttribute("vertex", "vertex-normal");
					}
				}
			}
			else
			{
				cout << "File type '" + *argument + "' unsupported.\n";
			}
		}
	}
	else
	{
		cout << "The command " + command + " was not recognised. Try a different command or type 'help' for information concerning commands.\n";
	}
	runCustomCommands();
	return false;
}

FileReader::FileReader() {
	currentFileName = "";
	textFile = nullptr;
	streams = std::vector<std::ifstream>();

}


// returns if the file could be opened successfully
bool FileReader::openFile(std::string fileName) {

	try
	{
		//Opening as read-only just for keeping open
		ifstream inputFileStream;
		inputFileStream.open(fileName);

		if (!inputFileStream)
		{
			cout << "The specified file didn't exist, nothing was opened.\n";
			return false;
		}

		streams.push_back(std::move(inputFileStream)); //moves inputFileStream into vector; if passed by reference, will be deleted early & result in nullptr (old version; doesn't fit requirements)

		string streamsIndex = to_string(streams.size() - 1);
		storage.emplace(fileName, streamsIndex);

		currentFileName = fileName;
	}
	catch (exception& e)
	{
		cout << e.what();
		return false;
	}
	cout << "File '" + fileName + "' " + "retrieved.\n";
	return true;
}
std::string FileReader::currentFile() {
	return currentFileName;
}

int FileReader::lineCount()
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	string textFileToModify; //for some reason the value is 'forgotten' even though it isn't a pointer, so goes here
	if (isFileAlreadyOpen)
	{
		string textFileToModify = "";
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				ostringstream stringStream;
				int fileStreamIndex = stoi(filePath.second);

				try
				{
					stringStream << streams[fileStreamIndex].rdbuf();
					streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream
				}
				catch (exception e)
				{
					e.what();
				}

				textFileToModify = stringStream.str(); //String reference to stream of text

				textFile = &textFileToModify; //Sets object var pointer to textFileToModify
				break;
			}
		}

		if (textFileToModify != "")
		{
			int lineCount = 0;
			for (int i = 0; i < textFileToModify.size(); i++)
			{
				if (textFileToModify[i] == '\n') //Adds 1 per new line character found
				{
					lineCount++;
				}
			}

			if (lineCount > 0) //last line added, only if any lines were present prior
			{
				lineCount++;
			}

			string linesDisplay = to_string(lineCount);
			cout << linesDisplay + "\n";
			return lineCount;
		}
	}

	return -1;
}

std::string FileReader::getLine(int line)
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	string textFileToModify; //for some reason the value is 'forgotten' even though it isn't a pointer, so goes here
	if (isFileAlreadyOpen)
	{
		textFileToModify = "";
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				ostringstream stringStream;
				int fileStreamIndex = stoi(filePath.second);

				stringStream << streams[fileStreamIndex].rdbuf();
				streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream

				textFileToModify = stringStream.str(); //String variable to stream of text

				textFile = &textFileToModify; //Sets object var pointer to textFileToModify
				break;
			}
		}

		if (textFileToModify != "");
		{
			string selectedLine;
			string commandDelimiter = "\n";
			int position = 0;

			for (int i = 0; ((position = textFileToModify.find(commandDelimiter)) != string::npos && (i <= textFileToModify.size())); i++)
			{
				if (line == i) //Checks if chosen line matches
				{
					selectedLine = textFileToModify.substr(0, position); //retrieves contents of line
					return selectedLine;
				}
				textFileToModify.erase(0, position + commandDelimiter.length());
			}
		}
	}

	cout << "Failed to acquire the specified line.\n";
	return "";
}

std::vector<std::string> FileReader::getLines(int start, int end)
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}
	//isFileAlreadyOpen = false;

	string textFileToModify; //for some reason the value is 'forgotten' even though it isn't a pointer, so goes here
	if (isFileAlreadyOpen)
	{
		textFileToModify = "";
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				ostringstream stringStream;
				int fileStreamIndex = stoi(filePath.second);

				stringStream << streams[fileStreamIndex].rdbuf();
				streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream

				textFileToModify = stringStream.str(); //String variable to stream of text

				textFile = &textFileToModify; //Sets object var pointer to textFileToModify
				break;
			}
		}

		if (textFileToModify != "")
		{
			vector<string> selectedLines;

			string commandDelimiter = "\n";
			int position = 0;

			int i;
			for (i = 0; ((position = textFileToModify.find(commandDelimiter)) != string::npos) && (i <= end); i++) //Ends at last line to select
			{
				if (start <= i) //Checks if 1st line to return has been reached
				{
					selectedLines.push_back(textFileToModify.substr(0, position)); //Add line to list of lines to select
				}
				textFileToModify.erase(0, position + commandDelimiter.length());
			}
			return selectedLines;
		}
	}

	cout << "Failed to acquire all lines in range.\n";
	return std::vector<std::string>();
}

std::vector<std::string> FileReader::getElement(std::string elemType)
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	string textFileToModify; //for some reason the value is 'forgotten' even though it isn't a pointer, so goes here
	if (isFileAlreadyOpen)
	{
		textFileToModify = "";
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);

				vector<string> selectedElement;
				string commandDelimiter = "\n";
				int position = 0;

				streams[fileStreamIndex].clear(); //Clears stream in the case that nothing was found & the 'cursor' resultingly 'fell off' the end of the stream; if not cleared, cannot be reset to start
				streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream
				string lineToCheck;

				while (getline(streams[fileStreamIndex], lineToCheck)) //Gets each line & returns result into 'lineToCheck'
				{
					if (lineToCheck[0] == 'o') //checks for object prefix 'o'
					{
						selectedElement.push_back(lineToCheck); //returns object name
						selectedElement[0].erase(0, 2); //trimming prefix

						cout << "Selected Element: " + selectedElement[0] + "\n";
						return selectedElement;
					}
				}
				//streams[fileStreamIndex].clear(); //Clears stream in the case that nothing was found & the 'cursor' resultingly 'fell off' the end of the stream; if not cleared, cannot be reset to start
				selectedElement.push_back("");
				return selectedElement;
			}
		}
	}

	cout << "Failed to acquire the specified element type.\n";

	return std::vector<std::string>();
}

std::vector<std::string> FileReader::getElement(std::string elemType, int id)
{
	return std::vector<std::string>();
}

std::vector<std::string> FileReader::getNextElement(std::string elemType)
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	string textFileToModify; //for some reason the value is 'forgotten' even though it isn't a pointer, so goes here
	if (isFileAlreadyOpen)
	{
		textFileToModify = "";
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);

				vector<string> selectedElement;
				string commandDelimiter = "\n";
				int position = 0;

				if (streams[fileStreamIndex].bad() || streams[fileStreamIndex].fail() || !streams[fileStreamIndex].good())
				{
					streams[fileStreamIndex].clear();
				}

				//Continues reading at the last position accessed from the stream
				string lineToCheck;
				while (getline(streams[fileStreamIndex], lineToCheck))
				{
					if (lineToCheck[0] == 'o')
					{
						selectedElement.push_back(lineToCheck);
						selectedElement[0].erase(0, 2); //trimming prefix

						cout << "Selected Element: " + selectedElement[0] + "\n";
						return selectedElement;
					}
				}
				//streams[fileStreamIndex].clear();
				selectedElement.push_back("");
				return selectedElement;
			}
		}
	}

	cout << "Failed to acquire the specified element type.\n";

	return std::vector<std::string>();
}

void FileReader::resetElement(std::string elemType)
{

	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	if (isFileAlreadyOpen)
	{
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);
				streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream
				cout << "Cursor reset successfully.\n";
				break;
			}
		}
	}
	else
	{
		cout << "File was not open, so no cursor could be reset.\n";
	}
}

std::string FileReader::getElementAttribute(std::string elemType, std::string attrib)
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	if (isFileAlreadyOpen)
	{
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);

				string selectedElement = "";
				string commandDelimiter = "\n";
				int position = 0;

				string lineToCheck;
				int originalIndex = streams[fileStreamIndex].tellg();
				bool elementFound = false;

				int streamIndex = originalIndex;
				vector<int> potentialElementIndexes;
				if (elemType == "object")
				{
					streams[fileStreamIndex].seekg(0);

					int currentIndex = 0;
					while (streams[fileStreamIndex].tellg() < originalIndex)
					{
						getline(streams[fileStreamIndex], lineToCheck);

						if (lineToCheck[0] == 'o')
						{
							elementFound = true;
							int currentIndex = streams[fileStreamIndex].tellg();
							potentialElementIndexes.push_back(currentIndex);
						}
					}

					if (elementFound == true)
					{
						streams[fileStreamIndex].seekg(potentialElementIndexes.back());
					}

					/*if (textFileToModify != "");
					{
						string selectedLine;
						string commandDelimiter = "\n";
						int position = 0;

						for (int i = 0; ((position = textFileToModify.find(commandDelimiter)) != string::npos && (i <= textFileToModify.size())); i--)
						{
							if (lineToCheck[0] == 'o') //Checks if chosen line matches
							{
								elementFound = true;
								break;
							}
							textFileToModify.erase(0, position + commandDelimiter.length());
						}
					}*/

					/*for (int i = originalIndex; i > -1; i--)
					{
						getline(streams[])
					}*/

					/*while (getline(streams[fileStreamIndex], lineToCheck)) //search for start of object backwards first
					{
						if (lineToCheck[0] == 'o') //checks for object prefix 'o'
						{
							elementFound = true;
							break;
						}
						streams[fileStreamIndex].seekg(streamIndex - 1);
						streamIndex = streams[fileStreamIndex].tellg();
					}*/

					if (!elementFound) //didn't find existingly selected object, so searches forwards
					{
						streams[fileStreamIndex].seekg(originalIndex);
						while (getline(streams[fileStreamIndex], lineToCheck)) //Gets each line & returns result into 'lineToCheck'
						{
							if (lineToCheck[0] == 'o') //checks for object prefix 'o'
							{
								selectedElement = lineToCheck; //returns object name
								selectedElement.erase(0, 2); //trimming prefix
								elementFound = true;
								break;
							}
						}
						if (!elementFound) //nothing found, so returns nothing
						{
							streams[fileStreamIndex].clear(); //Clears stream in the case that nothing was found & the 'cursor' resultingly 'fell off' the end of the stream; if not cleared, cannot be reset to start
							selectedElement = "";
							return selectedElement;
						}
					}

					//streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream

					if (attrib == "vertex")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'v')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 2); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
					else if (attrib == "face")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'f')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 2); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
				}
				else if (elemType == "vertex")
				{
					if (attrib == "vertex-texture")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'v' && lineToCheck[1] == 't')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 3); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
					else if (attrib == "vertex-normal")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'v' && lineToCheck[1] == 'n')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 3); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
				}
				streams[fileStreamIndex].clear();
				return "";
			}
		}
	}

	cout << "Failed to acquire the specified element attribute.\n";
	return "";
}

std::string FileReader::getNextElementAttribute(std::string elemType, std::string attrib)
{
	//Your code here
	
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	if (isFileAlreadyOpen)
	{
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);

				string selectedElement = "";
				string commandDelimiter = "\n";
				int position = 0;

				//Continues reading at the last position accessed from the stream
				string lineToCheck;

				if (elemType == "object")
				{
					if (attrib == "vertex")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'v')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 2); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
					else if (attrib == "face")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'f')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 2); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
				}
				else if (elemType == "vertex")
				{
					if (attrib == "vertex-texture")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'v' && lineToCheck[1] == 't')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 3); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
					else if (attrib == "vertex-normal")
					{
						while (getline(streams[fileStreamIndex], lineToCheck))
						{
							if (lineToCheck[0] == 'v' && lineToCheck[1] == 'n')
							{
								selectedElement = lineToCheck;
								selectedElement.erase(0, 3); //trimming prefix

								cout << "Selected attribute: " + selectedElement + "\n";
								return selectedElement;
							}
						}
					}
				}
				streams[fileStreamIndex].clear();
				return "";
			}
		}
	}

	cout << "Failed to acquire the specified element attribute.\n";
	return "";
}

std::string FileReader::getElementAttribute(std::string elemType, int id, std::string attrib)
{
	/*if (id == 0)
	{
		string result = getElementAttribute(elemType, attrib);
		return result;
	}*/

	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	if (isFileAlreadyOpen)
	{
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);

				string selectedElement = "";
				string commandDelimiter = "\n";
				int position = 0;

				string lineToCheck;
				int originalIndex = streams[fileStreamIndex].tellg();
				bool elementFound = false;

				int streamIndex = originalIndex;
				vector<int> potentialElementIndexes;
				if (elemType == "object")
				{
					streams[fileStreamIndex].seekg(0);

					int currentIndex = 0;
					while (streams[fileStreamIndex].tellg() < originalIndex)
					{
						getline(streams[fileStreamIndex], lineToCheck);

						if (lineToCheck[0] == 'o')
						{
							elementFound = true;
							int currentIndex = streams[fileStreamIndex].tellg();
							potentialElementIndexes.push_back(currentIndex);
						}
					}

					if (elementFound == true)
					{
						streams[fileStreamIndex].seekg(potentialElementIndexes.back());
					}

					/*if (textFileToModify != "");
					{
						string selectedLine;
						string commandDelimiter = "\n";
						int position = 0;

						for (int i = 0; ((position = textFileToModify.find(commandDelimiter)) != string::npos && (i <= textFileToModify.size())); i--)
						{
							if (lineToCheck[0] == 'o') //Checks if chosen line matches
							{
								elementFound = true;
								break;
							}
							textFileToModify.erase(0, position + commandDelimiter.length());
						}
					}*/

					/*for (int i = originalIndex; i > -1; i--)
					{
						getline(streams[])
					}*/

					/*while (getline(streams[fileStreamIndex], lineToCheck)) //search for start of object backwards first
					{
						if (lineToCheck[0] == 'o') //checks for object prefix 'o'
						{
							elementFound = true;
							break;
						}
						streams[fileStreamIndex].seekg(streamIndex - 1);
						streamIndex = streams[fileStreamIndex].tellg();
					}*/

					if (!elementFound) //didn't find existingly selected object, so searches forwards
					{
						streams[fileStreamIndex].seekg(originalIndex);
						while (getline(streams[fileStreamIndex], lineToCheck)) //Gets each line & returns result into 'lineToCheck'
						{
							if (lineToCheck[0] == 'o') //checks for object prefix 'o'
							{
								selectedElement = lineToCheck; //returns object name
								selectedElement.erase(0, 2); //trimming prefix
								elementFound = true;
								break;
							}
						}
						if (!elementFound) //nothing found, so returns nothing
						{
							streams[fileStreamIndex].clear(); //Clears stream in the case that nothing was found & the 'cursor' resultingly 'fell off' the end of the stream; if not cleared, cannot be reset to start
							selectedElement = "";
							return selectedElement;
						}
					}

					//streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream

					if (attrib == "vertex")
					{
						int index = 0;
						while (getline(streams[fileStreamIndex], lineToCheck) && index <= id)
						{
							if (lineToCheck[0] == 'v')
							{
								if (index == id)
								{
									selectedElement = lineToCheck;
									selectedElement.erase(0, 2); //trimming prefix

									cout << "Selected attribute: " + selectedElement + "\n";
									return selectedElement;
								}
								else
								{
									index++;
								}
							}
						}
					}
					else if (attrib == "face")
					{
						int index = 0;
						while (getline(streams[fileStreamIndex], lineToCheck) && index <= id)
						{
							if (lineToCheck[0] == 'f')
							{
								if (index == id)
								{
									selectedElement = lineToCheck;
									selectedElement.erase(0, 2); //trimming prefix

									cout << "Selected attribute: " + selectedElement + "\n";
									return selectedElement;
								}
								else
								{
									index++;
								}
							}
						}
					}
					else if (attrib == "vertex-texture")
					{
						int index = 0;
						while (getline(streams[fileStreamIndex], lineToCheck) && index <= id)
						{
							if (lineToCheck[0] == 'f' && lineToCheck[1] == 't')
							{
								if (index == id)
								{
									selectedElement = lineToCheck;
									selectedElement.erase(0, 3); //trimming prefix

									cout << "Selected attribute: " + selectedElement + "\n";
									return selectedElement;
								}
								else
								{
									index++;
								}
							}
						}
					}
					else if (attrib == "vertex-normal")
					{
						int index = 0;
						while (getline(streams[fileStreamIndex], lineToCheck) && index <= id)
						{
							if (lineToCheck[0] == 'f' && lineToCheck[1] == 'n')
							{
								if (index == id)
								{
									selectedElement = lineToCheck;
									selectedElement.erase(0, 3); //trimming prefix

									cout << "Selected attribute: " + selectedElement + "\n";
									return selectedElement;
								}
								else
								{
									index++;
								}
							}
						}
					}
				}
				else if (elemType == "vertex")
				{
					if (attrib == "vertex-texture")
					{
						int index = 0;
						while (getline(streams[fileStreamIndex], lineToCheck) && index <= id)
						{
							if (lineToCheck[0] == 'f' && lineToCheck[1] == 't')
							{
								if (index == id)
								{
									selectedElement = lineToCheck;
									selectedElement.erase(0, 3); //trimming prefix

									cout << "Selected attribute: " + selectedElement + "\n";
									return selectedElement;
								}
								else
								{
									index++;
								}
							}
						}
					}
					else if (attrib == "vertex-normal")
					{
						int index = 0;
						while (getline(streams[fileStreamIndex], lineToCheck) && index <= id)
						{
							if (lineToCheck[0] == 'f' && lineToCheck[1] == 'n')
							{
								if (index == id)
								{
									selectedElement = lineToCheck;
									selectedElement.erase(0, 3); //trimming prefix

									cout << "Selected attribute: " + selectedElement + "\n";
									return selectedElement;
								}
								else
								{
									index++;
								}
							}
						}
					}
				}
				streams[fileStreamIndex].clear();
				return "";
			}
		}
	}

	cout << "Failed to acquire the specified element attribute.\n";
	return "";
}

std::string FileReader::getNextElementAttribute(std::string elemType, int id, std::string attrib)
{
	return std::string();
}

void FileReader::resetElementAttribute(std::string elemType, std::string attrib)
{
	bool isFileAlreadyOpen = false;
	for (pair<const string, const string> filePath : storage)
	{
		if (currentFileName == filePath.first) //comparing against key, not value
		{
			isFileAlreadyOpen = true;
			break;
		}
	}

	if (isFileAlreadyOpen)
	{
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				int fileStreamIndex = stoi(filePath.second);
				streams[fileStreamIndex].seekg(0); //Ensures that reading initiates from the start of the stream
				cout << "Cursor reset successfully";
				break;
			}
		}
	}
	else
	{
		cout << "File was not open, so no cursor could be reset.\n";
	}
}

void FileReader::printDataToCMD(std::string* lines)
{
	cout << *lines;
}

bool FileReader::switchCurrentFile(std::string fileName) {

	for (pair<const string, const string> filePath : storage)
	{
		if (fileName == filePath.first)
		{
			currentFileName = fileName;
			cout << "Switched file successfully.\n";
			return true;
		}
	}
	cout << "Failed to switch file; may have not been open.\n";
	return false;
}

bool FileReader::overrideCurrentFile(std::string fileName, std::string* lines) {
	return false;
}
bool FileReader::closeFile(std::string fileName) {
	for (pair<const string, const string> filePath : storage)
	{
		if (fileName == filePath.first)
		{
			currentFileName = "";

			int fileStreamIndex = stoi(filePath.second);

			streams[fileStreamIndex].close();
			streams.erase(next(streams.begin(), fileStreamIndex)); //Removes from vector of open streams
			storage.erase(filePath.first); //Removes from map of file paths of vector indexes of open streams
			cout << "File closed successfully.\n";
			return true;
		}
	}
	cout << "Couldn't close specific file at '" + fileName + ".'\n";
	return false;
}
bool FileReader::closeCurrentFile()
{
	if (currentFileName != "")
	{
		for (pair<const string, const string> filePath : storage)
		{
			if (currentFileName == filePath.first)
			{
				currentFileName = "";

				int fileStreamIndex = stoi(filePath.second);

				streams[fileStreamIndex].close();
				streams.erase(next(streams.begin(), fileStreamIndex)); //Removes from vector of open streams
				storage.erase(filePath.first); //Removes from map of file paths of vector indexes of open streams
				cout << "File closed successfully.";
				return true;
			}
		}
	}
	return false;
}