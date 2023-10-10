#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

// レベルデータ
struct LevelData {

	struct ObjectData {
		// 平行移動
		DirectX::XMVECTOR translation;
		// 回転角
		DirectX::XMVECTOR rotation_;
		// スケーリング
		DirectX::XMVECTOR scaling;
		// ファイル名
		std::string fileName;
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
	static const std::string kDefaultbaseDirectory_;

	static const std::string kExtension;

};

