#include "UI_Rand_Test.h"
#include "imgui-master\imgui.h"
#include "PCG-rng\pcg_random.hpp"

UI_Rand_Test::UI_Rand_Test(bool active) : UI_Element(UI_RAND_TEST, active)
{
	num0_0 = num0_1 = num0_2 = num0_3 = num0_4 = num0_5 = num0_6 = num0_7 = num0_8 = num0_9 = num1_0 = 0;
}

UI_Rand_Test::~UI_Rand_Test()
{}

bool UI_Rand_Test::Update()
{
	ImGui::Begin("Random Numbers Generator Testing");
	if (ImGui::Button("Generate!"))
	{
		//TODO Generate

		//TODO: COUNT THE AMOUNT

	}

	float f_num = 0.0f;
	ImGui::Separator();
	for (int i = 0; i < 11; i++)
	{
		ImGui::Columns(1, "Number", true);
		{
			ImGui::Text("%s %.1f", "Amount of", f_num);
			f_num += 0.1f;
			ImGui::NextColumn();
			ImGui::Text("%i", num0_0);
		}
		ImGui::Separator();
	}

	ImGui::End();

	return true;
}