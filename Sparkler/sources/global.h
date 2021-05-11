#ifndef GLOBAL_H
#define GLOBAL_H

class GlobalVariables
{
protected:
	GlobalVariables(const std::string value) : value_(value)
	{
	}

	static GlobalVariables* singleton_;

	std::string value_;

public:
	GlobalVariables(GlobalVariables another) = delete;
	void operator=(const GlobalVariables another) = delete;

	static GlobalVariables* GetInstance(const std::string& value);
};

#endif
