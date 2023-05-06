#include <wx/wx.h>
#include <wx/wrapsizer.h>
#include <wx/splitter.h>

#include <string>
#include <vector>
#include <chrono>

#include "myapp.h"

#include "colorpane.h"
#include "pensizepane.h"

#include "drawingcanvas.h"
#include "drawingdocument.h"
#include "drawingview.h"

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxDocParentFrame
{
public:
    MyFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString &title,
            const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);

    void SetupCanvasForView(DrawingView *view);

private:
    wxPanel *BuildControlsPanel(wxWindow *parent);

    void SetupColorPanes(wxWindow *parent, wxSizer *sizer);
    void SetupPenPanes(wxWindow *parent, wxSizer *sizer);

    void SelectColorPane(ColorPane *pane);
    void SelectPenPane(PenSizePane *pane);

    void BuildMenuBar();

    std::vector<ColorPane *> colorPanes{};
    std::vector<PenSizePane *> penPanes{};

    wxPanel *docPanel;

    const std::vector<std::string> niceColors = {"#000000", "#ffffff", "#fd7f6f",
                                                 "#7eb0d5", "#b2e061", "#bd7ebe",
                                                 "#ffb55a", "#ffee65", "#beb9db",
                                                 "#fdcce5", "#8bd3c7"};

    const int penCount = 6;

    const std::string lightBackground = "#f4f3f3";
    const std::string darkBackground = "#2c2828";
};

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    SetAppDisplayName("Paint App");

    docManager.reset(new wxDocManager);
    docManager->SetMaxDocsOpen(1);

    new wxDocTemplate(docManager.get(), "Drawing",
                      "*.pxz", "", "pxz", "DrawingDocument", "DrawingView",
                      CLASSINFO(DrawingDocument),
                      CLASSINFO(DrawingView));

    frame = new MyFrame(docManager.get(), nullptr, wxID_ANY, wxGetApp().GetAppDisplayName());
    frame->Show(true);
    return true;
}

ToolSettings &MyApp::GetToolSettings()
{
    return wxGetApp().toolSettings;
}

void MyApp::SetupCanvasForView(DrawingView *view)
{
    wxGetApp().frame->SetupCanvasForView(view);
}

void MyFrame::SetupCanvasForView(DrawingView *view)
{
    if (docPanel->GetChildren().size() > 0)
    {
        docPanel->GetSizer()->Clear(true);
    }

    if (view != nullptr)
    {
        auto canvas = new DrawingCanvas(docPanel, view, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        docPanel->GetSizer()->Add(canvas, 1, wxEXPAND);

        view->SetFrame(this);
    }
    else
    {
        this->SetTitle(wxGetApp().GetAppDisplayName());
    }

    docPanel->Layout();
}

void MyFrame::SetupColorPanes(wxWindow *parent, wxSizer *sizer)
{
    for (const auto &color : niceColors)
    {
        auto colorPane = new ColorPane(parent, wxID_ANY, wxColour(color));

        colorPane->Bind(wxEVT_LEFT_DOWN, [this, colorPane](wxMouseEvent &event)
                        { SelectColorPane(colorPane); });

        colorPanes.push_back(colorPane);
        sizer->Add(colorPane, 0, wxRIGHT | wxBOTTOM, FromDIP(5));
    }
}

void MyFrame::SetupPenPanes(wxWindow *parent, wxSizer *sizer)
{
    for (int i = 0; i < penCount; i++)
    {
        auto penPane = new PenSizePane(parent, wxID_ANY, i * FromDIP(4) + FromDIP(1));

        penPane->Bind(wxEVT_LEFT_DOWN, [this, penPane](wxMouseEvent &event)
                      { SelectPenPane(penPane); });

        penPanes.push_back(penPane);
        sizer->Add(penPane, 0, wxRIGHT | wxBOTTOM, FromDIP(5));
    }
}

wxPanel *MyFrame::BuildControlsPanel(wxWindow *parent)
{
    auto controlsPanel = new wxScrolled<wxPanel>(parent, wxID_ANY);
    controlsPanel->SetScrollRate(0, FromDIP(10));

    bool isDark = wxSystemSettings::GetAppearance().IsDark();
    controlsPanel->SetBackgroundColour(wxColour(isDark ? darkBackground : lightBackground));

    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto text = new wxStaticText(controlsPanel, wxID_ANY, "Colors");
    mainSizer->Add(text, 0, wxALL, FromDIP(5));

    auto colorPaneSizer = new wxWrapSizer(wxHORIZONTAL);
    SetupColorPanes(controlsPanel, colorPaneSizer);

    mainSizer->Add(colorPaneSizer, 0, wxALL, FromDIP(5));

    text = new wxStaticText(controlsPanel, wxID_ANY, "Pens");
    mainSizer->Add(text, 0, wxALL, FromDIP(5));

    auto penPaneSizer = new wxWrapSizer(wxHORIZONTAL);
    SetupPenPanes(controlsPanel, penPaneSizer);
    mainSizer->Add(penPaneSizer, 0, wxALL, FromDIP(5));

    controlsPanel->SetSizer(mainSizer);

    return controlsPanel;
}

MyFrame::MyFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString &title,
                 const wxPoint &pos, const wxSize &size)
    : wxDocParentFrame(manager, frame, id, title, pos, size)
{
    wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    splitter->SetMinimumPaneSize(FromDIP(150));

    auto controlsPanel = BuildControlsPanel(splitter);
    docPanel = new wxPanel(splitter, wxID_ANY);
    docPanel->SetSizer(new wxBoxSizer(wxVERTICAL));

    splitter->SplitVertically(controlsPanel, docPanel);
    splitter->SetSashPosition(FromDIP(220));

    this->SetSize(FromDIP(800), FromDIP(500));
    this->SetMinSize({FromDIP(400), FromDIP(200)});

    SelectColorPane(colorPanes[0]);
    SelectPenPane(penPanes[0]);

    BuildMenuBar();
}

void MyFrame::SelectColorPane(ColorPane *pane)
{
    for (auto colorPane : colorPanes)
    {
        colorPane->selected = (colorPane == pane);
        colorPane->Refresh();
    }

    MyApp::GetToolSettings().currentColor = pane->color;
}

void MyFrame::SelectPenPane(PenSizePane *pane)
{
    for (auto penPane : penPanes)
    {
        penPane->selected = (penPane == pane);
        penPane->Refresh();
    }

    MyApp::GetToolSettings().currentWidth = pane->penWidth;
}

void MyFrame::BuildMenuBar()
{
    auto menuBar = new wxMenuBar;

    auto fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW);
    fileMenu->Append(wxID_OPEN);
    fileMenu->Append(wxID_SAVE);
    fileMenu->Append(wxID_SAVEAS);
    fileMenu->Append(wxID_CLOSE);
    fileMenu->Append(wxID_EXIT);

    menuBar->Append(fileMenu, "&File");

    auto editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO);
    editMenu->Append(wxID_REDO);
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT);
    editMenu->Append(wxID_COPY);
    editMenu->Append(wxID_PASTE);
    editMenu->Append(wxID_DELETE);
    editMenu->AppendSeparator();
    editMenu->Append(wxID_SELECTALL);

    menuBar->Append(editMenu, "&Edit");

    SetMenuBar(menuBar);
}