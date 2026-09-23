#pragma once

#include <atlstr.h>
#include <iostream>
#include <stdint.h>
#include "Structures.h"

class CDataOperations
{
public:
	int fill_method;
	bool fill_type;
	double fill_with;

	bool csv_with_filename = false;
	string csv_filename_header = "";
	vector<string> csv_image_filenames;

	vector<string> last_wave_fnames;


public:
	CDataOperations(void);
	~CDataOperations(void);

	bool areFilesCsv(vector<string> fnames);
	bool areFilesJpg(vector<string> fnames);
	bool areFilesRaw(vector<string> fnames);
	bool areFilesWave(vector<string> fnames);

	bool getDataFromCsv(string fname, vector<dataset2>* fileData);
	bool getProcessedDataFromFileData(string fname, vector<string> fields, vector<dataset2>* processedData, vector<dataset2>* fileData);
	int getColumnIndexFromHeader(int findex, string hname, vector<dataset2>* fileData);
	data_column2 getColumnFromField(int findex, int hindex, vector<dataset2>* fileData);
	data_column2 getColumnFromFieldWithOffset(int findex, int hindex, int offset, vector<dataset2>* fileData);
	bool columnIsNumbersData(vector<double>* col);

	string checkFilenamesInCsv(vector<dataset2>* fileData);

	bool addMissingDataColumns(string fname, vector<string> fields, vector<outputData> outdata, vector<dataset2>* processedData);
	data_column2 getColumnFromField(string header, int hindex, CSVFile *mfile);
	bool isFieldInDataset(string header, dataset2* dset);
	void applyOffsetToData(vector<dataset2>* processedData);

	bool addSoundConvDataRows(string fname, int nsteps, vector<dataset2>* processedData);

	bool processDataColumn(string *fieldname, processData mprocess, double leakage, vector<dataset2>* processedData);

	bool processFFTColumn(string fieldname, processData mprocess, vector<dataset2>* processedData);

	bool getProcessData(string* fieldname, processData* mprocess, vector<dataset2>* processedData);
	
	vector<double> getFFT(int index, int N, int downsample, bool normalize, vector<double> data);
	vector<double> getFFT2(int index, int N, int downsample, vector<double> data);
	void fft(complex* v, int n);
	void fft(complex* v, int n, complex* tmp);

	double firstDerivativeBackward(int index, int stride, bool relative, vector<double> *data);
	double secondDerivativeBackward(int index, int stride, bool relative, vector<double>* data);
	double thirdDerivativeBackward(int index, int stride, bool relative, vector<double>* data);
	double fourthDerivativeBackward(int index, int stride, bool relative, vector<double>* data);

	bool isLabelInFilename(string label, string dataPath);
	vector<string> parseKeyNamesFromFilename(string dataPath);
	string removeExtensionFromFilename(string path);


	int getColumnIndexFromHeader(string fieldname, int fileindex, vector<dataset2>* processedData);


	bool getImages(vector<string> fnames, int image_width, int image_height, _int64 image_train_data_size, double* image_train_data);
	bool getSoundData(vector<string> fnames, wave_check_results* wresults, _int64 wave_allocation_size, double* wave_train_data);
	int getLongestWaveFile(vector<string> fnames);
	bool getMaxWaveMagnitude(vector<string> fnames, wave_check_results *wresults);
	bool fillSoundFFTProcessData(int fft_size, int soundfftstride, int soundfftdownsample, wave_check_results* wresults, vector<dataset2>* processedData);
	bool processSoundFFTData(int fft_size, int soundfftstride, int soundfftdownsample, bool normaiize_file_set, wave_check_results *wresults, vector<dataset2> *processedData);


	void addErrorString(string err);

};

