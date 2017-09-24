#include "UI_Rand_Test.h"
#include "imgui\imgui.h"
#include "MathGeoLib\src\Algorithm\Random\LCG.h"

int UI_Rand_Test::GetNum(int i)
{
	switch (i)
	{
	case 0:
		return num0_0;
		break;
	case 1:
		return num0_1;
		break;
	case 2:
		return num0_2;
		break;
	case 3:
		return num0_3;
		break;
	case 4:
		return num0_4;
		break;
	case 5:
		return num0_5;
		break;
	case 6:
		return num0_6;
		break;
	case 7:
		return num0_7;
		break;
	case 8:
		return num0_8;
		break;
	case 9:
		return num0_9;
		break;
	case 10:
		return num1_0;
		break;

	default:
		break;
	}
	return 0;
}

UI_Rand_Test::UI_Rand_Test(bool active) : UI_Element(UI_RAND_TEST, active)
{
	num0_0 = num0_1 = num0_2 = num0_3 = num0_4 = num0_5 = num0_6 = num0_7 = num0_8 = num0_9 = num1_0 = 0;
}

UI_Rand_Test::~UI_Rand_Test()
{}

bool UI_Rand_Test::Update()
{
	ImGui::SetNextWindowPos(ImVec2(100, 300), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_Once);

	ImGui::Begin("Random Numbers Generator Testing", &active);
	if (ImGui::Button("Generate!"))
	{
		num0_0 = num0_1 = num0_2 = num0_3 = num0_4 = num0_5 = num0_6 = num0_7 = num0_8 = num0_9 = num1_0 = 0;

		LCG lcg = LCG::LCG();

		for (int c = 0; c < 1000000; c++)
		{
			int i = lcg.Int(0, 10);
			switch (i)
			{
			case 0:
				num0_0++;
				break;
			case 1:
				num0_1++;
				break;
			case 2:
				num0_2++;
				break;
			case 3:
				num0_3++;
				break;
			case 4:
				num0_4++;
				break;
			case 5:
				num0_5++;
				break;
			case 6:
				num0_6++;
				break;
			case 7:
				num0_7++;
				break;
			case 8:
				num0_8++;
				break;
			case 9:
				num0_9++;
				break;
			case 10:
				num1_0++;
				break;

			default:
				break;
			}
		}
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
			ImGui::Text("%i", GetNum(i));
		}
		ImGui::Separator();
	}

	ImGui::End();

	return true;
}