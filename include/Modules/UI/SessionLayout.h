 /*******************************************************************************
 *  @file      SessionLayout.h 2014\8\5 20:25:03 $
 *  @author    ���<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef SESSIONLAYOUT_6BC9730E_47F6_4BCB_936D_AC034AA10DFF_H__
#define SESSIONLAYOUT_6BC9730E_47F6_4BCB_936D_AC034AA10DFF_H__

#include "DuiLib/UIlib.h"
#include "GlobalDefine.h"
#include <memory>

/******************************************************************************/
using namespace DuiLib;

/**
 * The class <code>SessionLayout</code> 
 *
 */
struct ST_picData
{
	UINT32	nPos;
	CString strLocalPicPath;
	CString strNetPicPath;
};

class MixedMsg
{
public:
	MixedMsg();
	BOOL SetNetWorkPicPath(IN CString strLocalPicPath,IN CString strNetPicPath);
	BOOL SucceedToGetAllNetWorkPic();
	CString makeMixedLocalMSG();
	CString makeMixedNetWorkMSG();
private:
	CString _getPicMsgByAddPicTeg(IN CString picPath);

public:
	CString												m_strTEXT;//����
	std::vector<ST_picData>	m_picDataVec;//ͼƬ���������е�λ�ã�ͼƬ�ı���·����ͼƬ������·��
private:
	UINT32												m_nSetNetWorkPathSuccTime;//�ɹ���õ�ͼƬ�Ĵ���
};

class MessageEntity;
class EmotionDialog;
class UIIMEdit;
class SessionLayout :public CHorizontalLayoutUI, public INotifyUI, public CWebBrowserEventHandler
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
	SessionLayout(const std::string& sId, CPaintManagerUI& paint_manager);
    /**
     * Destructor
     */
    virtual ~SessionLayout();
    //@}
public:
	virtual void DoInit();
	virtual void DoEvent(TEventUI& event);
	virtual void Notify(TNotifyUI& msg);
	void SendMsg();
	void UpdateRunTimeMsg();
	void UpdateSendMsgKey();
	void UpdateBottomLayout();
	void freshGroupMemberAvatar(IN const std::string& sID);		//ˢ��Ⱥ��Ա������״̬
	void freshAllGroupMemberAvatar();
	void onCallbackOperation(std::shared_ptr<void> param);//����ͼƬ����

	void DocmentComplete(IDispatch *pDisp, VARIANT *&url);//�򿪻Ự�Ѿ������������ʼ����ɣ�����δ����Ϣ
	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
	virtual void NewWindow2(VARIANT_BOOL *&Cancel, BSTR bstrUrl);
private:
	CString _getGetContent();	//��ȡ����
	BOOL	_displayMsgToIE(IN MessageEntity msg, IN CString jsInterface);
	void	_updateGroupMembersList();//չʾȺ��Ա

	//����png��gif
	//int	GetObjectPos(CRichEditUI* pRichEdit);
	//bool	InsertAniSmiley(CRichEditUI* pRichEdit, long hwndHostWindow, BSTR bstrFileName, OLE_COLOR clr, INT cy);
	//HRESULT GetNewStorage(LPSTORAGE* ppStg);
	//void	_InsertPic(IN CString strFilePath);
	//void	GetObjectInfo(IRichEditOle *pIRichEditOle);
	//void	ReleaseAllGif();

	void	_AddGroupMemberToList(IN const std::string& sID);
private:
	void	OnWindowInitialized(TNotifyUI& msg);
private:
	BOOL	_displayUnreadMsg();
	void	_displayHistoryMsg(UInt32 nMsgCount);

	void	_sendReadAck(IN MessageEntity msg);	//�����Ѷ�ȷ��
	
private:
	CPaintManagerUI&	m_paint_manager;

	CWebBrowserUI*		m_pWebBrowser;//������ʾ��
	UIIMEdit*			m_pInputRichEdit;

	CContainerUI*		m_pRightContainer;
	CListUI*			m_pSearchResultList;
	CListUI*			m_pGroupMemberList;

	CTextUI*			m_pSendDescription;	// ctrl+enter /enter

	CButtonUI*			m_pBtnSendMsg;
	CButtonUI*			m_pBtnClose;

	COptionUI*			m_pBtnEmotion;
	CButtonUI*			m_pBtnSendImage;
	COptionUI*			m_pBtnscreenshot;
	CButtonUI*			m_pBtnjisuanqi;
	CButtonUI*			m_pBtnshock;
	CButtonUI*			m_pBtnsendfile;
	CButtonUI*			m_pBtnadduser;//����������Ա

	CHorizontalLayoutUI*	m_bottomLayout;

	std::string			m_sId;		//�ỰId

	std::vector<MixedMsg>	m_SendingMixedMSGList;

	BOOL				m_bGroupSession;//��Ϊ����״̬�ж��ã�Ⱥ�ǲ���Ҫ��״̬��
};
/******************************************************************************/
#endif// SESSIONLAYOUT_6BC9730E_47F6_4BCB_936D_AC034AA10DFF_H__
