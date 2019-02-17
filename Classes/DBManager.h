#pragma execution_character_set("utf-8")

#include "cocos2d.h"

/* 

	DB 접속 관련 매니저.
	모든 회원정보 및 로그인 관리는
	이 클래스에서 한다

	싱글턴 패턴으로 getInstance를 통해 호출해서 사용.

*/

class DBManager  
{
private:
	/* 싱글턴 패턴 */
	DBManager() {}
	static bool instanceFlag;
	static DBManager* instance;
	
public:
	/* 싱글턴 패턴 */
	static DBManager* getInstance(); 
	virtual ~DBManager(){ instanceFlag = false; };

	const static int NOTOKEN = -1;
	int checkLoginInfo(std::string id, std::string pw);


	
};
