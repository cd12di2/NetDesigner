#pragma once

#include "Structures.h"


class CLineParser
{
public:
	CLineParser(void) {};
	~CLineParser(void) {};

	bool isNumber(char data);
	bool isOpenParenthesis(char data);
	bool isCloseParenthesis(char data);
	bool isForwardSlash(char data);
	bool isBackSlash(char data);
	bool isColon(char data);
	bool isComma(char data);
	bool isUnderscore(char data);
	bool isSpace(char data);
	bool isWhiteSpace(char data);
	bool isSeparator(char data);
	bool isSeparator2(char data);
	bool isSeparator3(char data);
	bool isSeparator4(char data);
	bool isValidCharacter(char data);
	bool isEOL(char mdata);

	int findSeparator(int index, string line);
	int findOpenParenthesis(int index, string line);
	int findCloseParenthesis(int index, string line);
	int findForwardSlash(int index, string line);
	int findLastSpace(string line);
	int findUnderscore(int index, string line);

	bool isInt(string data);

	int getNextWord(int start, string data, string* word);
	bool isWordNumber(string word);

	int myStoi(string s);

	bool hasSingleColon(string cname);
	string getStringBeforeColon(string cname);
	string getStringAfterColon(string cname);
	string getStringAfterLastColon(string cname);

	int getNumberFromString(string cname);

	string makeLower(string mstring);

	vector<string> parseLine(string line);
	vector<string> parseLine2(string line);

	unsigned char* downscaleImageData(int inrows, int incols, int instride, unsigned char* rgbin, int outrows, int outcols, unsigned char* rgbout);
	rgb_value downscaleImageRowSlice(float rstart, float rend, float cstart, float cend, int rstride, unsigned char* rgbin);
	rgb_value downscaleImageColumnSlice(float start, float end, int rowoffset, unsigned char* rgbin);

	string removePathFromFilename(string path);

	string getFileExtension(string fname);

	void addErrorString(string err);
	void addPanel1String(string msg);


};


class File : public CLineParser
{
public:
	string filename;
	bool display_error;
	vector<string> flines;

public:
	File(void);
	~File(void);

	void init();

	bool loadFile(string fname, bool derror = true);
	void addErrorString(string err);

};


class CSVFile : public File
{
public:
	vector<string> headers;
	vector<bool> columnIsNumbersData;
	vector<vector<string>> sdata;
	vector<vector<double>> ddata;

public:
	CSVFile(void);
	~CSVFile(void);

	void init();

	bool isCSVFile(string fname);

	bool loadCSVFile(string fname, bool derror);

	bool parseCSVFile(bool derror);

	int getColumnIndexFromHeader(string hname);

	bool isColumnNumbersData(int cindex);
	string getStringData(int cindex, int rindex);
	vector<string> getColumnStringData(int cindex);
	vector<double> getColumnDoubleData(int cindex);




};


class JPGFile : public File
{
public:

public:
	JPGFile(void);
	~JPGFile(void);

	void init();

	bool isJPGFile(string fname);

};


class RawFile : public File
{
public:

public:
	RawFile(void);
	~RawFile(void);

	void init();

	bool isRawFile(string fname);

};


class WaveFile : public File
{
public:
	int minimum_sound_level = 20;
	bool normaiize_file_set = false;

public:
	WaveFile(void);
	~WaveFile(void);

	void init();

	bool isWaveFile(string fname);

	wave_data getWaveDataFromFile(string fname);




};



