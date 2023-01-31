#pragma once

class Audio {
public://構造体
	//チャンクヘッダ
	struct ChunkHeader {
		char id_[4];//チャンクのID
		int32_t size_;//チャンクサイズ
	};

	//RIFFヘッダファイル
	struct RiffHeader {
		ChunkHeader chunk_;//"RIFF"
		char type_[4];//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk_;//"FMT"
		WAVEFORMAT fmt_;//波形フォーマット
	};

	//音声データ
	struct SoundData {
		//波形フォーマット
		WAVEFORMAT wfex_;
		//バッファの先頭アドレス
		BYTE* pBuffer_;
		//バッファのサイズ
		unsigned int bufferSize_;
	};


public://メンバ関数
	void Initialize();

	//音声データ読み込み
	SoundData SoundLoadWave(const char* filename);

private:

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;
public:
	static Audio* GetInstance();
};