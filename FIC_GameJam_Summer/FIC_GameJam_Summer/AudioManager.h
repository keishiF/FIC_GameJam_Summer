#pragma once
#include <DxLib.h>

class AudioManager
{
public:
	enum class BGM
	{
		Title,
		Battle,
		Clear,
		GameOver,
	};

	static AudioManager& GetInstance();

	// DxLib が初期化されたあとで呼ぶ
	void Init();

	void PlayBGM(BGM bgm);
	void StopBGM();

	void PlayShotSE();

private:
	AudioManager();
	~AudioManager();

	int m_bgmTitleHandle;
	int m_bgmBattleHandle;
	int m_bgmClearHandle;
	int m_bgmGameOverHandle;
	int m_shotSeHandle;

	int m_currentBgmHandle;
};
