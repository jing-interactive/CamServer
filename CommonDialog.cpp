#include "./CommonDialog.h"


CComDialog::CComDialog(HWND hwnd)
{
    strcpy(_default_filter, "All Files (*.*)\0*.*\0\0");

    ZeroMemory(&_ofn, sizeof(OPENFILENAME));
	_hwnd = hwnd;

    _ofn.lStructSize                      = sizeof (OPENFILENAME) ;
    _ofn.hwndOwner                        = hwnd ;
    _ofn.hInstance                        = NULL ;
    _ofn.lpstrFilter                      = _default_filter ;
    _ofn.lpstrCustomFilter = NULL ;
    _ofn.nMaxCustFilter    = 0 ;
    _ofn.nFilterIndex      = 0 ;
    _ofn.lpstrFile         = NULL ;              // Set in Open and Close functions
    _ofn.nMaxFile                = MAX_PATH ;
    _ofn.lpstrFileTitle            = NULL ;              // Set in Open and Close functions
    _ofn.nMaxFileTitle             = MAX_PATH ;
    _ofn.lpstrInitialDir           = NULL ;
    _ofn.lpstrTitle                = NULL ;
    _ofn.Flags                    = 0 ;                         // Set in Open and Close functions
    _ofn.nFileOffset               = 0 ;
    _ofn.nFileExtension            = 0 ;
    _ofn.lpstrDefExt               = TEXT ("txt") ;
    _ofn.lCustData                 = 0L ;
    _ofn.lpfnHook                  = NULL ;
    _ofn.lpTemplateName            = NULL ;
}

CComDialog::~CComDialog()
{

}


bool CComDialog::PopFileOpenDlg (char* pstrFileName,
                                 char* szFilter, char* extension,
                                 char* pstrTitleName)
{
    _ofn.lpstrFilter = szFilter;		// Filter text
    _ofn.lpstrFile = pstrFileName;		// File name string
    _ofn.Flags = OFN_HIDEREADONLY;	// don't display "Read Only"
    _ofn.lpstrDefExt = extension;		// extension name
    _ofn.lpstrTitle = pstrTitleName; // title of dlg box

    return GetOpenFileName(&_ofn);
}


bool CComDialog::PopFileSaveDlg (char* pstrFileName,
                                 char* szFilter, char* extension)
{
    _ofn.lpstrFilter = szFilter;		// Filter text
    _ofn.lpstrFile                 = pstrFileName ;
    _ofn.Flags                     = OFN_OVERWRITEPROMPT;
    //_ofn.lpstrFileTitle            = pstrTitleName ;
    _ofn.lpstrDefExt = extension;		// extension name

    return GetSaveFileName (&_ofn) ;
}


bool CComDialog::PopFolderDlg(OUT char* path, IN char* title, IN BFFCALLBACK cb)
{ 
    BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));    

//��ʼ����ڲ���bi��ʼ****************************
    bi.hwndOwner = _hwnd;//::AfxGetMainWnd()->GetSafeHwnd();
    bi.pidlRoot = NULL;
    bi.pszDisplayName = path;//�˲�����ΪNULL������ʾ�Ի���
    bi.lpszTitle = title;
    bi.ulFlags = 0;
    bi.lpfn = cb;
/*    bi.iImage= IDR_MAINFRAME;*/
//��ʼ����ڲ���bi����*****************************

    LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//������ʾѡ��Ի���
    if (pIDList)
    {
        SHGetPathFromIDList(pIDList, path);
        //ȡ���ļ���·����Buffer��
		LPMALLOC lpMalloc;
		if (FAILED(SHGetMalloc(&lpMalloc))) 
			return false;
		//�ͷ��ڴ�
		lpMalloc->Free(pIDList);
		lpMalloc->Release();

		return true;
    }
	else
	{
		return false;
	}
}

bool CComDialog::PopDlgFromId(int id)
{
	::DialogBox( NULL, MAKEINTRESOURCE(id), _hwnd, (DLGPROC)aboutDlgProc );
	return true;
}

LRESULT CALLBACK aboutDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM  )
{
   switch( uMsg ) {
      case WM_INITDIALOG:
         return true;
      case WM_COMMAND:
         switch( wParam ) {
            case IDOK:
               EndDialog( hDlg, TRUE );
               return true;
         }
      break;
   }
   return false;
}