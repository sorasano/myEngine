#pragma once
#include "DirectXMath.h"

#include "Input.h"
#include "Collision.h"
#include "Sprite.h"

enum MenuSerect {
	NONE,//選択なし
	MENUSETTING,//設定
	MENUTITLE,//タイトルにもどる
	MENUCLOSE,//メニューを閉じる
	SETTINGMOUSEON,//マウスロックオン
	SETTINGMOUSEOFF,//マウスロックオフ
	SETTINGCLOSE,//設定画面を閉じる
};



class Menu
{
private: // エイリアス
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;

public:

	/**
	* 初期化
	*/
	void Initialize();

	/**
	* 更新
	*/
	void Update();

	/**
	* 描画
	*
	* @param[out] cmdList コマンドリスト
	*/
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/**
	* 選択判定
	*/
	void SerectCollision();
	/**
	* 選択強調
	*/
	void SerectEmphasis();
	/**
	* 選択確定時
	*/
	void SerectConfirm();
	/**
	* 選択されているもの表示(設定画面)
	*/
	void SerectDecided();


	/**
	* positionセット
	*/
	void SetPosition(const XMFLOAT2& position);
	/**
	* isLockOperation_セット
	*/
	void SetIsLockOperation(bool isLockOperation) { this->isLockOperation_ = isLockOperation; };


	/**
	* @return int serect_取得
	*/
	int GetSerect()const { return serect_; }
	/**
	* @return bool isSerect_取得
	*/
	bool GetIsSerect()const { return isSerect_; }

	/**
	* リセット(メニューを閉じる度)
	*/
	void CloseReset();
	/**
	* リセット(ゲームループ一周後)
	*/
	void Reset();
	/**
	* リセット(設定など全要素)
	*/
	void AllReset();

private:

	//入力
	Input* input_;
	//当たり判定
	Collision* collisionManager_ = nullptr;

	//操作ロック
	bool isLockOperation_ = false;

	//全体の基本位置
	XMFLOAT2 position_ = {};
	//何を選択しているか
	int serect_;
	//シーン変更が確定したか
	bool isSerect_;

	//セッティング画面か
	bool isSetting_ = false;

	//ベースの画像
	Sprite* baseSprite_;
	//"設定画面を開く"
	Sprite* settingSprite_;
	//"タイトルに戻る"
	Sprite* titleSprite_;
	//"メニューを閉じる"
	Sprite* closeSprite_;

	//セッティングベース画像
	Sprite* baseSettingSprite;
	//"マウスロック"
	Sprite* settingMouseLockSprite_;
	//"マウスロックオン"
	Sprite* settingMouseLockOnSprite_;
	//"マウスロックオフ"
	Sprite* settingMouseLockOffSprite_;
	//"セッティングを閉じる"
	Sprite* closeSettingSprite_;
};