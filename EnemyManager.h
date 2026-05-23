#pragma once
#include<vector>
#include<memory>
class Enemy;
class Player;
class Collision;
//class Effect;
class ICharacter;

struct SpawnPoint {
	VECTOR center;
	float radius;
	int max_num;
	bool is_spawned;

	std::vector<std::weak_ptr<Enemy>> live_enemies;

	bool is_cooling_down = false;
	float cool_time = 0.0f;
	float coll_down_limit = 30.0f; //出現間隔（フレーム）
};

class EnemyManager
{
private:
	const int MAX_COUNT = 45;
	std::vector<std::shared_ptr<ICharacter>>enemies;
	std::vector<SpawnPoint>spawn_points;

public:
	EnemyManager();
	~EnemyManager();

	const std::vector<std::shared_ptr<ICharacter>>& getEnemies() const { return enemies; }

	void Init();
	void Update(const Player& player, Collision& collision, const float& deltaTime/*, const Effect& effect*/,const int&mapModelHandle);
	void Draw();
	void AddSpawnPoint(VECTOR center, float radius, int max_num);
	void CheckSpawnPoints(const VECTOR& playerPos, const float delta_time);
	bool IsCharacterWithInRange(const VECTOR& playerPos, const VECTOR& center, const float& radius);
	void ExecuteSpawning(const SpawnPoint& sp);
	void UpdateSpawnCoolTime(SpawnPoint& sp, const float& deltaTime);
};
