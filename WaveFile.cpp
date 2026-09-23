               

#include "pch.h"
#include "framework.h"
#include "NetDesignerDoc.h"
#include "WaveFile.h"
#include <math.h>

#include <mmsystem.h>
#include <afxdlgs.h>


CString initials = _T("sp");
CString directory = _T("C:\\Copydump\\");

long acquisition_rate = 44100;


CWaveFile::CWaveFile()
{
	Initialize();
}

CWaveFile::~CWaveFile()
{
	if (wave_data_allocated) {
		delete [] wave_data;
		wave_data_allocated = FALSE;
	}
}

void CWaveFile::Initialize()
{
	filename = "uninitialized";
	filelength = 0;
	
	num_channels = 2;
	bytes_per_sample = 4;
	bits_per_sample = 16;

	file_length = 0;
	sample_rate = acquisition_rate;
	bytes_per_second = bytes_per_sample * acquisition_rate;
	data_length = 0;
	num_wave_data = 0;

	wave_data_allocated = FALSE;
}

void CWaveFile::addErrorString(CString err) {
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString(err);
}

BOOL CWaveFile::ReadWaveFile(string name)
{
	int i;
	unsigned char data[50];
	CString sbuf,riff,wave;

	CFile file;

	filename = (CString)name.c_str();

	if (file.Open(filename, CFile::modeRead | CFile::shareDenyWrite)) {

		filelength = (unsigned long) file.GetLength();

		if (wave_data_allocated) {
			delete [] wave_data;
			wave_data_allocated = FALSE;
		}
		if (filelength > 0) {
			wave_data = new long[(long)filelength + 100];
			if (wave_data != 0) {
				wave_data_allocated = TRUE;
				for (unsigned long i = 0; i < filelength; i++) {
					wave_data[i] = 0;
				}
			}
		}

		if (wave_data_allocated) {

			file.Read(data,12);

			riff = "";
			wave = "";
			for (i=0; i<4; i++) {
				riff += data[i];
				wave += data[i+8];
			}

			if ((riff == "RIFF") && (wave == "WAVE")) {

				file_length = (long) data[4]; 
				file_length += (long) data[5] * 256; 
				file_length += (long) data[6] * 65536; 
				file_length += (long) data[7] * 16777216;

				GetNextChunk(&file);
				while ((chunk_id != "fmt ") && (chunk_id != "end")) {
					SkipChunk(&file);
					GetNextChunk(&file);
				}

				GetFormatChunk(&file);

				GetNextChunk(&file);
				while ((chunk_id != "data") && (chunk_id != "end")) {
					SkipChunk(&file);
					GetNextChunk(&file);
				}

				data_length = chunk_length;

				GetDataChunk(&file);

			} else {
				sbuf = filename;
				sbuf += " not a .wav file";
				file.Close();
				addErrorString(sbuf);
				return(FALSE);
			}

		} else {
			addErrorString(_T("Unable to allocate memory for wave data"));
		}

		file.Close();

	} else {
		if (filename != _T("")) {
			sbuf = "Unable to open filename : ";
			sbuf += filename;
			addErrorString(sbuf);
		}
		return(FALSE);
	}

	return(TRUE);

}

void CWaveFile::DisplayChunkInfo()
{
	CString s;
	s.Format(_T(" : chunk_length = %i"), chunk_length);
	addErrorString(_T("chunk_id = ") + chunk_id + s);
}


void CWaveFile::GetNextChunk(CFile *file)
{
	int i;
	unsigned char data[50];

	chunk_id = "";
	chunk_length = 0;

	if (file->Read(data,4) == 4) {

		for (i=0; i<4; i++) {
			chunk_id += data[i];
		}

		if (file->Read(data,4) == 4) {
			chunk_length = (long) data[0];
			chunk_length += (long) data[1] * 256;
			chunk_length += (long) data[2] * 65536;
			chunk_length += (long) data[3] * 16777216;
		}

	} else {
		chunk_id = "end";
	}

}

void CWaveFile::SkipChunk(CFile *file)
{
	long num_read;
	unsigned char data[500];

	while (chunk_length > 0) {
		if (chunk_length > 400) {
			num_read = 400;
		} else {
			num_read = chunk_length;
		}
		file->Read(data,num_read);
		chunk_length -= num_read;
	}

}

void CWaveFile::GetFormatChunk(CFile *file)
{
	unsigned char data[50];

	num_channels = 0;
	bytes_per_sample = 0;
	bits_per_sample = 0;
	sample_rate = 0;
	bytes_per_second = 0;

	if (file->Read(data,2) != 2) {
		addErrorString(_T("GetFormatChunk error : chunk_length"));
		return;
	}

	if (file->Read(data,2) == 2) {
		num_channels = (long) data[0];
		num_channels += (long) data[1] * 256;
	} else {
		addErrorString(_T("GetFormatChunk error : num_channels"));
		return;
	}

	if (file->Read(data,4) == 4) {
		sample_rate = (long) data[0];
		sample_rate += (long) data[1] * 256;
		sample_rate += (long) data[2] * 65536;
		sample_rate += (long) data[3] * 16777216;
	} else {
		addErrorString(_T("GetFormatChunk error : sample_rate"));
		return;
	}

	if (file->Read(data,4) == 4) {
		bytes_per_second = (long) data[0];
		bytes_per_second += (long) data[1] * 256;
		bytes_per_second += (long) data[2] * 65536;
		bytes_per_second += (long) data[3] * 16777216;
	} else {
		addErrorString(_T("GetFormatChunk error : bytes_per_second"));
		return;
	}

	if (file->Read(data,2) == 2) {
		bytes_per_sample = (long) data[0];
		bytes_per_sample += (long) data[1] * 256;
	} else {
		addErrorString(_T("GetFormatChunk error : bytes_per_sample"));
		return;
	}
	
	if (file->Read(data,2) == 2) {
		bits_per_sample = (long) data[0];
		bits_per_sample += (long) data[1] * 256;
	} else {
		addErrorString(_T("GetFormatChunk error : bits_per_sample"));
		return;
	}

	if ((chunk_length > 16) && (chunk_length < 50)) {
		file->Read(data,chunk_length - 16);
	}

	//CString s = _T("");
	//s.Format(_T("num_channels = %i : sample_rate = %i : bytes_per_second = %i : bytes_per_sample = %i : bits_per_sample = %i"), num_channels, sample_rate, bytes_per_second, bytes_per_sample, bits_per_sample);
	//addErrorString(s);
	
}

void CWaveFile::GetDataChunk(CFile *file)
{
	int i,j,bytes_per_channel;
	long multiplier,data_index,tnum_data;
	long temp_data[8];
	unsigned char data[50];

	tnum_data = data_length / bytes_per_sample;

	if ((num_channels * tnum_data) > (long) filelength) {
		addErrorString(_T("Not enough memory allocated for wave data"));
		num_wave_data = 0;
		return;
	}

	unsigned long test_index = 0;
	unsigned char* test_data = new unsigned char[filelength];
	long test_size = file->Read(test_data, filelength); 
	//CString s = _T("");
	//s.Format(_T("GetDataChunk : test_size = %i : filelength = %i : "), test_size, filelength);
	//addErrorString(s + filename);

	data_index = 0;

	if ((bytes_per_sample <= 8) && (num_channels > 0) && (num_channels < 8)) {

		bytes_per_channel = bytes_per_sample / num_channels;

		while (chunk_length > 0) { 

			if (chunk_length >= bytes_per_sample) {

				//file->Read(data,bytes_per_sample);

				if ((test_index + bytes_per_sample) < filelength) {
					for (i = 0; i < bytes_per_sample; i++) {
						data[i] = test_data[test_index + i];
					}
					test_index += bytes_per_sample;
				}
				else {
					addErrorString(_T("GetDataChunk : test_index error for ") + filename);
				}

				chunk_length -= bytes_per_sample;
			
				for (i=0; i<8; i++) {
					temp_data[i] = 0;
				}

				for (j=0; j<num_channels; j++) {

					multiplier = 1;
	
					for (i=0; i<bytes_per_channel; i++) {

						temp_data[j] += (long) data[(j*bytes_per_channel)+i] * multiplier;

						multiplier = multiplier * 256;

					}

				}
				
				for (j=0; j<num_channels; j++) {
					wave_data[data_index + (j * tnum_data)] = temp_data[j];
				}
				if (data_index < tnum_data) {
					data_index++;
				}
			
			}

		}

		data_index = tnum_data;

		if (bytes_per_channel == 1) {
			for (j=0; j<(num_channels*data_index); j++) {
				wave_data[j] = wave_data[j] - 128;
			}
		} else if (bytes_per_channel == 2) {
			for (j=0; j<(num_channels*data_index); j++) {
				if (wave_data[j] > 32768) {
					wave_data[j] = wave_data[j] - 65536;
				}
			}
		} 

	}

	delete[] test_data;

	num_wave_data = data_index;

}

void CWaveFile::GetList(CFile *file)
{
	int count;
	char cbuf[50];
	unsigned char data[50];
	CString sbuf = _T("");

	data[0] = ' ';
	data[1] = ' ';
	data[2] = ' ';
	data[3] = ' ';

	count = 0;

	while (file->Read(data,1)) {

		if (data[0] >= 32) {
			sbuf += data[0];
			sbuf += "(";
			_itoa_s(data[0],cbuf,10);
			sbuf += sbuf;
			sbuf += ")";
		} else {
			sbuf += "(";
			_itoa_s(data[0],cbuf,10);
			sbuf += cbuf;
			sbuf += ")";
		}
		sbuf += " ";
		count++;
		if (count >= 40) {
			sbuf += "\n";
			count = 0;
		}

	}

}

void CWaveFile::SaveWaveFile(CString name)
{
	int j,bytes_per_channel;

	if ((bytes_per_sample <= 8) && (num_channels > 0) && (num_channels < 8)) {

		CFile file;

		filename = directory + name;
	
		bytes_per_channel = bytes_per_sample / num_channels;

		if (bytes_per_channel == 1) {
			for (j=0; j<(num_channels*num_wave_data); j++) {
				wave_data[j] = wave_data[j] + 128;
			}
		} else if (bytes_per_channel == 2) {
			for (j=0; j<(num_channels*num_wave_data); j++) {
				if (wave_data[j] < 0) {
					wave_data[j] = wave_data[j] + 65536;
				}
			}
		} 

		data_length = num_wave_data * bytes_per_sample;

//		AfxMessageBox(filename);

		if (file.Open(filename,CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive)) {

			SaveRiffChunk(&file);
			SaveFormatChunk(&file);
			SaveDataChunk(&file);
			AddListChunk(&file);

			file.Close();			

		} else {
			AfxMessageBox(_T("Unable to open file to save"));
		}

	} else {
		AfxMessageBox(_T("Unable to save file due to error with bytes_per_sample or with num_channels"));
	}

}

void CWaveFile::SaveRiffChunk(CFile *file)
{
	long total_length;
	char cbuf[50];

	file->Write("RIFF",4);

	total_length = 28 + data_length + 20;//48;

	cbuf[0] = (char) (255 & total_length);
	cbuf[1] = (char) (255 & (total_length / 256));
	cbuf[2] = (char) (255 & (total_length / 65536));
	cbuf[3] = (char) (255 & (total_length / 16777216));

	file->Write(cbuf,4);

	file->Write("WAVE",4);

}

void CWaveFile::SaveFormatChunk(CFile *file)
{
	char cbuf[50];

	file->Write("fmt ",4);

	cbuf[0] = 16;
	cbuf[1] = 0;
	cbuf[2] = 0;
	cbuf[3] = 0;
	file->Write(cbuf,4);

	cbuf[0] = 1;
	cbuf[1] = 0;
	file->Write(cbuf,2);

	cbuf[0] = (char) num_channels;
	cbuf[1] = 0;
	file->Write(cbuf,2);

	cbuf[0] = (char) (255 & sample_rate);
	cbuf[1] = (char) (255 & (sample_rate / 256));
	cbuf[2] = (char) (255 & (sample_rate / 65536));
	cbuf[3] = (char) (255 & (sample_rate / 16777216));
	file->Write(cbuf,4);

	cbuf[0] = (char) (255 & bytes_per_second);
	cbuf[1] = (char) (255 & (bytes_per_second / 256));
	cbuf[2] = (char) (255 & (bytes_per_second / 65536));
	cbuf[3] = (char) (255 & (bytes_per_second / 16777216));
	file->Write(cbuf,4);

	cbuf[0] = (char) (255 & bytes_per_sample);
	cbuf[1] = (char) (255 & (bytes_per_sample / 256));
	file->Write(cbuf,2);

	cbuf[0] = (char) (255 & bits_per_sample);
	cbuf[1] = (char) (255 & (bits_per_sample / 256));
	file->Write(cbuf,2);

}

void CWaveFile::SaveDataChunk(CFile *file)
{
	int i,j,k,bytes_per_channel;
	long data,divisor;
	char cbuf[50];

	bytes_per_channel = bytes_per_sample / num_channels;

	file->Write("data",4);

	cbuf[0] = (char) (255 & data_length);
	cbuf[1] = (char) (255 & (data_length / 256));
	cbuf[2] = (char) (255 & (data_length / 65536));
	cbuf[3] = (char) (255 & (data_length / 16777216));

	file->Write(cbuf,4);

	for (i=0; i<num_wave_data; i++) {
		for (j=0; j<num_channels; j++) {
			divisor = 1;
			data = wave_data[i+(j*num_wave_data)];
			for (k=0; k<bytes_per_channel; k++) {
				cbuf[k+(j*bytes_per_channel)] = (char) (255 & (data / divisor));
				divisor = 256 * divisor;
			}
		}
		file->Write(cbuf,bytes_per_sample);
	}

}

void CWaveFile::AddListChunk(CFile *file)
{
	char cbuf[50];
	CString sbuf;

	sbuf = "LIST";
	file->Write(sbuf,sbuf.GetLength());

	cbuf[0] = 12;
	cbuf[1] = 0;
	cbuf[2] = 0;
	cbuf[3] = 0;
	file->Write(cbuf,4);

	sbuf = "Testing 2004";

}

