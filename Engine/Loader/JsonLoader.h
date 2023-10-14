/**
* @file JsonLoader.h
* @brief jsonから情報を読み取る
*/

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
	/**
	* jsonファイル読み込み
	*
	* @param[in] filename ファイルの名前
	* @return LevelData 読み込んだデータ
	*/
	static LevelData* LoadFile(const std::string filename);

public:

	//jsonファイル格納ルートパス
	static const std::string kDefaultbaseDirectory_;

	static const std::string kExtension;

};

