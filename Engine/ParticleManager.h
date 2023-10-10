#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include "Base/DirectXCommon.h"

using namespace Microsoft::WRL;

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

	};

private: // 定数
	const int division = 50;					// 分割数
	const float radius = 5.0f;				// 底面の半径
	const float prizmHeight = 8.0f;			// 柱の高さ
	const int planeCount = division * 2 + division * 2;		// 面の数

	static const int vertexCount = 1024;		// 頂点数

public: // 静的メンバ関数


	ParticleManager();

	~ParticleManager();

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(DirectXCommon* dx, int windowWidth, int windowHeight);

	/// <summary>
	/// 静的更新
	/// </summary>
	/// <param name="eye">視点座標</param>
	/// <param name="target">視点方向座標</param>
	static void StaticUpdate(XMFLOAT3 eye, XMFLOAT3 target);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	ParticleManager* Create(DirectXCommon* dx, const std::string& resourcename);

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetEye() { return eye_; }

	/// <summary>
	/// 視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetEye(XMFLOAT3 eye_);

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetTarget() { return target_; }

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetTarget(XMFLOAT3 target);

	/// <summary>
	/// ベクトルによる移動
	/// </summary>
	/// <param name="move">移動量</param>
	void CameraMoveVector(XMFLOAT3 move);

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

	static DirectXCommon* dx_;

private:// 静的メンバ関数
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	void InitializeDescriptorHeap();

	/// <summary>
	/// カメラ初期化
	/// </summary>
	/// <param name="window_width">画面横幅</param>
	/// <param name="window_height">画面縦幅</param>
	static void InitializeCamera(int windowWidth, int windowHeight);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture();

	void LoadTexture(const std::string& resourcename);

	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	static void UpdateViewMatrix();

public: // メンバ関数
	bool Initialize(const std::string& resourcename);
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//パーティクルの追加
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel);

private: // メンバ変数

	ComPtr<ID3D12Resource> constBuff_; // 定数バッファ

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;

	// ローカルスケール
	XMFLOAT3 scale_ = { 1,1,1 };

	//パーティクル配列
	std::forward_list<Particle> particles_;
};

