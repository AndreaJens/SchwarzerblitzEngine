/*
	*** Schwarzerblitz 3D Fighting Game Engine  ***

	=================== Source Code ===================
	Copyright (C) 2016-2022 Andrea Demetrio

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation and/or
	   other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may be
	   used to endorse or promote products derived from  this software without specific
	   prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	=============== Additional Components ==============
	Please refer to the license/irrlicht/ and license/SFML/ folder for the license
	indications concerning those components. The irrlicht-schwarzerlicht engine and
	the SFML code and binaries are subject to their own licenses, see the relevant
	folders for more information.

	=============== Assets and resources ================
	Unless specificed otherwise in the Credits file, the assets and resources
	bundled with this engine are to be considered "all rights reserved" and
	cannot be redistributed without the owner's consent. This includes but it is
	not limited to the characters concepts / designs, the 3D models, the music,
	the sound effects, 2D and 3D illustrations, stages, icons, menu art.

	Tutorial Man, Evil Tutor, and Random:
	Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

#include<fstream>
#include"FK_BoneFileParser.h"

namespace fk_engine{

	FK_BoneFileParser::FK_BoneFileParser(){
		setupParsingMap();
	};

	FK_BoneFileParser::FK_BoneFileParser(std::string filename){
		setupParsingMap();
		outputMap.clear();
		parseBoneFile(filename);
	};

	void FK_BoneFileParser::setupParsingMap(){
		stringToBoneMap["HEAD"] = FK_Bones_Type::HeadArmature;
		stringToBoneMap["NECK"] = FK_Bones_Type::NeckArmature;
		stringToBoneMap["TORSO"] = FK_Bones_Type::TorsoArmature;
		stringToBoneMap["SPINE"] = FK_Bones_Type::SpineArmature;
		stringToBoneMap["HIPS"] = FK_Bones_Type::HipsArmature;
		stringToBoneMap["SHOULDER_L"] = FK_Bones_Type::LeftArmArmature;
		stringToBoneMap["SHOULDER_R"] = FK_Bones_Type::RightArmArmature;
		stringToBoneMap["ELBOW_L"] = FK_Bones_Type::LeftForearmArmature;
		stringToBoneMap["ELBOW_R"] = FK_Bones_Type::RightForearmArmature;
		stringToBoneMap["WRIST_L"] = FK_Bones_Type::LeftWristArmature;
		stringToBoneMap["WRIST_R"] = FK_Bones_Type::RightWristArmature;
		stringToBoneMap["LEG_L"] = FK_Bones_Type::LeftLegArmature;
		stringToBoneMap["LEG_R"] = FK_Bones_Type::RightLegArmature;
		stringToBoneMap["KNEE_L"] = FK_Bones_Type::LeftKneeArmature;
		stringToBoneMap["KNEE_R"] = FK_Bones_Type::RightKneeArmature;
		stringToBoneMap["ANKLE_L"] = FK_Bones_Type::LeftAnkleArmature;
		stringToBoneMap["ANKLE_R"] = FK_Bones_Type::RightAnkleArmature;
		stringToBoneMap["FOOT_L"] = FK_Bones_Type::LeftFootArmature;
		stringToBoneMap["FOOT_R"] = FK_Bones_Type::RightFootArmature;
		stringToBoneMap["THUMB_L"] = FK_Bones_Type::LeftThumbArmatureTip;
		stringToBoneMap["THUMB_R"] = FK_Bones_Type::RightThumbArmatureTip;
		stringToBoneMap["FINGERS_L"] = FK_Bones_Type::LeftFingersArmatureMedium;
		stringToBoneMap["FINGERS_R"] = FK_Bones_Type::RightFingersArmatureMedium;
		stringToBoneMap["EXTRA_1"] = FK_Bones_Type::AdditionalArmature01;
		stringToBoneMap["EXTRA_2"] = FK_Bones_Type::AdditionalArmature02;
		stringToBoneMap["EXTRA_3"] = FK_Bones_Type::AdditionalArmature03;
		stringToBoneMap["EXTRA_4"] = FK_Bones_Type::AdditionalArmature04;
		stringToBoneMap["EXTRA_5"] = FK_Bones_Type::AdditionalArmature05;
		stringToBoneMap["EXTRA_6"] = FK_Bones_Type::AdditionalArmature06;
		stringToBoneMap["EXTRA_7"] = FK_Bones_Type::AdditionalArmature07;
		stringToBoneMap["EXTRA_8"] = FK_Bones_Type::AdditionalArmature08;
		stringToBoneMap["EXTRA_9"] = FK_Bones_Type::AdditionalArmature09;
		stringToBoneMap["EXTRA_10"] = FK_Bones_Type::AdditionalArmature10;
	};

	const std::map<std::string, FK_Bones_Type> &FK_BoneFileParser::getParsingMap(){
		return stringToBoneMap;
	};

	void FK_BoneFileParser::parseBoneFile(std::string filename){
		std::ifstream inputFile(filename.c_str());
		while (!inputFile.eof()){
			std::string temp = std::string();
			inputFile >> temp;
			auto iterator = stringToBoneMap.find(temp);
			if (iterator != stringToBoneMap.end()) {
				inputFile >> temp;
				outputMap[iterator->second] = temp;
			}
		}
		inputFile.close();
	};
	std::map<FK_Bones_Type, std::string> FK_BoneFileParser::getBonesMap(){
		return outputMap;
	};
}