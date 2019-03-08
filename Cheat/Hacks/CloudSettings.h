#pragma once
#include <string>
#include <fstream>
#include <vector>

class ICloudSets
{
public:
	struct cfg_info_s
	{
		std::string name;
		std::string hash;
		std::string id;
		bool added = false;
	};

	virtual std::vector<cfg_info_s> UpdateList(std::string _user) = 0;
	virtual bool Delete(std::string _user, std::string _id, std::string _filename) = 0;
	virtual void Save(std::string _userdata, std::string _filename, std::string _settings) = 0;
	virtual std::string Load(std::string _hash, std::string &_name) = 0;
};

class CCloudSets : public ICloudSets
{
public:

	virtual std::vector<cfg_info_s> UpdateList(std::string _user);
	virtual bool Delete(std::string _user, std::string _id, std::string _filename);
	virtual void Save(std::string _userdata, std::string _filename, std::string _settings);
	virtual std::string Load(std::string _hash, std::string &_name);

	static CCloudSets& CCloudGet()
	{
		static CCloudSets obj;
		return obj;
	}

};