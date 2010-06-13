#include "DiscEditor.h"

#include <wx/dcclient.h>
#include <wx/brush.h>

#include "emulator.h"

t_sector* DiscEditorImpl::sectorClipboard = NULL;

DiscEditorImpl::DiscEditorImpl( wxWindow* parent )
:
DiscEditor( parent )
{
	// Get the disk info.
	FloppyImage = Emulator::getInstance()->GetFDC().GetDriveA();
	// Fill in the Overview
	wxString s;
	s << FloppyImage.tracks;
	trackCount->SetValue(s);
	s.Empty();
	s << FloppyImage.sides;
	sideCount->SetValue(s);

	// Fill in the sector editor
	lb_sectors->Clear();
	for(unsigned int s = 0;s<FloppyImage.track[0][0].sectors;s++) {
		wxString sNum;
		sNum.Printf(_("%x"),(int)FloppyImage.track[0][0].sector[s].CHRN[2]);
		lb_sectors->Append(sNum);
	}
}


void DiscEditorImpl::drawSectorExplorer( wxPaintEvent& event )
{
	wxPaintDC drawContext(explorerPanel);
	wxFont f(8,wxDEFAULT,wxNORMAL,wxNORMAL);

	drawContext.SetFont(f);
	wxColour c;
	wxBrush fill;
	wxPen p;
	wxString v;

	c.Set(128,128,128);
	p.SetColour(c);
	drawContext.SetPen(p);

	for(unsigned int side = 0; side <= FloppyImage.sides; side++) {
		for(unsigned int row = 0; row < FloppyImage.tracks; row++) {
			t_track* currentTrack = &FloppyImage.track[row][side];
			int pos = 0;
			for(unsigned int col = 0;col < currentTrack->sectors; col++)
			{
			    v.Printf("%2.2X",(unsigned char)currentTrack->sector[col].CHRN[2]);
				if(currentTrack->sector[col].size != currentTrack->sector[col].declared_size)
				{
					if(currentTrack->sector[col].flags[1] &0x40)
						c.Set(255,0,255); // Weak and Erased!
					else
						c.Set(255,0,0); // Red : weak sector
				}
				else if(currentTrack->sector[col].flags[1] &0x40)
					c.Set(0,0,255);
				else
					c.Set(255,255,255); // White : ok
				// TODO : erased sectors should have another color
				fill.SetColour(c);
				drawContext.SetBrush(fill);
				wxSize k(currentTrack->sector[col].declared_size/16,12);
				wxPoint l(pos,row*12);
				drawContext.DrawRectangle(l,k);
				drawContext.DrawText(v,l);
				pos += currentTrack->sector[col].declared_size/16;
			}
		}
	}
}


void DiscEditorImpl::setTrack( wxSpinEvent& event )
{
	lb_sectors->Clear();
	currentTrack = event.GetPosition();
	for(unsigned int s = 0;s<FloppyImage.track[event.GetPosition()][0].sectors;s++) {
		wxString sNum;
		sNum.Printf(_("%x"),(int)FloppyImage.track[event.GetPosition()][0].sector[s].CHRN[2]);
		lb_sectors->Append(sNum);
	}
}

void DiscEditorImpl::setSector( wxCommandEvent& event )
{
	// Display sector info and data
	t_sector selectedSector = FloppyImage.track[currentTrack][0].sector[event.GetInt()];
	if(selectedSector.flags[1] & 0x40)
		st_erased->SetLabel(_("erased"));
	else
		st_erased->SetLabel(_("not erased"));

	wxString txt = _("Size : ");
	txt << selectedSector.declared_size/128;
	st_size->SetLabel(txt);

	if(selectedSector.declared_size > selectedSector.size)
		st_weak->SetLabel(_("weak"));
	else if(selectedSector.declared_size < selectedSector.size)
		st_weak->SetLabel(_("missing data"));
	else
		st_weak->SetLabel(_("sane"));

	// TODO : build a proper hexdump
	txt.Clear();
	int actualSize = std::min(selectedSector.declared_size, selectedSector.size);
	if(actualSize) {
		for(int i=0; i<actualSize; i++) {
			txt << wxString::Format(_("%02x "),(int)(selectedSector.data[i]));
			if ((i+1)%16 == 0) txt.Append(_("\n"));
		}
	} else {
		txt << _("This sector is empty or invalid");
	}
	tc_sectordata->SetValue(txt);
}


void DiscEditorImpl::sectorLeftClick( wxMouseEvent& event )
{
	int itemID;
	if(itemID = lb_sectors->HitTest(event.GetPosition()))
	{
		lb_sectors->SetSelection(itemID);
	}

	DiscEditor::lb_sectorsOnContextMenu(event);
}


void DiscEditorImpl::cutSector( wxCommandEvent& event )
{
	copySector(event);
	deleteSector(event);
}


void DiscEditorImpl::copySector( wxCommandEvent& event )
{
	// Get listbox selection
	// Grab selected sector and copy it to clipboard
	int sect_id = lb_sectors->GetSelection();
	int track_id = spinTrack->GetValue();

	delete sectorClipboard;
	// TODO : handle side 1
	sectorClipboard = new t_sector(
		FloppyImage.track[track_id][0].sector[sect_id]);
}


void DiscEditorImpl::pasteSector( wxCommandEvent& event )
{
	// Copy clipboard to selected track
	int sect_id = lb_sectors->GetSelection();
	int track_id = spinTrack->GetValue();

	FloppyImage.track[track_id].sectors++;
	// dwTrackSize
	// data (realloc)
	// sector[] itself
}


void DiscEditorImpl::deleteSector( wxCommandEvent& event )
{
	// Get listbox selection
	// Remove it from the box and the track itself
}


void DiscEditorImpl::renameSector( wxCommandEvent& event )
{
	// Ask for the new ID and change it in the track
}

