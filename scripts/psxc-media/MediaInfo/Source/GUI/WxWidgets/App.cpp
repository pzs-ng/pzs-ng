// ----------------------------------------------------------------------------
#include "GUI/WxWidgets/App.h"
#include "GUI/WxWidgets/GUI_Main.h"

// ----------------------------------------------------------------------------
#if defined(__BORLANDC__) && defined(_DEBUG)
    void wxAssert(int, const wchar_t*, int, const wchar_t*, const wchar_t*) {}
    void wxAppConsole::OnAssert(const wxChar *file, int line, const wxChar *cond, const wxChar *msg)  {}
#endif //_DEBUG
IMPLEMENT_APP(App)
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
bool App::OnInit()
{
    GUI_Main *frame = new GUI_Main(wxPoint(50, 50), wxSize(450, 340));
    frame->Show(TRUE);
    return TRUE;
}


