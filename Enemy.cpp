#include "Enemy.h"

Enemy* Enemy::GetInstance()
{
	static Enemy instance;
	return &instance;
}

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	FBX_SAFE_DELETE(enemyObject);
}

void Enemy::Initialize(FbxModel* EnemyModel)
{

	//3dオブジェクト生成とモデルのセット
	enemyObject = new FbxObject3D;
	enemyObject->Initialize();
	enemyObject->SetModel(EnemyModel);

}

void Enemy::Update()
{
	//移動
	Move();

	enemyObject->SetPosition(position_);
	enemyObject->SetScale(scale_);
	enemyObject->SetRotate(rotation_);
	enemyObject->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isDead) {
		enemyObject->Draw(cmdList);
	}
}

void Enemy::Move()
{
}

bool Enemy::Collition(XMFLOAT3 pos, XMFLOAT3 size)
{
	XMFLOAT3 ePos = position_;
	XMFLOAT3 eSize = scale_;

	float ePosX1 = ePos.x - eSize.x;
	float ePosX2 = ePos.x + eSize.x;

	float ePosY1 = ePos.y - eSize.y;
	float ePosY2 = ePos.y + eSize.y;

	float ePosZ1 = ePos.z - eSize.z;
	float ePosZ2 = ePos.z + eSize.z;

	XMFLOAT3 bPos = pos;
	XMFLOAT3 bSize = size;

	float bPosX1 = bPos.x - bSize.x;
	float bPosX2 = bPos.x + bSize.x;

	float bPosY1 = bPos.y - bSize.y;
	float bPosY2 = bPos.y + bSize.y;

	float bPosZ1 = bPos.z - bSize.z;
	float bPosZ2 = bPos.z + bSize.z;


	if (ePosX1 < bPosX2 && bPosX1 < ePosX2) {
		if (ePosY1 < bPosY2 && bPosY1 < ePosY2) {
			if (ePosZ1 < bPosZ2 && bPosZ1 < ePosZ2) {

				//自機の弾に当たったら死亡
				isDead = true;

			}
		}
	}

	return isDead;
}
