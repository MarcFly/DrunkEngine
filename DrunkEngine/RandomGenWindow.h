#ifndef _RANDOMGEN_H_
#define _RANDOMGEN_H_

#include "Window.h"
#include "pcg-c-basic-0.9\pcg_basic.h"

class RandomGenWindow : public Window
{
public:
	RandomGenWindow();
	virtual ~RandomGenWindow();

	void Draw() override;

public:
	int min_num, max_num, rand_int;
	float rand_float;
	pcg32_random_t rng;
};

#endif
