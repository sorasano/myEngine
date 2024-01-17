#include "Menu.h"
#include "Imgui.h"

void Menu::Initialize()
{
	//インスタンス取得
	input_ = Input::GetInstance();
	collisionManager_ = Collision::GetInstance();

	baseSprite_ = new Sprite();
	baseSprite_->SetTextureNum(7);
	baseSprite_->Initialize();
	baseSprite_->SetScale(XMFLOAT2(400, 500));
	baseSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

	settingSprite_ = new Sprite();
	settingSprite_->SetTextureNum(8);
	settingSprite_->Initialize();
	settingSprite_->SetScale(XMFLOAT2(400, 100));
	settingSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 - 50));

	titleSprite_ = new Sprite();
	titleSprite_->SetTextureNum(9);
	titleSprite_->Initialize();
	titleSprite_->SetScale(XMFLOAT2(400, 100));
	titleSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 + 50));

	closeSprite_ = new Sprite();
	closeSprite_->SetTextureNum(10);
	closeSprite_->Initialize();
	closeSprite_->SetScale(XMFLOAT2(400, 100));
	closeSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 + 150));

	//-----セッティング------
	baseSettingSprite = new Sprite();
	baseSettingSprite->SetTextureNum(11);
	baseSettingSprite->Initialize();
	baseSettingSprite->SetScale(XMFLOAT2(800, 500));
	baseSettingSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));

	settingMouseLockSprite_ = new Sprite();
	settingMouseLockSprite_->SetTextureNum(14);
	settingMouseLockSprite_->Initialize();
	settingMouseLockSprite_->SetScale(XMFLOAT2(800, 100));
	settingMouseLockSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 - 50));

	settingMouseLockOnSprite_ = new Sprite();
	settingMouseLockOnSprite_->SetTextureNum(12);
	settingMouseLockOnSprite_->Initialize();
	settingMouseLockOnSprite_->SetScale(XMFLOAT2(200, 100));
	settingMouseLockOnSprite_->SetPosition(XMFLOAT2(window_width / 2 - 100, window_height / 2 + 50));

	settingMouseLockOffSprite_ = new Sprite();
	settingMouseLockOffSprite_->SetTextureNum(13);
	settingMouseLockOffSprite_->Initialize();
	settingMouseLockOffSprite_->SetScale(XMFLOAT2(200, 100));
	settingMouseLockOffSprite_->SetPosition(XMFLOAT2(window_width / 2 + 100, window_height / 2 + 50));

	closeSettingSprite_ = new Sprite();
	closeSettingSprite_->SetTextureNum(10);
	closeSettingSprite_->Initialize();
	closeSettingSprite_->SetScale(XMFLOAT2(400, 100));
	closeSettingSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 + 150));

	gideSprite_ = new Sprite();
	gideSprite_->SetTextureNum(15);
	gideSprite_->Initialize();
	gideSprite_->SetScale(XMFLOAT2(400, 500));
	gideSprite_->SetPosition(XMFLOAT2(window_width / 2 + 425, window_height / 2));

	this->position_ = baseSprite_->GetPosition();

}

void Menu::Update()
{

	//選択中判定
	SerectCollision();

	//選択中を強調
	SerectEmphasis();

	//選択確定時
	SerectConfirm();

	//選択されているもの表示(設定画面)
	SerectDecided();

	if (!isSetting_) {
		baseSprite_->Update();
		settingSprite_->Update();
		titleSprite_->Update();
		closeSprite_->Update();
		gideSprite_->Update();

	}
	else {
		baseSettingSprite->Update();
		closeSettingSprite_->Update();
		settingMouseLockSprite_->Update();
		settingMouseLockOnSprite_->Update();
		settingMouseLockOffSprite_->Update();
	}
}

void Menu::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isSetting_) {

		baseSprite_->Draw(cmdList);
		settingSprite_->Draw(cmdList);
		titleSprite_->Draw(cmdList);
		closeSprite_->Draw(cmdList);
		gideSprite_->Draw(cmdList);

	}
	else {
		baseSettingSprite->Draw(cmdList);
		closeSettingSprite_->Draw(cmdList);
		settingMouseLockSprite_->Draw(cmdList);
		settingMouseLockOnSprite_->Draw(cmdList);
		settingMouseLockOffSprite_->Draw(cmdList);

	}

}

void Menu::SerectCollision()
{
	XMFLOAT2 mousePos = input_->GetMousePosition();

	//操作制限の場合マウス位置を画面外に
	if (isLockOperation_) {
		mousePos = { -1,-1 };
	}


	if (!isSetting_) {

		if (collisionManager_->CheckSpriteTo2Dpos(settingSprite_, mousePos)) {
			serect_ = MENUSETTING;
		}
		else if (collisionManager_->CheckSpriteTo2Dpos(titleSprite_, mousePos)) {
			serect_ = MENUTITLE;
		}
		else if (collisionManager_->CheckSpriteTo2Dpos(closeSprite_, mousePos)) {
			serect_ = MENUCLOSE;
		}
		else {
			serect_ = NONE;
		}
	}
	else {

		if (collisionManager_->CheckSpriteTo2Dpos(closeSettingSprite_, mousePos)) {
			serect_ = SETTINGCLOSE;
		}
		else if (collisionManager_->CheckSpriteTo2Dpos(settingMouseLockOnSprite_, mousePos)) {
			serect_ = SETTINGMOUSEON;
		}
		else if (collisionManager_->CheckSpriteTo2Dpos(settingMouseLockOffSprite_, mousePos)) {
			serect_ = SETTINGMOUSEOFF;
		}
		else {
			serect_ = NONE;
		}
	}


}

void Menu::SerectEmphasis()
{

	//選択中を強調

	if (!isSetting_) {

		switch (serect_) {

		case MENUSETTING:
			settingSprite_->color = { 1,1,0,1 };

			titleSprite_->color = { 1,1,1,1 };
			closeSprite_->color = { 1,1,1,1 };

			break;
		case MENUTITLE:
			titleSprite_->color = { 1,1,0,1 };

			settingSprite_->color = { 1,1,1,1 };
			closeSprite_->color = { 1,1,1,1 };

			break;
		case MENUCLOSE:
			closeSprite_->color = { 1,1,0,1 };

			titleSprite_->color = { 1,1,1,1 };
			settingSprite_->color = { 1,1,1,1 };
			break;
		}

	}
	else {

		switch (serect_) {


		case SETTINGMOUSEON:
			settingMouseLockOnSprite_->color = { 1,1,0,1 };

			closeSettingSprite_->color = { 1,1,1,1 };
			settingMouseLockOffSprite_->color = { 1,1,1,1 };

			break;
		case SETTINGMOUSEOFF:

			settingMouseLockOffSprite_->color = { 1,1,0,1 };

			settingMouseLockOnSprite_->color = { 1,1,1,1 };
			closeSettingSprite_->color = { 1,1,1,1 };
			break;
		case SETTINGCLOSE:
			closeSettingSprite_->color = { 1,1,0,1 };

			settingMouseLockOnSprite_->color = { 1,1,1,1 };
			settingMouseLockOffSprite_->color = { 1,1,1,1 };

			break;
		}

	}

	//何も選択していないときはすべて基本色
	if (serect_ == NONE) {
		settingSprite_->color				= { 1,1,1,1 };
		titleSprite_->color					= { 1,1,1,1 };
		closeSprite_->color					= { 1,1,1,1 };
		settingMouseLockOnSprite_->color	= { 1,1,1,1 };
		closeSettingSprite_->color			= { 1,1,1,1 };
		settingMouseLockOffSprite_->color	= { 1,1,1,1 };
	}

}

void Menu::SerectConfirm()
{
	if (serect_ != NONE) {

		if (input_->IsMouseTrigger(LEFT_CLICK)) {

			if (!isSetting_) {

				switch (serect_) {

				case MENUSETTING:
					isSetting_ = true;
					break;
				case MENUTITLE:
					isSerect_ = true;

					//メニュー操作ロック
					isLockOperation_ = true;

					break;
				case MENUCLOSE:
					isSerect_ = true;
					break;
				}

			}
			else {

				switch (serect_) {

				case SETTINGMOUSEON:
					//マウスウィンドウズ内ロック
					input_->WindowLock();
					break;
				case SETTINGMOUSEOFF:
					//マウスウィンドウズ内アンロック
					input_->WindowUnLock();
					break;
				case SETTINGCLOSE:
					isSetting_ = false;
					break;
				}
			}
		}
	}

}

void Menu::SerectDecided()
{
	//選択中表示

	//マウスロック
	{
		bool isMouseLock = input_->GetIsLockInWindow();

		if (isMouseLock) {
			settingMouseLockOnSprite_->color = { 1,0,0,1 };
		}
		else {
			settingMouseLockOffSprite_->color = { 1,0,0,1 };
		}
	}
}

void Menu::SetPosition(const XMFLOAT2& position)
{
	//ベースを移動
	baseSprite_->SetPosition(position);
	//スプライトの移動量
	XMFLOAT2 positionMove = XMFLOAT2{ this->position_.x - position.x , this->position_.y - position.y };

	//移動しただけ追従して移動させる
	XMFLOAT2 restartPosition = XMFLOAT2{ settingSprite_->GetPosition().x - positionMove.x ,  settingSprite_->GetPosition().y - positionMove.y };
	settingSprite_->SetPosition(restartPosition);

	XMFLOAT2 titlePosition = XMFLOAT2{ titleSprite_->GetPosition().x - positionMove.x ,  titleSprite_->GetPosition().y - positionMove.y };
	titleSprite_->SetPosition(titlePosition);

	XMFLOAT2 closePosition = XMFLOAT2{ closeSprite_->GetPosition().x - positionMove.x ,  closeSprite_->GetPosition().y - positionMove.y };
	closeSprite_->SetPosition(closePosition);
}

void Menu::CloseReset()
{
	serect_ = NONE;
	isSerect_ = false;
	isSetting_ = false;

}

void Menu::Reset()
{
	CloseReset();

	isLockOperation_ = false;
}