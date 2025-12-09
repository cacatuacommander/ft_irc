
#include <string>
#include <sys/types.h>
#include <poll.h>

class User 
{
	private:
		std::string	nickname;
		std::string	username;
		std::string	realname;
		std::string	ip;
/* 		pollfd		pollfd; */

		bool		hasputpassword;
		int			fd;
		bool		isverified;
		std::string send_buffer;
		std::string		buffer;

	public:
		
	//User();
	User(int newFd, std::string & newip/* , struct pollfd & newpollfd */);
	//User(const User& other);
	//User& operator=(const User& other);
	~User();

	void setNickName(std::string & newnickname);
	void setUserName(std::string & newusername);
	void setRealName(std::string & newrealname);
	void setPassword();
	void setIsVerified();
	void addToSendBuffer(std::string & toAdd);
	void sendBufferErase(int pos, int n);
	void bufferErase(int pos, int n);
	void bufferAppend(char * buf, ssize_t n);

	std::string bufferSubstr(size_t pos, size_t n);
	size_t bufferFind(const char * toFind) const;
	bool bufferEmpty() const;
	std::string getBuffer() const;
	bool sendBufferEmpty() const;
	const char * sendBufferCstr() const;
	size_t sendBufferSize() const;
	std::string getNickName() const;
	std::string getUserName() const;
	std::string getRealName() const;
	std::string getIp() const;
	bool getPassword() const;
	int getFd() const;
	bool getIsVerified() const;

};
