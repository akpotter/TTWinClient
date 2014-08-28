/******************************************************************************* 
 *  @file      SearchLayout.cpp 2014\8\11 16:31:05 $
 *  @author    ���<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "Modules/UI/SearchLayout.h"
#include "Modules/IUserListModule.h"
#include "Modules/IGroupListModule.h"
#include "Modules/UI/UIGroupsTreelist.h"
#include "Modules/ISessionModule.h"

/******************************************************************************/

// -----------------------------------------------------------------------------
//  SearchLayout: Public, Constructor

SearchLayout::SearchLayout()
:m_btnSearch(nullptr)
, m_edit(nullptr)
, m_SearchResultList(nullptr)
{

}

// -----------------------------------------------------------------------------
//  SearchLayout: Public, Destructor

SearchLayout::~SearchLayout()
{
	m_pManager->RemoveNotifier(this);
}

LPVOID SearchLayout::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, _T("SearchLayout")) == 0) return static_cast<SearchLayout*>(this);
	return __super::GetInterface(pstrName);
}

LPCTSTR SearchLayout::GetClass() const
{
	return _T("SearchLayout");
}

void SearchLayout::DoInit()
{
	m_pManager->AddNotifier(this);

	m_btnSearch = static_cast<CButtonUI*>(m_pManager->FindSubControlByName(this, _T("searchbtn")));
	m_edit = static_cast<CEditUI*>(m_pManager->FindSubControlByName(this, _T("editSearch")));
	m_SearchResultList = static_cast<CGroupsTreelistUI*>(m_pManager->FindSubControlByName(this, _T("searchResultList")));

}

void SearchLayout::Notify(TNotifyUI& msg)
{
	if (msg.pSender == m_edit)
	{
		if (msg.sType == DUI_MSGTYPE_TEXTCHANGED && m_SearchResultList)
		{
			m_SearchResultList->RemoveAll();
			CDuiString inputText = m_edit->GetText();
			if (inputText.IsEmpty())
			{
				SetFixedHeight(30);
				m_SearchResultList->SetVisible(false);
			}
			else
			{
				module::UserInfoEntityVec userList;
				module::getUserListModule()->getSearchUserNameListByShortName(inputText.GetData(), userList);
				_updateSearchResultList(userList,1);

				module::GroupVec gidList;
				module::getGroupListModule()->GetSearchGroupNameListByShortName(inputText.GetData(), gidList);
				_updateSearchResultList(gidList,2);
				if (0 != GetHeight())
				{
					m_SearchResultList->SetVisible(true);
					SetFixedHeight(0);
				}
				m_SearchResultList->ExpandAll();
			}
			//NeedParentUpdate();
		}
	}
	else if (msg.pSender == m_btnSearch && msg.sType == DUI_MSGTYPE_CLICK)
	{
		APP_LOG(LOG_DEBUG, _T("������ť��Ϣ"));
		module::getGroupListModule()->onCreateDiscussionGrpDialog();
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE
		&&m_SearchResultList
		&& -1 != m_SearchResultList->GetItemIndex(msg.pSender)
		&& 0 == _tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")))
	{
		Node* node = (Node*)msg.pSender->GetTag();
		if (!m_SearchResultList->CanExpand(node)&& node)
		{
			std::string sid = util::cStringToString(CString(node->data().sId));
			logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_OPENNEWSESSION, sid);//֪ͨ�����ڴ����Ự
			//m_pManager->SendNotify(this, IM_MSG_DBCLICK_OPENNEWSESSION, WPARAM(sid.c_str()));//֪ͨ�����ڴ����Ự
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK
		&&m_SearchResultList
		&& -1 != m_SearchResultList->GetItemIndex(msg.pSender)
		&& 0 == _tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")))
	{
		Node* node = (Node*)msg.pSender->GetTag();

		if (m_SearchResultList->CanExpand(node))
		{
			m_SearchResultList->SetChildVisible(node, !node->data().child_visible_);
		}
	}
}

void SearchLayout::_updateSearchResultList(IN const std::vector<std::string>& nameList, int IdType)
{
	if (nameList.empty())
	{
		return;
	}
	GroupsListItemInfo item;

	item.folder = true;
	item.empty = false;
	item.description = _T("�������1");
	if (1 == IdType)
	{
		item.nickName = _T("�ҵĺ���");
	}
	else
	{
		item.nickName = _T("�ҵ�Ⱥ");
	}
	Node* root_parent = m_SearchResultList->AddNode(item, NULL);

	item.folder = false;
	for (std::string sid : nameList)
	{
		if (1 == IdType)
		{
			module::UserInfoEntity userInfo;
			module::getUserListModule()->getUserInfoBySId(sid, userInfo);
			item.id = util::stringToCString(userInfo.sId);
			item.nickName = userInfo.getRealName();
			item.avatarPath = util::stringToCString(userInfo.getAvatarPath());
		}
		else if (2 == IdType)
		{
			module::GroupInfoEntity groupInfo;
			module::getGroupListModule()->getGroupInfoBySId(sid, groupInfo);
			item.id = util::stringToCString(groupInfo.gId);
			item.nickName = groupInfo.csName;
			item.avatarPath = util::stringToCString(groupInfo.getAvatarPath());
		}
		m_SearchResultList->AddNode(item, root_parent);
	}
}


/******************************************************************************/