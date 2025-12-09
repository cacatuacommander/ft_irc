
#include <string>
#include <vector>

class User;

class Channel 
{
	private:
		std::string			name;
		std::string			topic;
		std::vector<int>	users;//vettori di int perchè identifico utenti da loro fd che è unica cosa immutabile e unica
		std::vector<int>	operators;
		std::vector<int>	invites;
		//mettere banmask?
		
		bool				istopicrestricted;
		bool				isinviteonly;

		std::string			password;//e password è "" vuol dire che canale non ha password

		int					userlimit;//if userlimit is negative it means there i no userlimit for the channel

	public:

	//Channel();
	Channel(int fd, std::string channelname);
	//Channel(const Channel& other);
	//Channel& operator=(const Channel& other);
	~Channel();


	void setIsTopicRestricted(bool newistopicrestricted);
	void setIsInviteOnly(bool newisinviteonly);
	void setPass(std::string newpassword);
	void setUserLimit(int newuserlimit);
	void addToUsers(int fd);
	void addToOperators(int fd);
	void removeFromUsers(int fd);
	void removeFromInvites(int fd);
	void removeFromOperators(int fd);
	
	std::string getName() const;
	std::string getPass() const;
	std::string getTopic() const;
	int 		getUserlimit() const;
 	bool needsInvite() const;
	bool needsPass() const;
	bool isTopicResticted() const;
	bool userIsInOperators(int fd) const;
	bool userIsInChannel(int fd) const;
	bool userIsInvited(int fd) const;
	bool reachedUserLimit() const;
	bool searchFDinOperators(int fd);
	bool removeUser(int fd);
	void addUserToInviteList(int fd);
	void changeTopic(std::string new_topic);

	
	bool checkPass(std::string & password) const;
	void sendToAll(std::vector<User> & uservect, std::string message, int fd);

	size_t getOperatorsSize() const;
	size_t getUsersSize() const;

	std::string getListOfNicks(std::vector<User> & uservect) const;
};
