
#include "irc.hpp"

class Channel 
{
	private:
		std::vector<std::string>	users;
		std::vector<std::string>	operators;
		std::vector<std::string>	invites;
		std::string					topic;
		//mettere banmask?
		
		bool						isinviteonly;
		bool						onlyopsmaysettopic;
		int							ispasswordrequired;
		std::string					password;
		bool						hasuserlimit;
		std::string					userlimit;

	public:
		std::string			buffer;
	//Channel();
	Channel( std::string newuser, std::string newip);
	//Channel(const Channel& other);
	//Channel& operator=(const Channel& other);
	~Channel();

	void setNickName(std::string & newnickname);
	void setUserName(std::string & newusername);
	void setRealName(std::string & newrealname);
	void setPassword();
	void setIsVerified();

	std::string getNickName() const;
	std::string getUserName() const;
	std::string getRealName() const;
	std::string getIp() const;
	bool getPassword() const;
	int getFd() const;
	bool getIsVerified() const;

};