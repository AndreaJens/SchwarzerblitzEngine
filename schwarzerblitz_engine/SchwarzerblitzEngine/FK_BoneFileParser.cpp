
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