#include "Audio.h"
#include <wrl.h>

#include <cassert>

//省略
template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

void Audio::Initialize() {
	HRESULT result;
	
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	//XAudioエンジンのインスタンス生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	SUCCEEDED(result);
	
	//マスタリングボイス生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	SUCCEEDED(result);
}

Audio::SoundData Audio::SoundLoadWave(const char* filename) {
#pragma region OpenFile
	std::ifstream file;
	//wavをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出
	assert(file.is_open());
#pragma endregion

#pragma region LoadWave
	//RIFFヘッダー読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFがチェック
	if (strncmp(riff.chunk_.id_, "RIFF", 4) != 0) {
		assert(0);
	}
	//ファイルがWAVEがチェック
	if (strncmp(riff.chunk_.id_, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダー確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if(strncmp(format.chunk_.id_,"fmt ",4) != 0) {
		assert(0);
	}
	assert(format.chunk_.size_ <= sizeof(format.fmt_));
	file.read((char*)&format.fmt_, format.chunk_.size_);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出したら
	if (strncmp(data.id_, "JUNK", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size_,std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id_, "data", 4) != 0) {
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size_];
	file.read(pBuffer, data.size_);

	//Waveファイルを閉じる
	file.close();

#pragma endregion

	//返す音声データ
	SoundData soundData = {};

	soundData.wfex_ = format.fmt_;
	soundData.pBuffer_ = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize_ = data.size_;

	return soundData;
}

void Audio::SoundUnload(SoundData* soundData) {
	//バッファのメモリを解放
	delete[] soundData->pBuffer_;

	soundData->pBuffer_ = 0;
	soundData->bufferSize_ = 0;
	soundData->wfex_ = {};
}

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}
