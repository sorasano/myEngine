#include "CSVLoader.h"
#include<fstream>
#include "sstream"
#include "stdio.h"

void CSVLoader::LoadCSV(const std::string fileName)
{
	position.clear();
	scale.clear();
	rotation.clear();

	//ファイルを開く
	std::ifstream file;
	file.open(fileName);
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
			DirectX::XMFLOAT3 pos1;
			line_stream >> pos1.x;
			line_stream >> pos1.y;
			line_stream >> pos1.z;
			position.emplace_back(pos1);
		}

		if (key == "rotation")
		{
			DirectX::XMFLOAT3 rot1;
			line_stream >> rot1.x;
			line_stream >> rot1.y;
			line_stream >> rot1.z;
			rotation.emplace_back(rot1);
		}

		if (key == "scale")
		{
			DirectX::XMFLOAT3 scale1;
			line_stream >> scale1.x;
			line_stream >> scale1.y;
			line_stream >> scale1.z;
			scale.emplace_back(scale1);
		}
	}
	//ファイルを閉じる
	file.close();
}