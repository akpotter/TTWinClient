/******************************************************************************* 
 *  @file      LoginWnd.cpp 2014\7\31 12:23:51 $
 *  @author    ���<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "LoginDialog.h"
#include "GlobalDefine.h"
#include "LoginOperation.h"
#include "Modules/ISysConfigModule.h"
#include "Modules/ILoginModule.h"
#include "Modules/IMiscModule.h"
#include "Modules/ISessionModule.h"
#include "Modules/IMessageModule.h"
#include "DoLoginServer.h"
#include "utility/Multilingual.h"
#include "TTLogic/ITcpClientModule.h"

DUI_BEGIN_MESSAGE_MAP(LoginDialog, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_WINDOWINIT, OnWindowInitialized)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

/******************************************************************************/

// -----------------------------------------------------------------------------
//  LoginWnd: Public, Constructor

LoginDialog::LoginDialog()
:m_ptxtTip(0)
,m_pedtUserName(0)
,m_pedtPassword(0)
,m_pBtnSysSetting(0)
,m_pChkRememberPWD(0)
{

}

// -----------------------------------------------------------------------------
//  LoginWnd: Public, Destructor

LoginDialog::~LoginDialog()
{
}

LPCTSTR LoginDialog::GetWindowClassName() const
{
	return _T("LoginDialog");
}

DuiLib::CDuiString LoginDialog::GetSkinFile()
{
	return  _T("LoginDialog\\LoginDialog.xml");
}

DuiLib::CDuiString LoginDialog::GetSkinFolder()
{
	return _T("");
}

CControlUI* LoginDialog::CreateControl(LPCTSTR pstrClass)
{
	return nullptr;
}
void LoginDialog::OnFinalMessage(HWND hWnd)
{
	DoLoginServer::getInstance()->shutdown();
	DoLoginServer::releaseInstance();
	
	__super::OnFinalMessage(hWnd);

	delete this;
}

LRESULT LoginDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_NCLBUTTONDBLCLK == uMsg)//����˫�����������
	{
		return 0;
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);;
}

void LoginDialog::OnWindowInitialized(TNotifyUI& msg)
{
	m_ptxtTip = (CTextUI*)m_PaintManager.FindControl(_T("tipText"));
	PTR_VOID(m_ptxtTip);
	m_pedtUserName = (CEditUI*)m_PaintManager.FindControl(_T("edit_name"));
	PTR_VOID(m_pedtUserName);
	m_pedtPassword = (CEditUI*)m_PaintManager.FindControl(_T("edit_pwd"));
	PTR_VOID(m_pedtPassword);
	m_pBtnLogin = (CButtonUI*)m_PaintManager.FindControl(_T("loginBtn"));
	PTR_VOID(m_pBtnLogin);
	m_pBtnSysSetting = (CButtonUI*)m_PaintManager.FindControl(_T("setting"));
	PTR_VOID(m_pBtnSysSetting);
	m_pChkRememberPWD = (CCheckBoxUI*)m_PaintManager.FindControl(_T("isRememberPWD"));
	PTR_VOID(m_pChkRememberPWD);

	_Initlize();
}

void LoginDialog::OnClick(TNotifyUI& msg)
{
	PTR_VOID(msg.pSender);
	if (msg.pSender == m_pBtnLogin)
	{
		_DoLogin();
		return;
	}
	else if (msg.pSender == m_pBtnSysSetting)
	{
		Close(IDOK);
		return;
	}
	__super::OnClick(msg);
}

void LoginDialog::_DoLogin()
{
	APP_LOG(LOG_DEBUG, TRUE, _T("LoginDialog DoLogin now"));

	m_ptxtTip->SetText(_T(""));
	CDuiString userName = m_pedtUserName->GetText();
	CDuiString password = m_pedtPassword->GetText();
	if (userName.IsEmpty())
	{
		CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_USERNAME_EMPTY"));
		m_ptxtTip->SetText(csTip);
		return;
	}
	if (password.IsEmpty())
	{
		CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_PASSWORD_EMPTY"));
		m_ptxtTip->SetText(csTip);
		return;
	}
	module::TTConfig* pCfg = module::getSysConfigModule()->getSystemConfig();
	pCfg->userName = userName;
	pCfg->password = password;
	pCfg->isRememberPWD = m_pChkRememberPWD->GetCheck();
	module::getSysConfigModule()->saveData();

	if (!DoLoginServer::getInstance()->createLink())
	{
		APP_LOG(LOG_ERROR, _T("Create login link service error"));
		return;
	}
	if (!logic::getTcpClientModule()->create())
	{
		APP_LOG(LOG_ERROR, _T("Create logic link service error"));
		return;
	}

	CString csTxt = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_BTN_DOLOGIN"));
	m_pBtnLogin->SetText(csTxt);
	m_pBtnLogin->SetEnabled(false);
	
	LoginParam param;
	param.csUserName = userName;
	param.csPassword = password;
	param.csUserName.Trim();
	param.csPassword.Trim();
	LoginOperation* pOperation = new LoginOperation(
		fastdelegate::MakeDelegate(this, &LoginDialog::OnOperationCallback),param);
	logic::GetLogic()->pushBackOperation(pOperation);
}

void LoginDialog::OnOperationCallback(std::shared_ptr<void> param)
{
	DoLoginServer::getInstance()->shutdown();
	LoginParam* pLoginParam = (LoginParam*)param.get();
	if (LOGIN_OK == pLoginParam->result)	//��½�ɹ�
	{
		Close(IDOK);

		//����ͬ����Ϣʱ��timer
		module::getSessionModule()->startSyncTimeTimer();
		module::getSessionModule()->setTime(pLoginParam->serverTime);

		//֪ͨ�������ͻ��˳�ʼ�����,��ȡ��֯�ܹ���Ϣ��Ⱥ�б�
		module::getLoginModule()->notifyLoginDone();

		//���������ȡ������Ϣ
		module::getSessionModule()->tcpGetOfflineMsgCount();

		//����ʷ��¼���ݿ�
		module::getMessageModule()->openDB();
	}
	else	//��½ʧ�ܴ���
	{
		logic::getTcpClientModule()->closeSocket();
		logic::getTcpClientModule()->shutdown();

		if (LOGIN_VERSION_TOOOLD == pLoginParam->result)
		{			
			CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_LOGIN_TOOOLD"));
			m_ptxtTip->SetText(csTip);
			APP_LOG(LOG_ERROR, _T("uname:%s,LoginDialog login for loging server faild��"), pLoginParam->csUserName);
		}
		else if (LOGIN_LOGINSVR_FAIL == pLoginParam->result)
		{
			CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_LOGIN_LOGINSVR_FAIL"));
			m_ptxtTip->SetText(csTip);
			APP_LOG(LOG_ERROR, _T("uname:%s,LoginDialog login for loging server faild��"), pLoginParam->csUserName);
		}
		else if (LOGIN_MSGSVR_FAIL == pLoginParam->result)
		{
			CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_LOGIN_MSGSVR_FAIL"));
			m_ptxtTip->SetText(csTip);
			APP_LOG(LOG_ERROR, _T("uname:%s,LoginDialog login msg server faild��"), pLoginParam->csUserName);
		}
		else
		{
			CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_LOGIN_FAIL"));
			m_ptxtTip->SetText(csTip);
			APP_LOG(LOG_ERROR, _T("uname:%s,LoginDialog login msg server faild��"), pLoginParam->csUserName);
		}
	}

	CString csTxt = util::getMultilingual()->getStringViaID(_T("STRID_LOGINDIALOG_BTN_LOGIN"));
	m_pBtnLogin->SetText(csTxt);
	m_pBtnLogin->SetEnabled(true);
}

void LoginDialog::_Initlize()
{
	module::TTConfig* pCfg = module::getSysConfigModule()->getSystemConfig();
	if (!pCfg->userName.IsEmpty())
		m_pedtUserName->SetText(pCfg->userName);


	if (pCfg->isRememberPWD)
	{
		m_pChkRememberPWD->SetCheck(true);
		if (!pCfg->password.IsEmpty())
		m_pedtPassword->SetText(pCfg->password);
		//_DoLogin();
	}
	else
		m_pChkRememberPWD->SetCheck(false);
}

BOOL LoginDialog::_CreateUsersFolder()
{
	module::IMiscModule* pModule = module::getMiscModule();

	//�˺�Ŀ¼
	if (!util::createAllDirectories(pModule->getCurAccountDir()))
	{
		APP_LOG(LOG_ERROR, TRUE, _T("_CreateUsersFolder current Account direcotry failed!"));
		return FALSE;
	}

	//������ʱĿ¼����
	util::emptyFolder(pModule->getUserTempDir());
	//��ǰ�û�����ʱ�ļ�Ŀ¼
	if (!util::createAllDirectories(pModule->getUserTempDir()))
	{
		APP_LOG(LOG_ERROR, TRUE, _T("_CreateUsersFolder users temp direcotry failed!"));
		return FALSE;
	}

	return TRUE;
}

//LRESULT LoginWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	COLORREF clTransparent = RGB(0xFF, 0xFF, 0xFF);  // ��xml�ļ���maskһ��,�����Լ���ͼƬ����
//	SetLayeredWindowAttributes(GetHWND(), clTransparent, 0, LWA_COLORKEY);
//	return __super::OnCreate(uMsg, wParam, lParam, bHandled);
//}



/******************************************************************************/