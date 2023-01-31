#include "Audio.h"
#include <xaudio2.h>

//省略
template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma comment(lib,"xaudio.2lib")

#include <fstream>

void Audio::Initialize() {
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
}

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}
