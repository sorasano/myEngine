#include "Player.h"

Player* Player::GetInstance()
{
	static Player instance;
	return &instance;
}

Player::Player()
{
}

Player::~Player()
{
	FBX_SAFE_DELETE(playerObject);
}

void Player::Initialize(FbxModel* model)
{
	//3dオブジェクト生成とモデルのセット
	playerObject = new FbxObject3D;
	playerObject->Initialize();
	playerObject->SetModel(model);

}

void Player::Update()
{
	playerObject->SetPosition(position_);
	playerObject->SetScale(scale_);
	playerObject->SetRotate(rotation_);
	playerObject->Update();
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	playerObject->Draw(cmdList);
}
