#include "AudioManager.h"
#include <cassert>

AudioManager::AudioManager()
	: m_bgmTitleHandle(-1)
	, m_bgmBattleHandle(-1)
	, m_bgmClearHandle(-1)
	, m_bgmGameOverHandle(-1)
	, m_shotSeHandle(-1)
	, m_currentBgmHandle(-1)
{
}

AudioManager::~AudioManager()
{
	if (m_bgmTitleHandle > 0) { StopSoundMem(m_bgmTitleHandle); DeleteSoundMem(m_bgmTitleHandle); }
	if (m_bgmBattleHandle > 0) { StopSoundMem(m_bgmBattleHandle); DeleteSoundMem(m_bgmBattleHandle); }
	if (m_bgmClearHandle > 0) { StopSoundMem(m_bgmClearHandle); DeleteSoundMem(m_bgmClearHandle); }
	if (m_bgmGameOverHandle > 0) { StopSoundMem(m_bgmGameOverHandle); DeleteSoundMem(m_bgmGameOverHandle); }
	if (m_shotSeHandle > 0) { StopSoundMem(m_shotSeHandle); DeleteSoundMem(m_shotSeHandle); }
}

AudioManager& AudioManager::GetInstance()
{
	static AudioManager instance;
	return instance;
}

void AudioManager::Init()
{
	// Data フォルダに置かれたファイルを読み込む
	m_bgmTitleHandle = LoadSoundMem("Data/TitleBGM.mp3");
	m_bgmBattleHandle = LoadSoundMem("Data/BattleBGM.mp3");
	m_bgmClearHandle = LoadSoundMem("Data/ClearBGM.mp3");
	m_bgmGameOverHandle = LoadSoundMem("Data/GameOver.mp3");
	m_shotSeHandle = LoadSoundMem("Data/Shot.mp3");

	// 読み込みに失敗している場合は -1 になっているので一応アサート
	// 実行環境によってはmp4がサポート外かもしれないためデプロイ時に注意
	// アサートは開発時のみ有効
	assert(m_bgmTitleHandle > 0 && "Failed to load TitleBGM");
	assert(m_bgmBattleHandle > 0 && "Failed to load BattleBGM");
	assert(m_bgmClearHandle > 0 && "Failed to load ClearBGM");
	assert(m_bgmGameOverHandle > 0 && "Failed to load GameOverBGM");
	assert(m_shotSeHandle > 0 && "Failed to load ShotSE");
}

void AudioManager::PlayBGM(BGM bgm)
{
	int handle = -1;
	switch (bgm)
	{
	case BGM::Title: handle = m_bgmTitleHandle; break;
	case BGM::Battle: handle = m_bgmBattleHandle; break;
	case BGM::Clear: handle = m_bgmClearHandle; break;
	case BGM::GameOver: handle = m_bgmGameOverHandle; break;
	}

	if (handle <= 0)
	{
		return;
	}

	if (m_currentBgmHandle == handle)
	{
		// 既に同じBGMが再生中
		return;
	}

	// 既に流れているBGMを止める
	if (m_currentBgmHandle > 0)
	{
		StopSoundMem(m_currentBgmHandle);
	}

	m_currentBgmHandle = handle;
	PlaySoundMem(m_currentBgmHandle, DX_PLAYTYPE_LOOP);
}

void AudioManager::StopBGM()
{
	if (m_currentBgmHandle > 0)
	{
		StopSoundMem(m_currentBgmHandle);
		m_currentBgmHandle = -1;
	}
}

void AudioManager::PlayShotSE()
{
	if (m_shotSeHandle <= 0)
	{
		return;
	}
	// 効果音は重ねて鳴らしたいので、再生モードは通常再生
	PlaySoundMem(m_shotSeHandle, DX_PLAYTYPE_BACK);
}
