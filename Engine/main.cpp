/**
* @file main.cpp
* @brief ソース全体
*/

#include <string>
#include <sstream>
#include <iomanip>

#include "FPS.h"
#include "Input.h"
#include "PostEffect.h"
#include "ImguiManager.h"

#include "SceneManager.h"
#include "TitleScene.h"

//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//ポインタ
	MyEngine::WinApp* winApp = MyEngine::WinApp::GetInstance();
	//WindowsAPIの初期化
	winApp->Initialize();
	//WindowsAPIの終了処理
	winApp->Finalize();

	// DirectX初期化処理
	//インスタンスの取得
	MyEngine::DirectXCommon* dxCommon = MyEngine::DirectXCommon::GetInstance();
	////DirectX初期化
	dxCommon->Initialize();

	////キーボード処理
	//インスタンスの取得
	MyEngine::Input* input = MyEngine::Input::GetInstance();
	//入力の初期化
	input->Initialize();

	//ImGuiManager
	ImGuiManager* imGuiManager = nullptr;
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize();

	//ポストエフェクト
	PostEffect* postEffect = nullptr;
	PostEffect::SetDevice(dxCommon->GetDevice());
	postEffect = new PostEffect;
	postEffect->Initialize();
	postEffect->CreateGraphicsPipeLine();

	//シーンマネージャー
	SceneManager* sceneManager = nullptr;
	sceneManager = new SceneManager();
	
	//最初のシーンの生成
	BaseScene* scene = new TitleScene();
	//シーンマネージャーに最初のシーンをセット
	sceneManager->SetNextScene(scene);

	//FPSを固定
	FPS* fps = nullptr;
	fps = new FPS;
	fps->SetFrameRate(60.0f);
	fps->FpsControlBegin();

	//描画初期化処理　ここまで

	// ゲームループ
	while (true) {

		//Windowsのメッセージ処理
		if (winApp->processMessage()) {
			//ゲームループを抜ける
			break;
		}

		fps->FpsControlBegin();

		//キー
		input->Update();

		imGuiManager->Begin();

		//更新
		sceneManager->Update();
		postEffect->Update();

		imGuiManager->End();

		//レンダーテクスチャへの描画
		//postEffect->PreDrawScene(dxCommon->GetCommandList());
		//gameScene->Draw();
		//postEffect->PostDrawScene(dxCommon->GetCommandList());
		//ポストエフェクト
		//postEffect->Draw(dxCommon->GetCommandList());

		//描画前処理
		dxCommon->PreDraw();

		//fbx描画
		sceneManager->Draw();
		//スプライト描画
		sceneManager->DrawSprite();

		//imgui描画
		imGuiManager->Draw();
		//描画後処理
		dxCommon->PostDraw();

		fps->FpsControlEnd();

		//escキーで終了
		if (input->IsKeyPress(DIK_ESCAPE)) {
			break;
		}

	}

	//imgui解放
	imGuiManager->Finalize();

	//gamescene解放
	delete sceneManager;

	//ポストエフェクト解放
	delete postEffect;

	return 0;
}
