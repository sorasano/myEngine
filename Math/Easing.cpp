#include "Easing.h"
#include<random>
#include<time.h>
#include<cmath>
using namespace std;

DirectX::XMFLOAT2 EaseIn(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, float timer)
{
	float nowTime;

	//�����ɃC�[�W���O�̎�������
	nowTime = pow(timer, 3);

	DirectX::XMFLOAT2 resultVec;
	resultVec.x = start.x * (1.0f - nowTime) + end.x * nowTime;
	resultVec.y = start.y * (1.0f - nowTime) + end.y * nowTime;

	return resultVec;
}

DirectX::XMFLOAT2 EaseOut(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, float timer)
{
	float nowTime;

	//�����ɃC�[�W���O�̎�������
	nowTime = 1 - pow(1 - timer, 3);

	DirectX::XMFLOAT2 resultVec;
	resultVec.x = start.x * (1.0f - nowTime) + end.x * nowTime;
	resultVec.y = start.y * (1.0f - nowTime) + end.y * nowTime;

	return resultVec;
}

void Easing::Update()
{
	//���ݎ������擾
	nowCount = clock();
	//�擾���ꂽ�C�[�W���O�J�n���ԂŏI�����Ԃ����܂�
	endCount = nowCount - startCount;
	//�^�C�}�[�̐i�݋��float�^�ɕϊ�
	endTimer = static_cast<float>(endCount) / 1'000.0f;
	//(�^�C�}�[�̐i�s�x/�ő厞��)�ŃC�[�W���O�̐i�݋���v�Z
	timeRate = min(endTimer / maxTime, 1.0f);
	//timeRate = endTimer / maxTime;
}

void Easing::Start(float animationSecond)
{
	startCount = clock();
	maxTime = animationSecond;
}