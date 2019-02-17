#include "DBManager.h"

/* 싱글턴 패턴 */
bool DBManager::instanceFlag = false;
DBManager* DBManager::instance = nullptr;
DBManager* DBManager::getInstance() {
	if (!instance) {
		instance = new DBManager();
		instanceFlag = true;
	}
	return instance;
}

/* 로그인 정보 확인 */
int DBManager::checkLoginInfo(std::string id, std::string pw) {
	/* 
	
		넘어온 id 와 pw 정보를 이용해 내 서버 DB를 조회하고
		해당 계정의 고유 토큰값을 넘겨준다.
	
	*/

	


	/* 구현 해야함 */



	return 1;
}