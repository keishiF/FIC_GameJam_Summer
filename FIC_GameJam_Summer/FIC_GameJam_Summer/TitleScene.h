#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class Bullet;
class EnemyBullet;
class TitleBattleActor;

class TitleScene final : public SceneBase
{
public:
	explicit TitleScene(SceneController& controller);
	~TitleScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_titleHandle;
	int m_bgHandle;

	std::vector<int> m_playerBulletAnimHandles;
	std::vector<int> m_enemyBulletAnimHandles;

	std::unique_ptr<TitleBattleActor> m_playerActor;
	std::unique_ptr<TitleBattleActor> m_enemyActor;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	std::vector<std::unique_ptr<EnemyBullet>> m_enemyBullets;

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(TitleScene::*)();
	using DrawFunc_t = void(TitleScene::*)();

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

	// 背景の撃ち合い演出(アクター・弾の更新)
	void UpdateBattleBackground();
	// 背景の撃ち合い演出(アクター・弾の描画)
	void DrawBattleBackground() const;
};