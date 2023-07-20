#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

// レベルデータ
struct LevelData {

	struct ObjectData {
		// ファイル名
		std::string fileName;
		// 平行移動
		DirectX::XMVECTOR translation;
		// 回転角
		DirectX::XMVECTOR rotation;
		// スケーリング
		DirectX::XMVECTOR scaling;
	};

	// オブジェクト配列
	std::vector<ObjectData> objects;
};

class JsonLoader
{

public:

	static LevelData* LoadFile(const std::string filename);

public:

	//jsonファイル格納ルートパス
	static const std::string kDefaultbaseDirectory;

	static const std::string kExtension;

};

