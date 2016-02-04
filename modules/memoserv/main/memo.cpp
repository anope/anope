//#include "memo.h"
#include "memotype.h"

/*MemoImpl::MemoImpl()
{
	//unread = receipt = false;
}*/

MemoImpl::~MemoImpl()
{
}

MemoServ::MemoInfo *MemoImpl::GetMemoInfo()
{
	return Get(&MemoType::mi);
}

void MemoImpl::SetMemoInfo(MemoServ::MemoInfo *mi)
{
	Set(&MemoType::mi, mi);
}

time_t MemoImpl::GetTime()
{
	return Get(&MemoType::time);
}

void MemoImpl::SetTime(const time_t &t)
{
	Set(&MemoType::time, t);
}

Anope::string MemoImpl::GetSender()
{
	return Get(&MemoType::sender);
}

void MemoImpl::SetSender(const Anope::string &s)
{
	Set(&MemoType::sender, s);
}

Anope::string MemoImpl::GetText()
{
	return Get(&MemoType::text);
}

void MemoImpl::SetText(const Anope::string &t)
{
	Set(&MemoType::text, t);
}

bool MemoImpl::GetUnread()
{
	return Get(&MemoType::unread);
}

void MemoImpl::SetUnread(const bool &b)
{
	Set(&MemoType::unread, b);
}

bool MemoImpl::GetReceipt()
{
	return Get(&MemoType::receipt);
}

void MemoImpl::SetReceipt(const bool &b)
{
	Set(&MemoType::receipt, b);
}

