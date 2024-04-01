#pragma once
/**
* @file BaseScene.h
* @brief シーンの基底クラス
*/

#pragma once
#include "SceneCommonData.h"

//前方宣言
class SceneManager;

class BaseScene {

public:

	BaseScene();

	BaseScene(SceneCommonData* cData);

	/**
	* デストラクタ
	*/
	virtual ~BaseScene() = default;

	/**
	* 初期化
	*
	*/
	virtual void Initialize() = 0;
	/**
	* 更新
	*/
	virtual void Update() = 0;
	/**
	* 描画
	*/
	virtual void Draw() = 0;
	/**
	* スプライト描画
	*/
	virtual void DrawSprite() = 0;

	/**
	* 終了処理
	*/
	virtual void Finalize() = 0;


	//共通処理
	/**
	* 初期化
	*/
	void CommonInitialize();
	/**
	* 更新
	*/
	void CommonUpdate();
	/**
	* 描画
	*/
	void CommonDraw();
	/**
	* スプライト描画
	*/
	void CommonDrawSprite();

	/**
	* プレイシーン初期化
	*/
	void PlaySceneInitialize();


	/**
	* 敵配置
	*/
	void SetEnemy();
	/**
	* 更新範囲
	*
	* @param[in] cameraPos カメラ座標
	* @param[in] pos 対象座標
	* @return bool 更新範囲内ならtrue
	*/
	bool UpadateRange(const XMFLOAT3& cameraPos, const XMFLOAT3& pos);
	/**
	* メニューUI判定
	*
	* @return true = UIが押された
	*/
	bool MenuUIColision();

	/**
	* リセット
	*/
	void Reset();

private:

	/**
	* シーン切り替え
	*/
	virtual void ChangeScene() = 0;
	/**
	* 当たり判定
	*/
	virtual void Collition() = 0;
	/**
	* 背景更新
	*/
	void UpdateBackGround();


public:

	//セッター
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }
	virtual void SetSceneCommonData(SceneCommonData* sceneCommonData) { cData_ = sceneCommonData; }

protected:

	//シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;

	//他共通データ
	SceneCommonData* cData_;
};

