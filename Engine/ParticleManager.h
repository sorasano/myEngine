/**
* @file ParticleManager.h
* @brief パーティクルの処理
*/

#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include "Base/DirectXCommon.h"

#include "Random.h"

using namespace Microsoft::WRL;

enum Partile {
	ENEMYDESTROY,//敵撃破演出
	PLAYERBULLETLANDING,//プレイヤー弾着弾演出
};

class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//モデル格納ルートパス
	static const std::string baseDirectory;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz座標
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{

		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matbBillboard;
	};

	struct Particle {
		//DirectX::を省略
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//座標
		XMFLOAT3 position_ = {};
		//速度
		XMFLOAT3 velocity = {};
		//加速度
		XMFLOAT3 accel = {};
		//現在フレーム
		int frame = 0;
		//終了フレーム
		int num_flame = 0;
		//死亡フラグ
		bool isDead = false;
	};

private: // 定数
	const int division = 50;					// 分割数
	const float radius = 5.0f;				// 底面の半径
	const float prizmHeight = 8.0f;			// 柱の高さ
	const int planeCount = division * 2 + division * 2;		// 面の数

	static const int vertexCount = 1024;		// 頂点数

public: // 静的メンバ関数


	/**
	* コンストラクタ
	*/
	ParticleManager();
	/**
	* デストラクタ
	*/
	~ParticleManager();

	/**
	* 静的初期化
	*
	* @param[in,out] dx dxCommon
	*/
	static void StaticInitialize(MyEngine::DirectXCommon* dx);
	/**
	* 静的更新
	* @param[in] eye 視点座標
	* @param[in] target 視点方向座標
	*
	*/
	static void StaticUpdate(XMFLOAT3 eye, XMFLOAT3 target);

	/**
	* 描画前処理
	*
	* @param[out] cmdList コマンドリスト
	*/
	void PreDraw(ID3D12GraphicsCommandList* cmdList);
	/**
	* 描画後処理
	*/
	void PostDraw();

	/**
	* @return int 視点座標の取得取得
	*/
	const XMFLOAT3& GetEye() { return eye_; }
	/**
	* @return int 注視点座標の取得
	*/
	const XMFLOAT3& GetTarget() { return target_; }


	/**
	* 視点座標の設定
	*/
	void SetEye(XMFLOAT3 eye_);
	/**
	* 注視点座標の設定
	*/
	void SetTarget(XMFLOAT3 target);


	/**
	* ベクトルによる視点移動
	*
	* @param[in] move 移動量
	*/
	void CameraMoveVector(XMFLOAT3 move);
	/**
	* ベクトルによる注視点移動
	*
	* @param[in] move 移動量
	*/
	void CameraMoveEyeVector(XMFLOAT3 move);

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device_;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize_;
	// コマンドリスト
	ID3D12GraphicsCommandList* cmdList_ = nullptr;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate_;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//// インデックスバッファ
	// ComPtr<ID3D12Resource> indexBuff;
	//// テクスチャバッファ
	// ComPtr<ID3D12Resource> texbuff;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV_;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV_;
	// ビュー行列
	static XMMATRIX matView_;
	// 射影行列
	static XMMATRIX matProjection_;
	// 視点座標
	static XMFLOAT3 eye_;
	// 注視点座標
	static XMFLOAT3 target_;
	// 上方向ベクトル
	static XMFLOAT3 up_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// 頂点データ配列
	static VertexPos vertices_[vertexCount];
	//// 頂点インデックス配列
	// unsigned short indices[indexCount];

	//ビューボード行列
	static XMMATRIX matBillbord_;
	//Y軸周りのビューボード行列
	static XMMATRIX matBillbordY_;

	static MyEngine::DirectXCommon* dx_;

private:// 静的メンバ関数
	/**
	* デスクリプタヒープの初期化
	*/
	void InitializeDescriptorHeap();
	/**
	* カメラ初期化
	* @param[in] windowWidth 画面横幅
	* @param[in] windowHeight 画面縦幅
	*
	*/
	static void InitializeCamera(int windowWidth, int windowHeight);
	/**
	* グラフィックパイプライン生成
	*/
	static void InitializeGraphicsPipeline();
	/**
	* テクスチャ読み込み
	* 
	* @param[in] resourcename テクスチャ名
	*/
	void LoadTexture(const std::string& resourcename);
	/**
	* モデル作成
	*/
	void CreateModel();
	/**
	* ビュー行列を更新
	*/
	static void UpdateViewMatrix();

public: // メンバ関数
	/**
	* 初期化
	*
	* @param[in] resourcename テクスチャ名
	*/
	bool Initialize(const std::string& resourcename);
	/**
	* 更新
	* 
	* @param[in] cameraPos カメラ座標
	*/
	void Update(XMFLOAT3 cameraPos);
	/**
	* 描画
	*/
	void Draw();
	/**
	* パーティクルの追加
	*
	* @param[in] position 発生座標
	* @param[in] velocity 加速
	* @param[in] accel 加速度
	*/
	void Add(int life, const XMFLOAT3& position, const  XMFLOAT3& velocity, const XMFLOAT3& accel);

	/**
	* パーティクル生成
	*
	*/
	void MakeParticle(const XMFLOAT3& position);

	/**
	* バッファーの更新
	*/
	void BuffUpdate();

private: // メンバ変数

	ComPtr<ID3D12Resource> constBuff_; // 定数バッファ
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;

	// ローカルスケール
	XMFLOAT3 scale_ = { 1,1,1 };
	//パーティクル配列
	std::forward_list<Particle> particles_;

	//パーティクル時間
	const int enemyDestroyParticleTime_ = 30;

};

