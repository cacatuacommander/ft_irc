
#include <string>

class User 
{
	private:
		std::string	nickname;
		std::string	username;
		std::string	realname;
		std::string	ip;
		bool		hasputpassword;
		int			fd;
		bool		isverified;

	public:
		std::string		buffer;
	//User();
	User(int newFd, std::string newip);
	//User(const User& other);
	//User& operator=(const User& other);
	~User();

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
