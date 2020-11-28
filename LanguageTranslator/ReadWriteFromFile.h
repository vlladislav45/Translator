#pragma once
#include <iostream>
#include <string>
#include <conio.h>
#include <fstream>
#include <stdio.h>
#include <map>

using namespace std;

class CFileDescription {
private:
	string enWord;
	string bgWord;
public:
	//Default Constructor
	CFileDescription() {
		enWord = "";
		bgWord = "";
	}

	//Explicit Constructor
	CFileDescription(const string enWord, const string bgWord) {
		this->enWord = enWord;
		this->bgWord = bgWord;
	}

	//Copy Constructor
	CFileDescription(const CFileDescription& param) {
		enWord = param.enWord;
		bgWord = param.bgWord;
	}

	//Accessors and mutators
	string getEnWord() {
		return enWord;
	}

	string getbgWord() {
		return bgWord;
	}

	friend istream& operator>>(istream& toStream, CFileDescription& obj);
};

istream& operator>>(istream& toStream, CFileDescription& obj) {
	toStream >> obj.enWord >> obj.bgWord;
	return toStream;
}

class ReadWriteFromFile {
private:
	map<string,string> translations;
public:
	ReadWriteFromFile() {}

	ReadWriteFromFile(string filename) {
		ifstream cf;

		cf.open(filename);
		if (cf.is_open()) {
			CFileDescription fileDes;
			while (cf >> fileDes)
			{
				translations.insert(pair<string, string>(fileDes.getEnWord(), fileDes.getbgWord()));
			}
		}
		cf.close();
	}

	//TODO: Change to FileDes object
	void addWord(string enWord, string bgWord, string filename) {
		ofstream cf;

		cf.open(filename, ios::app); // open file for appending
		if (cf.is_open()) {
			cf << enWord << " " << bgWord << " \n";
		}
		cf.close();
	}

	//TODO: Change to FileDes object
	void deleteWord(string enWord, string filename) {
		if (remove(filename.c_str()) != 0)
			perror("Error deleting file");
		else
			puts("File successfully deleted");

		map<string, string>::iterator it; // Declare iterator of our map
		it = translations.find(enWord); //find in our map
		translations.erase(it); // delete from our map with translations
		
		ofstream cf(filename);

		for (auto const& translation : translations) {
			cf << translation.first << " " << translation.second << " \n";
		}
	}

	//Get translations
	map<string, string> getTranslations() { return this->translations; }
};