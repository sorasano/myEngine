#include <string>
#include <sstream>
#include <iomanip>
#include "GameScene.h"
#include "FPS.h"
#include "PostEffect.h"
#include "ImguiManager.h"

// ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//ポインタ
	WinApp* winApp = nullptr;
	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();
	//WindowsAPIの終了処理
	winApp->Finalize();

	// DirectX初期化処理
	//ポインタ
	DirectXCommon* dxCommon = nullptr;
	////DirectX初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	////キーボード処理
	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//ImGuiManager
	ImGuiManager* imGuiManager = nullptr;
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize(winApp, dxCommon);

	//ポストエフェクト
	PostEffect* postEffect = nullptr;
	PostEffect::SetDevice(dxCommon->GetDevice());
	postEffect = new PostEffect;
	postEffect->Initialize();
	postEffect->CreateGraphicsPipeLine();

	//ゲームシーン
	GameScene* gameScene = nullptr;
	gameScene = new GameScene();
	gameScene->Initialize(dxCommon, input);

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
		gameScene->Update();
		postEffect->Update();

		imGuiManager->End();

		//レンダーテクスチャへの描画
		postEffect->PreDrawScene(dxCommon->GetCommandList());
		gameScene->Draw();
		postEffect->PostDrawScene(dxCommon->GetCommandList());

		//描画前処理
		dxCommon->PreDraw();
		//ポストエフェクト
		//postEffect->Draw(dxCommon->GetCommandList());
		gameScene->Draw();

		//imgui描画
		imGuiManager->Draw();
		//描画後処理
		dxCommon->PostDraw();

		fps->FpsControlEnd();

	}

	//WindowsAPI解放
	delete winApp;
	winApp = nullptr;

	//DirextX解放
	delete dxCommon;

	//入力開放
	delete input;

	//imgui解放
	imGuiManager->Finalize();

	//gamescene解放
	delete gameScene;

	//fbxLoader解放
	FbxLoader::GetInstance()->Finalize();

	//コンソールへの文字出力
	OutputDebugStringA("Hello,DirectX!!\n");

	return 0;
}
