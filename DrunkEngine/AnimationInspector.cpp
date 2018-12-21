#include "AnimationInspector.h"
#include "ComponentAnimation.h"
#include "ResourceAnimation.h"
#include "ResourceSkeleton.h"

AnimationInspectorWin::AnimationInspectorWin() : Window("Animation Inspector")
{
	anim = nullptr;
}

AnimationInspectorWin::~AnimationInspectorWin()
{
	anim = nullptr;
}

void AnimationInspectorWin::Draw()
{
	if (active)
	{
		ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImGui::SliderInt("Zoom", &zoom, 15, 100);

			//Animation typeos of Keys
			ImGui::BeginGroup();
			{
				ImGui::SetCursorPosY(85);

				ImGui::Text("Position");

				ImGui::SetCursorPosY(125);

				ImGui::Text("Rotation");

				ImGui::SetCursorPosY(165);

				ImGui::Text("Scale");
				
			}
			ImGui::EndGroup();

			ImGui::SameLine();

			//Animation TimeLine
			ImGui::BeginChild("TimeLine", ImVec2(winSize, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				ImVec2 p = ImGui::GetCursorScreenPos();
				ImVec2 anim_bar = ImGui::GetCursorScreenPos();
				ImGui::InvisibleButton("scrollbar", {(float)anim->duration*zoom ,140 });
				ImGui::SetCursorScreenPos(p);

				for (int i = 0; i < anim->duration; i++)
				{
					ImGui::BeginGroup();
					{
						ImGui::GetWindowDrawList()->AddLine({ p.x,p.y }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

						char frame[8];
						sprintf(frame, "%01d", i);
						ImVec2 aux = { p.x + 3,p.y };
						ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

						if (anim != nullptr && sel_bone_anim != nullptr)
						{
							for (int j = 0; j < sel_bone_anim->num_translation_keys; ++j)
							{
								if (sel_bone_anim->TranslationKeys[j].time == i)
								{
									ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 35), 6.0f, ImColor(0.0f, 0.5f, 1.0f, 0.5f));
								}
							}
							for (int j = 0; j < sel_bone_anim->num_rotation_keys; ++j)
							{
								if (sel_bone_anim->RotationKeys[j].time == i)
								{
									ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 75), 6.0f, ImColor(0.0f, 1.0f, 0.0f, 0.5f));
								}
							}
							for (int j = 0; j < sel_bone_anim->num_scaling_keys; ++j)
							{
								if (sel_bone_anim->ScalingKeys[j].time == i)
								{
									ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 115), 6.0f, ImColor(1.0f, 0.5f, 0.0f, 0.5f));
								}
							}
						}

						p = { p.x + zoom,p.y };
					}
					ImGui::EndGroup();

					ImGui::SameLine();

				}

				//RedLine 

				ImGui::GetWindowDrawList()->AddLine({ anim_bar.x + progress,anim_bar.y - 10 }, ImVec2(anim_bar.x + progress, anim_bar.y + 165), IM_COL32(255, 255, 0, 120), (float)zoom);

				progress = zoom * anim->timer;
				scrolled = false;

				float framesInWindow = winSize / zoom;

				if (progress != 0 && fmod(progress, winSize) == 0 && !scrolled)
				{
					float scrollPos = ImGui::GetScrollX();
					ImGui::SetScrollX(scrollPos + winSize);
					scrolled = true;
				}

			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginGroup();

			ImGui::BeginChild("All Animations", ImVec2(250, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

			if (anim != nullptr)
			{
				for (int i = 0; i < anim->r_anim->channels.size(); i++)
				{
					if (ImGui::Button(anim->r_anim->channels[i]->bone_name.c_str()))
					{
						sel_bone_anim = anim->r_anim->channels[i];
					}
				}
			}

			ImGui::EndChild();

			//ImGui::SameLine();

			ImGui::BeginChild("Selected Bone", ImVec2(250, 30), true);

			if (sel_bone_anim != nullptr)
			{
				ImGui::Text(sel_bone_anim->bone_name.c_str());
			}

			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::NewLine();
		}
		ImGui::End();
	}
}