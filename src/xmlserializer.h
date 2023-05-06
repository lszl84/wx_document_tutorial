#pragma once

#include <wx/xml/xml.h>
#include <wx/fs_zip.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

#include <memory>

#include "path.h"

struct XmlSerializer
{
    XmlSerializer()
    {
        wxFileSystem::AddHandler(new wxZipFSHandler);
    }

    wxXmlDocument SerializePaths(const std::vector<Path> &paths)
    {
        wxXmlDocument doc;

        wxXmlNode *docNode = new wxXmlNode(wxXML_ELEMENT_NODE, "PaintDocument");
        docNode->AddAttribute("version", "1.1");

        for (const auto &path : paths)
        {
            wxXmlNode *pathNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Object");
            pathNode->AddAttribute("type", "Path");
            pathNode->AddAttribute("color", path.color.GetAsString(wxC2S_HTML_SYNTAX));
            pathNode->AddAttribute("width", wxString::FromDouble(path.width));

            for (const auto &point : path.points)
            {
                wxXmlNode *pointNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Point");
                pointNode->AddAttribute("x", wxString::FromDouble(point.m_x));
                pointNode->AddAttribute("y", wxString::FromDouble(point.m_y));
                pathNode->AddChild(pointNode);
            }

            docNode->AddChild(pathNode);
        }

        doc.SetRoot(docNode);

        return doc;
    }

    std::vector<Path> DeserializePaths(const wxXmlDocument &doc)
    {
        wxXmlNode *root = doc.GetRoot();

        std::vector<Path> squiggles;

        for (wxXmlNode *node = root->GetChildren(); node; node = node->GetNext())
        {
            if (node->GetName() != "Object")
                continue;

            if (node->GetAttribute("type") == "Path")
            {
                Path squiggle;
                squiggle.color = wxColor(node->GetAttribute("color"));
                squiggle.width = wxAtof(node->GetAttribute("width"));

                for (wxXmlNode *pointNode = node->GetChildren(); pointNode; pointNode = pointNode->GetNext())
                {
                    if (pointNode->GetName() != "Point")
                        continue;

                    squiggle.points.push_back(wxPoint2DDouble(wxAtof(pointNode->GetAttribute("x")),
                                                              wxAtof(pointNode->GetAttribute("y"))));
                }

                squiggles.push_back(squiggle);
            }
        }

        return squiggles;
    }

    void CompressXml(const wxXmlDocument &doc, wxOutputStream &outStream)
    {
        wxZipOutputStream zip(outStream);

        zip.PutNextEntry("paintdocument.xml");
        doc.Save(zip);

        zip.CloseEntry();

        zip.Close();
    }

    void CompressXml(const wxXmlDocument &doc, const wxString &zipFile)
    {
        auto outStream = wxFileOutputStream(zipFile);

        CompressXml(doc, outStream);
        outStream.Close();
    }

    wxXmlDocument DecompressXml(wxInputStream &in)
    {
        wxXmlDocument doc;
        wxZipInputStream zipIn(in);
        std::unique_ptr<wxZipEntry> entry(zipIn.GetNextEntry());

        while (entry)
        {
            wxString entryName = entry->GetName();

            if (entryName == "paintdocument.xml" && zipIn.CanRead())
            {
                doc.Load(zipIn);
                zipIn.CloseEntry();

                break;
            }

            zipIn.CloseEntry();
            entry.reset(zipIn.GetNextEntry());
        }

        return doc;
    }

    wxXmlDocument DecompressXml(const wxString &in)
    {
        wxFileSystem fs;
        std::unique_ptr<wxFSFile> zip(fs.OpenFile(in + "#zip:paintdocument.xml"));

        wxXmlDocument doc;

        if (zip)
        {
            wxInputStream *in = zip->GetStream();

            if (in)
            {
                doc.Load(*in);
            }
        }

        return doc;
    }
};