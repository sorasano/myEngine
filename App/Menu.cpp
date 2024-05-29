#include "Menu.h"
#include "Imgui.h"

void Menu::Initialize()
{
	//インスタンス取得
	input_ = MyEngine::Input::GetInstance();

	std::unique_ptr<Sprite> newBaseSprite_ =std::make_unique<Sprite>();
	newBaseSprite_->SetTextureNum(7);
	newBaseSprite_->Initialize();
	newBaseSprite_->SetScale(XMFLOAT2(400, 500));
	newBaseSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	baseSprite_.swap(newBaseSprite_);

	std::unique_ptr<Sprite> newSettingSprite_ =std::make_unique<Sprite>();
	newSettingSprite_->SetTextureNum(8);
	newSettingSprite_->Initialize();
	newSettingSprite_->SetScale(XMFLOAT2(400, 100));
	newSettingSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 - 50));
	settingSprite_.swap(newSettingSprite_);

	std::unique_ptr<Sprite> newTitleSprite_ =std::make_unique<Sprite>();
	newTitleSprite_->SetTextureNum(9);
	newTitleSprite_->Initialize();
	newTitleSprite_->SetScale(XMFLOAT2(400, 100));
	newTitleSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 + 50));
	titleSprite_.swap(newTitleSprite_);

	std::unique_ptr<Sprite> newCloseSprite_ =std::make_unique<Sprite>();
	newCloseSprite_->SetTextureNum(10);
	newCloseSprite_->Initialize();
	newCloseSprite_->SetScale(XMFLOAT2(400, 100));
	newCloseSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 + 150));
	closeSprite_.swap(newCloseSprite_);

	//-----セッティング------
	std::unique_ptr<Sprite> newBaseSettingSprite =std::make_unique<Sprite>();
	newBaseSettingSprite->SetTextureNum(11);
	newBaseSettingSprite->Initialize();
	newBaseSettingSprite->SetScale(XMFLOAT2(800, 500));
	newBaseSettingSprite->SetPosition(XMFLOAT2(window_width / 2, window_height / 2));
	baseSettingSprite.swap(newBaseSettingSprite);

	std::unique_ptr<Sprite> newSettingMouseLockSprite_ =std::make_unique<Sprite>();
	newSettingMouseLockSprite_->SetTextureNum(14);
	newSettingMouseLockSprite_->Initialize();
	newSettingMouseLockSprite_->SetScale(XMFLOAT2(800, 100));
	newSettingMouseLockSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 - 50));
	settingMouseLockSprite_.swap(newSettingMouseLockSprite_);

	std::unique_ptr<Sprite> newSettingMouseLockOnSprite_ =std::make_unique<Sprite>();
	newSettingMouseLockOnSprite_->SetTextureNum(12);
	newSettingMouseLockOnSprite_->Initialize();
	newSettingMouseLockOnSprite_->SetScale(XMFLOAT2(200, 100));
	newSettingMouseLockOnSprite_->SetPosition(XMFLOAT2(window_width / 2 - 100, window_height / 2 + 50));
	settingMouseLockOnSprite_.swap(newSettingMouseLockOnSprite_);

	std::unique_ptr<Sprite> newSettingMouseLockOffSprite_ =std::make_unique<Sprite>();
	newSettingMouseLockOffSprite_->SetTextureNum(13);
	newSettingMouseLockOffSprite_->Initialize();
	newSettingMouseLockOffSprite_->SetScale(XMFLOAT2(200, 100));
	newSettingMouseLockOffSprite_->SetPosition(XMFLOAT2(window_width / 2 + 100, window_height / 2 + 50));
	settingMouseLockOffSprite_.swap(newSettingMouseLockOffSprite_);

	std::unique_ptr<Sprite> newCloseSettingSprite_ =std::make_unique<Sprite>();
	newCloseSettingSprite_->SetTextureNum(10);
	newCloseSettingSprite_->Initialize();
	newCloseSettingSprite_->SetScale(XMFLOAT2(400, 100));
	newCloseSettingSprite_->SetPosition(XMFLOAT2(window_width / 2, window_height / 2 + 150));
	closeSettingSprite_.swap(newCloseSettingSprite_);

	std::unique_ptr<Sprite> newGideSprite_ =std::make_unique<Sprite>();
	newGideSprite_->SetTextureNum(15);
	newGideSprite_->Initialize();
	newGideSprite_->SetScale(XMFLOAT2(400, 500));
	newGideSprite_->SetPosition(XMFLOAT2(window_width / 2 + 425, window_height / 2));
	gideSprite_.swap(newGideSprite_);

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

		if (Collision::CheckSpriteTo2Dpos(settingSprite_.get(), mousePos)) {
			serect_ = MENUSETTING;
		}
		else if (Collision::CheckSpriteTo2Dpos(titleSprite_.get(), mousePos)) {
			serect_ = MENUTITLE;
		}
		else if (Collision::CheckSpriteTo2Dpos(closeSprite_.get(), mousePos)) {
			serect_ = MENUCLOSE;
		}
		else {
			serect_ = NONE;
		}
	}
	else {

		if (Collision::CheckSpriteTo2Dpos(closeSettingSprite_.get(), mousePos)) {
			serect_ = SETTINGCLOSE;
		}
		else if (Collision::CheckSpriteTo2Dpos(settingMouseLockOnSprite_.get(), mousePos)) {
			serect_ = SETTINGMOUSEON;
		}
		else if (Collision::CheckSpriteTo2Dpos(settingMouseLockOffSprite_.get(), mousePos)) {
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