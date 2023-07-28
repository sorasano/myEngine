#include "CSVLoader.h"

#include<fstream>
#include "sstream"
#include "stdio.h"
#include <string>

const std::string CSVLoader::kDefaultbaseDirectory = "Resources/csv/";
const std::string CSVLoader::kExtension = ".csv";

void CSVLoader::LoadCSV(const std::string fileName)
{
	position_.clear();
	scale_.clear();
	rotation_.clear();
	type_.clear();

	//連結してフルパスを得る
	const std::string fullpath = kDefaultbaseDirectory + fileName + kExtension;

	//ファイルを開く
	std::ifstream file;
	file.open(fullpath);
	assert(!file.fail());

	std::string line;

	//ファイルから障害物の場所を読み込み
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string key;
		getline(line_stream, key, ' ');


		if (key == "position")
		{
			DirectX::XMFLOAT3 pos;
			line_stream >> pos.x;
			line_stream >> pos.y;
			line_stream >> pos.z;
			this->position_.emplace_back(pos);
		}
		
		if (key == "type") {
			int type;
			line_stream >> type;
			this->type_.emplace_back(type);
		}

		if (key == "stopInScreen") {
			bool stopInScreen;
			line_stream >> stopInScreen;
			this->stopInScreen_.emplace_back(stopInScreen);
		}
	}
	//ファイルを閉じる
	file.close();
}