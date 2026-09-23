#pragma once


//#define GCC


#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "CFileOperations.h"

using namespace std;


typedef struct jsonLineData {
	bool nodata = false;
	bool opencollection = false;
	bool closecollection = false;
	bool openarray = false;
	bool closearray = false;
	bool keyvaluestring = false;
	bool keyvalueint = false;
	bool keyvaluedouble = false;
	bool arrayvaluestring = false;
	bool arrayvalueint = false;
	bool arrayvaluedouble = false;
	string key = "";
	string valuestring = "";
	int valueint = 0;
	double valuedouble = 0.0;
} jsonLineData;

typedef struct valueStringData {
	string value = "";
	int endindex = 0;
} keyStringData;

typedef struct valueIntData {
	int value = 0;
	int endindex = 0;
} valueIntData;

typedef struct valueDoubleData {
	double value = 0.0;
	int endindex = 0;
} valueDoubleData;



class CJSON : public CLineParser
{
public:

	int indentcnt = 0;
	int commentcnt = 0;
	int parseindex = 0;
	string mystring = "";
	jsonLineData pdata;

public:

	void clear() {
		indentcnt = 0;
		commentcnt = 0;
		parseindex = 0;
		mystring = "";
	}

	void openCollection(string key)
	{
		if (key == "") {
			mystring += indent("{\n");
		}
		else {
			mystring += indent("\"" + key + "\": {\n");
		}
		indentcnt++;
	}

	void closeCollection(bool comma, bool linefeed)
	{
		indentcnt--;
		if (indentcnt < 0) {
			addErrorString("closeCollection : indentcnt going negative - setting to zero");
			indentcnt = 0;
		}
		mystring += indent("}");
		if (comma) {
			mystring += ",";
		}
		if (linefeed) {
			mystring += "\n";
		}
	}

	void openArray(string key)
	{
		mystring += indent(("\"" + key + "\": [\n"));
		indentcnt++;
	}

	void closeArray(bool comma, bool linefeed)
	{
		indentcnt--;
		if (indentcnt < 0) {
			addErrorString("closeArray : indentcnt going negative - setting to zero");
			indentcnt = 0;
		}
		mystring += indent("]");
		if (comma) {
			mystring += ",";
		}
		if (linefeed) {
			mystring += "\n";
		}
	}

	void addComment(string comment, bool comma) {
		addStringKey(("Comment" + codeInt(commentcnt)), comment, comma);
		commentcnt++;
	}

	void addStringKey(string key, string value, bool comma) {
		mystring += indent(codeStringKey(key, value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addIntKey(string key, int value, bool comma) {
		mystring += indent(codeIntKey(key, value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addDoubleKey(string key, double value, bool comma) {
		mystring += indent(codeDoubleKey(key, value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addDoubleKey2(string key, double value, bool comma) {
		mystring += indent(codeDoubleKey2(key, value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addString(string value, bool comma) {
		mystring += indent(codeString(value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addInt(int value, bool comma) {
		mystring += indent(codeInt(value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addDouble(double value, bool comma) {
		mystring += indent(codeDouble(value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addDouble2(double value, bool comma) {
		mystring += indent(codeDouble2(value));
		if (comma) {
			mystring += ",";
		}
		mystring += "\n";
	}

	void addComma()
	{
		mystring += ",";
	}

	void addLineFeed()
	{
		mystring += "\n";
	}

	string codeStringKey(string key, string value)
	{
		string rval = "\"" + key + "\": \"" + value + "\"";
		return(rval);
	}

	string codeIntKey(string key, int value)
	{
		char s[100];  sprintf_s(s, "%i", value);
		string rval = "\"" + key + "\": " + s;
		return(rval);
	}

	string codeDoubleKey(string key, double value)
	{
		char s[100];  sprintf_s(s, "%f", value);
		string rval = "\"" + key + "\": " + s;
		return(rval);
	}

	string codeDoubleKey2(string key, double value)
	{
		char s[100];  sprintf_s(s, "%.16e", value);
		string rval = "\"" + key + "\": " + s;
		return(rval);
	}

	string codeString(string value)
	{
		string rval = "\"" + value + "\"";
		return(rval);
	}

	string codeInt(int value)
	{
		char s[100];  sprintf_s(s, "%i", value);
		string rval =  s;
		return(rval);
	}

	string codeDouble(double value)
	{
		char s[100];  sprintf_s(s, "%f", value);
		string rval = s;
		return(rval);
	}

	string codeDouble2(double value)
	{
		char s[100];  sprintf_s(s, "%.16e", value);
		string rval = s;
		return(rval);
	}

	string indent(string line)
	{
		string rval = "";
		for (int i = 0; i < indentcnt; i++) {
			rval += "    ";
		}
		rval = rval + line;
		return(rval);
	}

	int findNextEndOfLine(int start) 
	{
		int index = start;
		while ((index < (int)mystring.length()) && (!isEOL(mystring[index]))) {
			index++;
		}
		return(index);
	}

	int findNextOfLStartine(int start)
	{
		int index = start;
		while ((index < (int)mystring.length()) && (isEOL(mystring[index]))) {
			index++;
		}
		return(index);
	}

	string getLine(int start, int end)
	{
		return(mystring.substr(start, ((int64_t)end - start)));
	}

	string removeLeadingWhitespace(string line)
	{
		int start = 0;
		while ((start < (int)line.length()) && (isWhiteSpace(line[start]))) {
			start++;
		}
		return(line.substr(start, (line.length() - start)));
	}

	bool parseNextLine()
	{
		while (parseindex < mystring.length()) {
			int end = findNextEndOfLine(parseindex);
			string line = removeLeadingWhitespace(getLine(parseindex, end));
			//addErrorString("line : " + line);
			pdata = parseLine(line);
			parseindex = findNextOfLStartine(end);
			if (!pdata.nodata) {
				return(true);
			}
			//addErrorString("parseNextLine bypassing " + pdata.key);
			//char s[200];  sprintf_s(s, "parseindex = %i", parseindex);  addErrorString(s);
		}
		return(false);
	}

	jsonLineData getCurrentParseData()
	{
		return(pdata);
	}

	jsonLineData parseLine(string line)
	{
		jsonLineData rval;
		rval.nodata = true;
		int index = 0;
		bool keyalready = false;
		string mvaluestring = "";
		while (index < (int)line.length()) {
			if (line[index] == '"') {
				valueStringData rdata = parseStringValue(index, line);
				if (keyalready) {
					//addErrorString("setting value : " + rdata.value);
					rval.nodata = false;
					rval.keyvaluestring = true;
					rval.valuestring = rdata.value;
					return(rval);
				}
				else {
					//addErrorString("setting key : " + rdata.value);
					rval.key = rdata.value;
					mvaluestring = rdata.value;
					index = rdata.endindex;
					keyalready = true;
				}
			}
			else if (line[index] == ':') {
				//addErrorString("need parsing for " + line + " char " + line[index]);
			}
			else if (line[index] == '{') {
				rval.nodata = false;
				rval.opencollection = true;
				return(rval);
			}
			else if (line[index] == '}') {
				rval.nodata = false;
				rval.closecollection = true;
				return(rval);
			}
			else if (line[index] == '[') {
				rval.nodata = false;
				rval.openarray = true;
				return(rval);
			}
			else if (line[index] == ']') {
				rval.nodata = false;
				rval.closearray = true;
				return(rval);
			}
			else if (isNumber(line[index])) {
				int nindex = index + 1;
				while ((nindex < (int)line.length()) && (isNumber(line[nindex]) || (line[nindex] == 'e'))) {
					nindex++;
				}
				string snumber = "number size error";
				int nsize = nindex - index;
				if (nsize > 0) {
					snumber = line.substr(index, nsize);
				}
				if (snumber.length() > 0) {
					if (isInt(snumber)) {
						rval.valueint = myStoi(snumber);
						rval.nodata = false;
						if (keyalready) {
							rval.keyvalueint = true;
						}
						else {
							rval.arrayvalueint = true;
						}
						return(rval);
					}
					else {
						//stringstream ss(snumber);
						//ss >> rval.valuedouble;
						rval.valuedouble = stold(snumber);
						rval.nodata = false;
						if (keyalready) {
							rval.keyvaluedouble = true;
						}
						else {
							rval.arrayvaluedouble = true;
						}
						return(rval);
					}
				}
				else {
					addErrorString("parseLine : unable to parse number - " + line);
				}
				return(rval);
			}
			else if (!isWhiteSpace(line[index])) {
				if (line[index] != ',') {
					addErrorString("parseLine : unable to parse char in line - " + line + " - char " + line[index]);
				}
			}
			index++;
		}
		if (keyalready) {
			rval.nodata = false;
			rval.arrayvaluestring = true;
			rval.valuestring = mvaluestring;
			return(rval);
		}
		return(rval);
	}

	valueStringData parseStringValue(int start, string line)
	{
		valueStringData rval;
		rval.value = "";
		rval.endindex = start + 1;
		int index = start;
		if (line[index] == '"') {
			index++;
			while ((index < (int)line.length()) && (line[index] != '"')) {
				index++;
			}
			if ((index - start) > 0) {
				rval.value = line.substr(((int64_t)start + 1), (((int64_t)index - 1) - start));
				rval.endindex = index;
			}
		}
		return(rval);
	}

	bool parseNextOpenCollection()
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).opencollection) {
			if (mdata.key != "") {
				addErrorString("parseNextOpenCollection1 : problem finding key in initial open collection : " + mdata.key);  return(false);
			}
		}
		else {
			addErrorString("parseNextOpenCollection1 : problem finding open collection : " + mdata.key);  return(false);
		}
		return(true);
	}

	bool parseNextOpenCollection(string value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).opencollection) {
			if (value != mdata.key) {
				addErrorString("parseNextOpenCollection2 : problem with key : " + value);  return(false);
			}
		}
		else {
			addErrorString("parseNextOpenCollection2 : problem finding open collection : " + mdata.key);  return(false);
		}
		return(true);
	}

	bool parseNextOpenCollection(string* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).opencollection) {
			*value = mdata.key;
		}
		else {
			addErrorString("parseNextOpenCollection3 : problem finding open collection : " + mdata.key);  return(false);
		}
		return(true);
	}

	bool parseNextCloseCollection()
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).closecollection) {
			if (mdata.key != "") {
				addErrorString("parseNextCloseCollection4 : problem finding key in close collection : " + mdata.key);  return(false);
			}
		}
		else {
			addErrorString("parseNextCloseCollection4 : problem finding close collection : " + mdata.key);  return(false);
		}
		return(true);
	}

	bool parseNextOpenArray(string* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).openarray) {
			*value = mdata.key;
		}
		else {
			addErrorString("parseNextOpenArray : problem finding open array");  return(false);
		}
		return(true);
	}

	bool parseNextCloseArray()
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).closearray) {
			if (mdata.key != "") {
				addErrorString("parseNextCloseArray : problem finding key in close array");  return(false);
			}
		}
		else {
			addErrorString("parseNextCloseArray : problem finding close array");  return(false);
		}
		return(true);
	}

	bool parseNextKeyValueInt(string key, int *value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).keyvalueint) {
			if (mdata.key != key) {
				addErrorString("parseNextKeyValueInt1 : problem finding " + key);  return(false);
			}
			*value = mdata.valueint;  //char s[200];  sprintf_s(s, "parseNextKeyValueInt : value = %i", *value);  addErrorString(s);
		}
		else {
			addErrorString("parseNextKeyValueInt1 : problem finding keyvalueint");  return(false);
		}
		return(true);
	}

	bool parseNextKeyValueInt(string* key, int* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).keyvalueint) {
			*key = mdata.key;
			*value = mdata.valueint;
		}
		else {
			addErrorString("parseNextKeyValueInt2 : problem finding keyvalueint");  return(false);
		}
		return(true);
	}

	bool parseNextKeyValueDouble(string key, double* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).keyvaluedouble) {
			if (mdata.key != key) {
				addErrorString("parseNextKeyValueDouble1 : problem finding " + key);  return(false);
			}
			*value = mdata.valuedouble;
		}
		else {
			addErrorString("parseNextKeyValueDouble1 : problem finding keyvaluedouble");  return(false);
		}
		return(true);
	}

	bool parseNextKeyValueDouble(string* key, double* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).keyvaluedouble) {
			*key = mdata.key;
			*value = mdata.valuedouble;
		}
		else {
			addErrorString("parseNextKeyValueDouble2 : problem finding keyvaluedouble");  return(false);
		}
		return(true);
	}

	bool parseNextKeyValueString(string key, string* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).keyvaluestring) {
			if (mdata.key != key) {
				addErrorString("parseNextKeyValueString1 : problem finding " + key);  return(false);
			}
			*value = mdata.valuestring;
		}
		else {
			addErrorString("parseNextKeyValueString1 : problem finding keyvaluestring : " + key + " : " + *value);  return(false);
		}
		return(true);
	}

	bool parseNextKeyValueString(string* key, string* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).keyvaluestring) {
			*key = mdata.key;
			*value = mdata.valuestring;
		}
		else {
			addErrorString("parseNextKeyValueString2 : problem finding keyvaluestring : " + *key + " : " + *value);  return(false);
		}
		return(true);
	}


	bool parseNextValueInt(int* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).valueint) {
			*value = mdata.valueint;
		}
		else {
			addErrorString("parseNextValueInt : problem finding valueint");  return(false);
		}
		return(true);
	}

	bool parseNextValueString(string* value)
	{
		jsonLineData mdata;
		parseNextLine();
		if ((mdata = getCurrentParseData()).arrayvaluestring) {
			*value = mdata.valuestring;
		}
		else {
			addErrorString("parseNextValueString : problem finding arrayvaluestring");  return(false);
		}
		return(true);
	}

	int myStoi(string s)
	{
		if (!isWordNumber(s)) {
			addErrorString(s);
		}
		return(stoi(s));
	}





	void saveFile(string filename)
	{
		// Create an output file stream
		ofstream outfile(filename);

		// Check if the file opened successfully
		if (!outfile.is_open()) {
			addErrorString("Error opening file: " + filename);
			return;
		}

		// Write the string to the file
		outfile << mystring;

		// Close the file
		outfile.close();

		addErrorString("File " + filename + " saved successfully!");

	}

	bool loadFile(string filename)
	{
		clear();

		ifstream infile(filename);
		
		std::stringstream buffer;

		if (infile.is_open()) {

			buffer << infile.rdbuf();
			mystring = buffer.str();

			infile.close();

			addErrorString("File " + filename + " loaded successfully!");

			return(true);

		}
		else {
			addErrorString("Error opening file " + filename);
		}

		return(false);

	}

	void addErrorString(string err);


};

