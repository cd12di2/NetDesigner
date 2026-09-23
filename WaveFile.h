
#define	MAX_WAVE_FILES	100



class CWaveFile 
{

public:

	CString filename;
	unsigned long filelength;
	
	int num_channels;
	int bytes_per_sample;
	int bits_per_sample;

	long file_length;
	long sample_rate;
	long bytes_per_second;
	long data_length;

	BOOL wave_data_allocated;
	long num_wave_data;
	long *wave_data;

	CString chunk_id;
	long chunk_length;

public:

	CWaveFile();
	~CWaveFile();
	void Initialize();

	void addErrorString(CString err);

	void setNumChannels(int n) { num_channels = n; };
	void setBytesPerSample(int b) { bytes_per_sample = b; };
	void setBitsPerSample(int b) { bits_per_sample = b; };
	void setSampleRate(long r) { sample_rate = r; };
	void setBytesPerSecond(long b) { bytes_per_second = b; };
	void setNumWaveData(long n) { num_wave_data = n; };
	void setWaveDataPointer(long *p) { wave_data = p; };

	int getBytesPerSample() { return(bytes_per_sample); };
	long getSampleRate() { return(sample_rate); };

	BOOL ReadWaveFile(string name);

	void GetNextChunk(CFile *file);
	void SkipChunk(CFile *file);
	void GetFormatChunk(CFile *file);
	void GetDataChunk(CFile *file);
	void GetList(CFile *file);

	void SaveWaveFile(CString name);

	void SaveRiffChunk(CFile *file);
	void SaveFormatChunk(CFile *file);
	void SaveDataChunk(CFile *file);
	void AddListChunk(CFile *file);

	void DisplayChunkInfo();

};


