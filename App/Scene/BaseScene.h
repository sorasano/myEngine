#pragma once
/**
* @file BaseScene.h
* @brief シーンの基底クラス
*/

//前方宣言
class SceneManager;

class BaseScene {

public:
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

	//セッター

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:
	//シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;


};

