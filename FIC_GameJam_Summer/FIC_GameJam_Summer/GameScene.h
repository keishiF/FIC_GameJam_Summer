#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class Physics;
class StageObjectManager;
class GameObjectManager;
class UIManager;
class Player;
class Bullet;

class GameScene final : public SceneBase
{
public:
	explicit GameScene(SceneController& controller, int stageNo);
	~GameScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_stageNo;
	int m_stagebgHandle;

	std::vector<int> m_bulletAnimHandles;

	std::unique_ptr<Player> m_player;
	std::vector<std::unique_ptr<Bullet>> m_bullets;

	int m_remainingBullets;	// このステージで残っている弾数

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(GameScene::*)();
	using DrawFunc_t = void(GameScene::*)();
	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	// 通常時更新処理
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();

	// 弾を発射できるか判定し、発射処理を行う
	void TryShoot();
	// 弾の生成・更新・削除
	void UpdateBullets();
};