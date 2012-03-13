#ifndef EXCEPTION_H
#define EXCEPTION_H

class Exception {
private:
	const char *m_pMessage;
public: 
	//con/destructors
	Exception() : m_pMessage("") {}
	Exception(const char *pMessage) : m_pMessage(pMessage) {} 
	~Exception() {}
	//get message
	const char * hmm() {return m_pMessage;}
};

#endif