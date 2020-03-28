#ifndef FK_BONEPARSER_CLASS
#define FK_BONEPARSER_CLASS

#include<string>
#include<map>
#include"FK_Database.h"

namespace fk_engine{
	class FK_BoneFileParser{
	public: 
		FK_BoneFileParser();
		FK_BoneFileParser(std::string filename);
		std::map<FK_Bones_Type, std::string> getBonesMap();
		const std::map<std::string, FK_Bones_Type>& getParsingMap();
	protected:
		void setupParsingMap();
		void parseBoneFile(std::string filename);
	private:
		std::map<std::string, FK_Bones_Type> stringToBoneMap;
		std::map<FK_Bones_Type, std::string> outputMap;
	};
}

#endif