#include"DxLib.h"
#include"Enemy.h"
#include"Collision.h"
#include"Player.h"
#include "EnemyManager.h"
//#define DEBUG

EnemyManager::EnemyManager()
{
	AddSpawnPoint(VGet(-2490.0f, 0.0f, -5126.0f), 500.0f, 3);
	AddSpawnPoint(VGet(-9056.0f, 0.0f, 11176.0f), 500.0f, 10);
	AddSpawnPoint(VGet(-2418.0f, 0.0f, -560.0f), 500.0f, 5);
	AddSpawnPoint(VGet(-4366.0f, 0.0f, 3884.0f), 500.0f, 4);
	AddSpawnPoint(VGet(-3450.0f, 0.0f, 10452.0f), 500.0f, 10);
	AddSpawnPoint(VGet(-1379.0f, 0.0f, 8676.0f), 500.0f, 10);
	AddSpawnPoint(VGet(1814.0f, 0.0f, -262.0f), 500.0f, 3);


	//既存のリストをクリア
	enemies.clear();

	//最初に最大数まで生成してプールしておく
	for (int i = 0; i < MAX_COUNT; i++) {
		auto new_enemy = std::make_shared<Enemy>();
		//この時点では active = false にしておく
		new_enemy->SetIsActive(false);
		enemies.push_back(new_enemy);
	}
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
	//スポーンポイントの状態もリセット（リトライ対応）
	for (auto& sp : spawn_points) {
		sp.is_spawned = false;
		sp.is_cooling_down = false;
		sp.cool_time = 0.0f;
		sp.live_enemies.clear(); // weak_ptrのリストもクリア
	}
	for (auto& character : enemies) {
		auto enemy = std::static_pointer_cast<Enemy>(character);
		enemy->SetIsActive(false);
	}
}

void EnemyManager::Update(const Player& player, Collision& collision, const float& deltaTime/*, const Effect& effect*/,const int&mapModelHandle)
{
	CheckSpawnPoints(player.getPos(), deltaTime);


	// 更新処理
	for (auto& character : enemies) {
		//ICharacterからEnemyにダウンキャスト
		auto enemy = std::static_pointer_cast<Enemy>(character);

		if (!enemy->getIsActive()) continue; // アクティブでないならスキップ

		if (enemy->getIsFinished()) {
			enemy->SetIsActive(false); // 倒れたら非アクティブにする
		}
		else {
			enemy->IsHitColl(player.getPos(), enemy->getCollCapsule());
			enemy->Update(player, collision, deltaTime/*, effect*/, mapModelHandle);
		}
	}

	// 衝突判定（アクティブな敵同士のみ）
	for (size_t i = 0; i < enemies.size(); i++) {
		auto enemyI = std::static_pointer_cast<Enemy>(enemies[i]);

		if (!enemyI->getIsActive()) continue;
		for (size_t j = i + 1; j < enemies.size(); j++) {
			auto enemyJ = std::static_pointer_cast<Enemy>(enemies[j]);

			if (!enemyJ->getIsActive()) continue;

			Capsule cap1 = enemyI->getCollCapsule();
			Capsule cap2 = enemyJ->getCollCapsule();
			if (collision.CapsuleToCapsule(cap1, cap2)) {
				enemyI->Extrusion(cap1, cap2, collision);
			}
		}
	}
}

void EnemyManager::Draw()
{
	for (auto& character : enemies) {
		auto enemy = std::static_pointer_cast<Enemy>(character);
		if (enemy->getIsActive()) {
			enemy->Draw();
		}
	}

#ifdef DEBUG
	for (auto& sp : spawn_points) {
		unsigned int color = GetColor(0, 255, 0); // 基本は緑（出現待ち）

		if (sp.is_cooling_down) {
			color = GetColor(0, 0, 255); // クールタイムは青
		}
		else if (sp.is_spawned) {
			color = GetColor(255, 0, 0); // 出現中は赤
		}

		DrawSphere3D(sp.center, sp.radius, 8, color, GetColor(0, 0, 0), false);
		// リセット半径はうっすら白で表示
		DrawSphere3D(sp.center, sp.radius * 2.5f, 8, GetColor(100, 100, 100), GetColor(0, 0, 0), false);
	}
#endif // DEBUG

}

void EnemyManager::AddSpawnPoint(VECTOR center, float radius, int max_num)
{
	spawn_points.push_back({ center,radius,max_num,false });
}

void EnemyManager::CheckSpawnPoints(const VECTOR& playerPos,const float delta_time)
{	
	for (auto& sp : spawn_points) {
		int current_alive_in_this_point = 0;
		if (sp.is_cooling_down) {
			UpdateSpawnCoolTime(sp,delta_time);
			continue;
		}

		if (!sp.is_spawned) {
			if (IsCharacterWithInRange(playerPos,sp.center,sp.radius)) {
				ExecuteSpawning(sp);
				sp.is_spawned = true;
			}
		}
		else {
			if( !IsCharacterWithInRange(playerPos,sp.center, sp.radius * 2.5f)) {
				sp.is_spawned = false;
			}
		}
	}
}

bool EnemyManager::IsCharacterWithInRange(const VECTOR& playerPos,const VECTOR& center,const float&radius)
{
	float dx = playerPos.x - center.x;
	float dy = playerPos.y - center.y;
	float dz = playerPos.z - center.z;
	float dist_sq = (dx * dx) + (dy * dy) + (dz * dz);
	float radius_sq = radius * radius;

	return dist_sq <= radius_sq;
}

void EnemyManager::ExecuteSpawning(const SpawnPoint& sp)
{
	int spawned_count = 0;
	for (auto& character : enemies) {
		auto enemy = std::static_pointer_cast<Enemy>(character);
		if (enemy->getIsActive())continue;
		// 出現位置をランダムに設定して初期化
		VECTOR start_pos = VGet(sp.center.x + GetRand(100) - 50.0f, sp.center.y, sp.center.z + GetRand(100) - 50.0f);
		enemy->Init(start_pos);
		enemy->SetIsActive(true); // ここで有効化

		spawned_count++;
		if (spawned_count >= sp.max_num) break; // このポイントの規定数に達したら終了
	}
}

void EnemyManager::UpdateSpawnCoolTime(SpawnPoint& sp,const float& deltaTime)
{
	sp.cool_time += deltaTime;
	if (sp.cool_time >= sp.coll_down_limit) {
		sp.is_cooling_down = false;
		sp.cool_time = 0.0f;
	}
}
