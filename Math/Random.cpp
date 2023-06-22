#include "Random.h"
#include<random>
#include<time.h>
#include<cmath>
using namespace std;

float Random(float num1, float num2) {

	float min, max;

	min = num2;
	max = num1;

	if (num1 < num2) {
		min = num1;
		max = num2;
	}



	//�����V�[�h������
	std::random_device seedGem;
	//�����Z���k�E�c�C�X�^�[�̗����G���W��
	std::mt19937_64 engine(seedGem());
	//�����͈͂̎w��
	std::uniform_real_distribution<float> dist(min, max);

	return dist(engine);
}
