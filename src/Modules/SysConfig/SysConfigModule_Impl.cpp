/******************************************************************************* 
 *  @file      SysConfigModule_Impl.cpp 2014\8\4 10:56:41 $
 *  @author    �쵶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "SysConfigModule_Impl.h"
#include "utility/utilCommonAPI.h"
#include "utility/utilStrCodeAPI.h"
#include "SysConfigDialog.h"

/******************************************************************************/
namespace module
{
	ISysConfigModule* getSysConfigModule()
	{
		return (ISysConfigModule*)logic::GetLogic()->getModule(MODULE_ID_SYSCONFIG);
	}
}

namespace
{
	const CString g_config = _T("config.dat");
	const CString g_flag = _T("$Teamtalk");
}

// -----------------------------------------------------------------------------
//  SysConfigModule_Impl: Public, Constructor

SysConfigModule_Impl::SysConfigModule_Impl()
:m_pSysConfigDialog(0)
{
	m_pConfig.sysBaseFlag |= module::BASE_FLAG_NOTIPWHENNEWMSG;
	m_pConfig.sysBaseFlag |= module::BASE_FLAG_NOSOUNDWHENMSG;
}

// -----------------------------------------------------------------------------
//  SysConfigModule_Impl: Public, Destructor

SysConfigModule_Impl::~SysConfigModule_Impl()
{

}

void SysConfigModule_Impl::release()
{
	delete this;
}

logic::LogicErrorCode SysConfigModule_Impl::onLoadModule()
{
	_loadData();

	return logic::LOGIC_OK;
}

logic::LogicErrorCode SysConfigModule_Impl::onUnLoadModule()
{
	_saveData();
	return logic::LOGIC_OK;
}

BOOL SysConfigModule_Impl::_loadData()
{
	CString fileName = util::getAppPath() + g_config;
	if (!util::isFileExist(fileName))
	{
		APP_LOG(LOG_ERROR,_T("_loadData system config file is not exist"));
		_saveData();
		return FALSE;
	}

	CFile file;
	try
	{
		if (FALSE == file.Open(fileName, CFile::modeRead))
		{
			APP_LOG(LOG_ERROR, 1, _T("_loadData open system config failed,%s"), fileName);
			return FALSE;
		}

		CArchive ar(&file, CArchive::load);
		_unMarshal(ar);
		ar.Close();
		file.Close();
	}
	catch(...)
	{
		APP_LOG(LOG_ERROR, 1, _T("_loadData open system config unknown exception,%s"), fileName);
		file.Close();
		::DeleteFile(fileName);
		_saveData();
		return FALSE;
	}

	return TRUE;
}

BOOL SysConfigModule_Impl::_saveData()
{
	CString fileName = util::getAppPath() + g_config;

	CFile file;
	try
	{
		if (FALSE == file.Open(fileName, CFile::modeCreate | CFile::modeWrite))
		{
			APP_LOG(LOG_ERROR, _T("_saveData open system config failed,%s"), fileName);
			return FALSE;
		}
		CArchive ar(&file, CArchive::store);
		_marshal(ar);
		ar.Close();
		file.Flush();
		file.Close();
	}
	catch(...)
	{
		APP_LOG(LOG_ERROR, 1, _T("_saveData open system config unknown exception,%s"), fileName);
		file.Close();
		return FALSE;
	}

	return TRUE;
}

void SysConfigModule_Impl::_unMarshal(CArchive& ar)
{
	CString csFlag(g_flag);
	ar >> csFlag;
	ar >> m_pConfig.version;

	ar >> m_pConfig.isRememberPWD;
	ar >> m_pConfig.userName;
	ar >> m_pConfig.password;
	ar >> m_pConfig.sysBaseFlag;
	ar >> m_pConfig.sysSoundTypeBaseFlag;
	//������demo
	//if (m_pConfig.version >= 2)
	//{
	//	ar >> m_pConfig.a2;
	//}
	//if (m_pConfig.version >= 3)
	//{
	//	ar >> m_pConfig.a3;
	//}
	//if (m_pConfig.version >= 4)
	//{
	//	ar >> m_pConfig.a4;
	//}
}

void SysConfigModule_Impl::_marshal(CArchive& ar)
{
	CString csFlag(g_flag);
	ar << csFlag;
	ar << module::SYSCONFIG_VERSIONI;

	ar << m_pConfig.isRememberPWD;
	ar << m_pConfig.userName;
	ar << m_pConfig.password;
	ar << m_pConfig.sysBaseFlag;
	ar << m_pConfig.sysSoundTypeBaseFlag;
	//������demo
	//ar << m_pConfig.a2;
	//ar << m_pConfig.a3;
	//ar << m_pConfig.a4;
}

module::TTConfig* SysConfigModule_Impl::getSystemConfig()
{
	return &m_pConfig;
}

BOOL SysConfigModule_Impl::saveData()
{
	return _saveData();
}

std::string SysConfigModule_Impl::userID() const
{
	return m_pConfig.userId;
}

CString SysConfigModule_Impl::UserID() const
{
	return m_pConfig.csUserId;
}

void SysConfigModule_Impl::showSysConfigDialog(HWND hParentWnd)
{
	if (m_pSysConfigDialog && IsWindow(m_pSysConfigDialog->GetHWND()))
	{
		return;
	}
	m_pSysConfigDialog = new SysConfigDialog();
	m_pSysConfigDialog->Create(hParentWnd, _T("SysConfigDialog")
		, UI_WNDSTYLE_FRAME | WS_THICKFRAME, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_STATICEDGE, 0, 0, 0, 0);
	m_pSysConfigDialog->CenterWindow();
}

/******************************************************************************/