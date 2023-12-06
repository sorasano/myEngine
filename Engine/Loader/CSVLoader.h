/**
* @file CSVLoader.h
* @brief CSVから情報を読み取る
*/

#pragma once
#include "array"
#include "DirectXMath.h"
#include "vector"
#include "string"

class CSVLoader
{
private:	//エイリアス
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	using string = std::string;	//std::を省略
public:
	/**
	* csvファイルを読み込んで変数に代入する
	*
	* @param[in] fileName ファイルの名前
	*/
	void LoadCSV(const std::string fileName);

	//ゲッター
	/**
	* @return XMFLOAT3 position_取得
	*/
	XMFLOAT3 GetPosition(int num) const { return position_[num]; }
	/**
	* @return XMFLOAT3 rotation_取得
	*/
	XMFLOAT3 GetRotation(int num) const { return rotation_[num]; }
	/**
	* @return XMFLOAT3 scale_取得
	*/
	XMFLOAT3 GetScale(int num) const { return scale_[num]; }

	/**
	* @return int type_取得
	*/
	int GetType(int num) const { return type_[num]; }
	/**
	* @return bool stopInScreen_取得
	*/
	bool GetStopInScreen(int num) const { return stopInScreen_[num]; }
	//読み込んだオブジェクトの数
	/**
	* @return int position_取得
	*/
	int GetSize() const { return static_cast<int>(position_.size()); }

private:

	std::vector<XMFLOAT3> position_;
	std::vector<XMFLOAT3> rotation_;
	std::vector<XMFLOAT3> scale_;

	std::vector<int> type_;
	std::vector<bool> stopInScreen_;

	//ファイル格納ルートパス
	static const std::string kDefaultbaseDirectory_;

	static const std::string kExtension;
};