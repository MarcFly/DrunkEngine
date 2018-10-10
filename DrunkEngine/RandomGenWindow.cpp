#include "RandomGenWindow.h"
#include <stdio.h>
#include <time.h>


RandomGenWindow::RandomGenWindow() : Window("Random Generator")
{
	min_num = -500;
	max_num = 500;
	rand_float = 0.0f;
	rand_int = 0;
}

RandomGenWindow::~RandomGenWindow()
{
}

void RandomGenWindow::Draw()
{
	pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, 5);

	ImGui::Begin(GetName().c_str(), &active);
	{
		//Float
		ImGui::Text("Random float number between 0.0 and 1.0");
		if (ImGui::Button("Get Float"))
		{
			rand_float = ldexp(pcg32_random_r(&rng), -32);
		}
		ImGui::Text("Result: %f", rand_float);
		ImGui::Separator();

		//Int
		ImGui::Text("Random int number between two fixed numbers");
		ImGui::SliderInt("Min", &min_num, -1000, 0);
		ImGui::SliderInt("Max", &max_num, 0, 1000);
		if (ImGui::Button("Get Int"))
		{
			if (max_num == 0 && min_num == 0)
				rand_int = 0;
			else
				rand_int = (int)pcg32_boundedrand_r(&rng, (max_num - min_num)) + min_num;
		}
		ImGui::Text("Result: %i", rand_int);
	}
	ImGui::End();
}
