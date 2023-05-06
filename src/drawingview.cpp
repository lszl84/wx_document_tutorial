#include <wx/graphics.h>

#include "drawingview.h"
#include "myapp.h"
#include "drawingcanvas.h"

#include <memory>

wxIMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView);

bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
    if (!wxView::OnCreate(doc, flags))
    {
        return false;
    }

    MyApp::SetupCanvasForView(this);

    return true;
}

bool DrawingView::OnClose(bool deleteWindow)
{
    if (deleteWindow)
    {
        MyApp::SetupCanvasForView(nullptr);
    }
    return wxView::OnClose(deleteWindow);
}

void DrawingView::OnChangeFilename()
{
    wxString appName = wxTheApp->GetAppDisplayName();
    wxString title;

    wxString docName = GetDocument()->GetUserReadableName();
    title = docName + (GetDocument()->IsModified() ? " - Edited" : "") + wxString(_(" - ")) + appName;

    GetFrame()->SetLabel(title);
}

void DrawingView::OnDraw(wxDC *dc)
{
    dc->SetBackground(*wxWHITE_BRUSH);
    dc->Clear();

    std::unique_ptr<wxGraphicsContext> gc{wxGraphicsContext::CreateFromUnknownDC(*dc)};

    if (gc)
    {
        auto squiggles = GetDocument()->squiggles;
        for (const auto &s : squiggles)
        {
            auto pointsVector = s.points;
            if (pointsVector.size() > 1)
            {
                gc->SetPen(wxPen(
                    s.color,
                    s.width));
                gc->StrokeLines(pointsVector.size(), pointsVector.data());
            }
        }
    }
}

void DrawingView::OnMouseDown(wxPoint pt)
{
    auto currentColor = MyApp::GetToolSettings().currentColor;
    auto currentWidth = MyApp::GetToolSettings().currentWidth;

    GetDocument()->squiggles.push_back({{}, currentColor, currentWidth});

    AddPointToCurrentSquiggle(pt);
}

void DrawingView::OnMouseDrag(wxPoint pt)
{
    AddPointToCurrentSquiggle(pt);
}

void DrawingView::OnMouseDragEnd()
{
    // Nothing to do here
}

void DrawingView::OnClear()
{
    GetDocument()->squiggles.clear();
    GetDocument()->Modify(true);
}

void DrawingView::AddPointToCurrentSquiggle(wxPoint pt)
{
    auto &currentSquiggle = GetDocument()->squiggles.back();

    currentSquiggle.points.push_back(pt);
    GetDocument()->Modify(true);
}

DrawingDocument *DrawingView::GetDocument() const
{
    return wxStaticCast(wxView::GetDocument(), DrawingDocument);
}