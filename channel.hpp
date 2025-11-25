
#include "irc.hpp"

class Channel 
{
	private:
		std::string			name;
		std::string			topic;
		std::vector<int>	users;//vettori di int perchè identifico utenti da loro fd che è unica cosa immutabile e unica
		std::vector<int>	operators;
		std::vector<int>	invites;
		//mettere banmask?
		
		bool				isinviteonly;
		bool				onlyopsmaysettopic;
//		bool				ispasswordrequired;
		std::string			password;//e password è "" vuol dire che canale non ha password
//		bool				hasuserlimit;
		size_t				userlimit;//if uerlimit i negative it mean there i no uerlimit for the channel

	public:
		std::string			buffer;

	//Channel();
	Channel(std::string newuser, std::string channelname);
	//Channel(const Channel& other);
	//Channel& operator=(const Channel& other);
	~Channel();

/* 	void setNickName(std::string & newnickname);
	void setUserName(std::string & newusername);
	void setRealName(std::string & newrealname);
	void setPassword();
	void setIsVerified(); */
	void addToUsers(int fd);
	void removeFromInvites(int fd);

	std::string getName() const;
 	bool needsInvite() const;
	bool needsPass() const;
	bool userIsInGroup(int fd) const;
	bool userIsInvited(int fd) const;
	bool reachedUserLimit() const;
/*	std::string getIp() const;
	bool getPassword() const;
	int getFd() const;
	bool getIsVerified() const; */

};