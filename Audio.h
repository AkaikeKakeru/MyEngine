#pragma once

class Audio {
public:
	void Initialize();

private:

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;
public:
	static Audio* GetInstance();

};