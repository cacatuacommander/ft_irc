
#include <string>

class User 
{
	private:
		std::string	nickname;
		std::string	username;
		bool		hasputpassword;
		int			fd;
		bool		isverified;
	public:

	//User();
	User(int newFd);
	//User(const User& other);
	//User& operator=(const User& other);
	~User();

	void setNickName(std::string & newnickname);
	void setUserName(std::string & newusername);
	void setPassword();
	void setIsVerified();

	bool getIsVerified() const;
	std::string getNickName() const;
	std::string getUserName() const;
	bool getPassword() const;
	int getFd() const;

};
