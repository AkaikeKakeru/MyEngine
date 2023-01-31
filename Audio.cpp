#include "Audio.h"
#include <xaudio2.h>
#include <wrl.h>

#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")


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

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}
