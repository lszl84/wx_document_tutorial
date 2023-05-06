#pragma once

#include <wx/docview.h>

#include "drawingdocument.h"

class DrawingView : public wxView
{
public:
    bool OnCreate(wxDocument *doc, long flags) override;

    void OnDraw(wxDC *dc) override;

    void OnMouseDown(wxPoint);
    void OnMouseDrag(wxPoint);
    void OnMouseDragEnd();

    void OnClear();
    bool OnClose(bool deleteWindow = true) override;

    // Setting the Frame title
    void OnChangeFilename() override;

    DrawingDocument *GetDocument() const;

    wxDECLARE_DYNAMIC_CLASS(DrawingView);

private:
    void AddPointToCurrentSquiggle(wxPoint);
};