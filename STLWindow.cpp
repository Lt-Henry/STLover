/*  STLover - Simple STL Viewer
 *  Copyright (C) 2020 Gerasim Troeglazov <3dEyes@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "STLApp.h"
#include "STLView.h"
#include "STLWindow.h"
#include "STLStatWindow.h"
#include "STLInputWindow.h"
#include "STLRepairWindow.h"
#include "STLToolBar.h"

STLWindow::STLWindow(BRect frame)
	: BWindow(frame, MAIN_WIN_TITLE, B_TITLED_WINDOW, 0),
	fOpenFilePanel(NULL),
	fSaveFilePanel(NULL),
	stlModified(false),
	showWireframe(false),
	showBoundingBox(false),
	showAxes(false),
	showOXY(false),
	exactFlag(false),
	nearbyFlag(false),
	removeUnconnectedFlag(false),
	fillHolesFlag(false),
	normalDirectionsFlag(false),
	normalValuesFlag(false),
	reverseAllFlag(false),
	iterationsValue(2),
	statWindow(NULL),
	stlValid(false),
	stlLoading(false),
	stlObject(NULL),
	stlObjectView(NULL),
	stlObjectAppend(NULL),
	errorTimeCounter(0),
	isRenderWork(true),
	zDepth(-5),
	maxExtent(10)
{
	fMenuBar = new BMenuBar(BRect(0, 0, Bounds().Width(), 22), "menubar");
	fMenuFile = new BMenu("File");
	fMenuFileSaveAs = new BMenu("Save as" B_UTF8_ELLIPSIS);
	fMenuView = new BMenu("View");
	fMenuTools = new BMenu("Tools");
	fMenuToolsMirror = new BMenu("Mirror");
	fMenuToolsScale = new BMenu("Scale");
	fMenuToolsMove = new BMenu("Move");
	fMenuHelp = new BMenu("Help");

	fMenuFileSaveAs->AddItem(new BMenuItem("STL (ASCII)", new BMessage(MSG_FILE_EXPORT_STLA)));
	fMenuFileSaveAs->AddItem(new BMenuItem("STL (Binary)", new BMessage(MSG_FILE_EXPORT_STLB)));
	fMenuFileSaveAs->AddItem(new BMenuItem("Geomview OFF", new BMessage(MSG_FILE_EXPORT_OFF)));
	fMenuFileSaveAs->AddItem(new BMenuItem("Autodesk DXF", new BMessage(MSG_FILE_EXPORT_DXF)));
	fMenuFileSaveAs->AddItem(new BMenuItem("Wavefront OBJ", new BMessage(MSG_FILE_EXPORT_OBJ)));
	fMenuFileSaveAs->AddItem(new BMenuItem("VRML", new BMessage(MSG_FILE_EXPORT_VRML)));

	fMenuFile->AddItem(new BMenuItem("Open" B_UTF8_ELLIPSIS, new BMessage(MSG_FILE_OPEN), 'O'));
	fMenuItemReload = new BMenuItem("Reload", new BMessage(MSG_FILE_RELOAD));
	fMenuFile->AddItem(fMenuItemReload);
//	fMenuItemAppend = new BMenuItem("Append...", new BMessage(MSG_FILE_APPEND));
//	fMenuFile->AddItem(fMenuItemAppend);
	fMenuFile->AddSeparatorItem();
	fMenuItemSave = new BMenuItem("Save", new BMessage(MSG_FILE_SAVE), 'S');
	fMenuFile->AddItem(fMenuItemSave);
	fMenuFile->AddItem(fMenuFileSaveAs);
	fMenuFileSaveAs->SetTargetForItems(this);
	fMenuFile->AddSeparatorItem();
	fMenuItemClose = new BMenuItem("Close", new BMessage(MSG_FILE_CLOSE));
	fMenuFile->AddItem(fMenuItemClose);
	fMenuFile->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
	fMenuBar->AddItem(fMenuFile);
	fMenuFile->SetTargetForItems(this);

	fMenuItemSolid = new BMenuItem("Solid", new BMessage(MSG_VIEWMODE_SOLID));
	fMenuView->AddItem(fMenuItemSolid);
	fMenuItemWireframe = new BMenuItem("Wireframe", new BMessage(MSG_VIEWMODE_WIREFRAME));
	fMenuView->AddItem(fMenuItemWireframe);
	fMenuView->AddSeparatorItem();
	fMenuItemShowAxes = new BMenuItem("Axes", new BMessage(MSG_VIEWMODE_AXES));
	fMenuView->AddItem(fMenuItemShowAxes);
	fMenuItemShowOXY = new BMenuItem("Plane OXY", new BMessage(MSG_VIEWMODE_OXY));
	fMenuView->AddItem(fMenuItemShowOXY);
	fMenuItemShowBox = new BMenuItem("Bounding box", new BMessage(MSG_VIEWMODE_BOUNDING_BOX));
	fMenuView->AddItem(fMenuItemShowBox);
	fMenuView->AddSeparatorItem();
	fMenuItemStatWin = new BMenuItem("Show statistics", new BMessage(MSG_VIEWMODE_STAT_WINDOW), 'I');
	fMenuView->AddItem(fMenuItemStatWin);
	fMenuView->AddSeparatorItem();
	fMenuItemReset = new BMenuItem("Reset", new BMessage(MSG_VIEWMODE_RESETPOS), 'R');
	fMenuView->AddItem(fMenuItemReset);

	fMenuToolsMirror->AddItem(new BMenuItem("Mirror XY", new BMessage(MSG_TOOLS_MIRROR_XY)));
	fMenuToolsMirror->AddItem(new BMenuItem("Mirror YZ", new BMessage(MSG_TOOLS_MIRROR_YZ)));
	fMenuToolsMirror->AddItem(new BMenuItem("Mirror XZ", new BMessage(MSG_TOOLS_MIRROR_XZ)));
	fMenuToolsMirror->SetTargetForItems(this);

	fMenuToolsScale->AddItem(new BMenuItem("Scale" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_SCALE)));
	fMenuToolsScale->AddItem(new BMenuItem("Axis scaling" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_SCALE_3)));
	fMenuToolsScale->SetTargetForItems(this);
	
	fMenuToolsMove->AddItem(new BMenuItem("To" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_MOVE_TO)));
	fMenuToolsMove->AddItem(new BMenuItem("By" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_MOVE_BY)));
	fMenuToolsMove->AddItem(new BMenuItem("To Center", new BMessage(MSG_TOOLS_MOVE_CENTER)));
	fMenuToolsMove->AddItem(new BMenuItem("To (0,0,0)", new BMessage(MSG_TOOLS_MOVE_ZERO)));
	fMenuToolsMove->AddItem(new BMenuItem("On the Middle", new BMessage(MSG_TOOLS_MOVE_MIDDLE)));
	fMenuToolsMove->SetTargetForItems(this);

	fMenuItemEditTitle = new BMenuItem("Edit title" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_EDIT_TITLE));
	fMenuTools->AddItem(fMenuItemEditTitle);
	fMenuTools->AddSeparatorItem();
	fMenuTools->AddItem(fMenuToolsScale);
	fMenuTools->AddItem(fMenuToolsMove);
	fMenuTools->AddItem(fMenuToolsMirror);
	fMenuItemRotate = new BMenuItem("Rotate" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_ROTATE));
	fMenuTools->AddItem(fMenuItemRotate);
	fMenuTools->AddSeparatorItem();
	fMenuItemRepair = new BMenuItem("Repair" B_UTF8_ELLIPSIS, new BMessage(MSG_TOOLS_REPAIR));
	fMenuTools->AddItem(fMenuItemRepair);

	fMenuBar->AddItem(fMenuView);
	fMenuView->SetTargetForItems(this);

	fMenuBar->AddItem(fMenuTools);
	fMenuTools->SetTargetForItems(this);

	fMenuHelp->AddItem(new BMenuItem("About", new BMessage(B_ABOUT_REQUESTED)));
	fMenuBar->AddItem(fMenuHelp);		
	fMenuHelp->SetTargetForItems(this);

	AddChild(fMenuBar);

	BRect toolBarRect = Bounds();
	toolBarRect.top = fMenuBar->Frame().bottom + 1;
	fToolBar = new STLToolBar(toolBarRect);
	fToolBar->AddAction(MSG_FILE_OPEN, this, STLoverApplication::GetIcon("document-open", TOOLBAR_ICON_SIZE), "Open");
	fToolBar->AddAction(MSG_FILE_SAVE, this, STLoverApplication::GetIcon("document-save", TOOLBAR_ICON_SIZE), "Save");
	fToolBar->AddSeparator();
	fToolBar->AddAction(MSG_TOOLS_EDIT_TITLE, this, STLoverApplication::GetIcon("document-edit", TOOLBAR_ICON_SIZE), "Edit title");
	fToolBar->AddAction(MSG_TOOLS_MIRROR_XY, this, STLoverApplication::GetIcon("mirror-xy", TOOLBAR_ICON_SIZE), "Mirror XY");
	fToolBar->AddAction(MSG_TOOLS_MIRROR_YZ, this, STLoverApplication::GetIcon("mirror-yz", TOOLBAR_ICON_SIZE), "Mirror YZ");
	fToolBar->AddAction(MSG_TOOLS_MIRROR_XZ, this, STLoverApplication::GetIcon("mirror-xz", TOOLBAR_ICON_SIZE), "Mirror XZ");
	fToolBar->AddAction(MSG_TOOLS_MOVE_MIDDLE, this, STLoverApplication::GetIcon("move-middle", TOOLBAR_ICON_SIZE), "Put on the Middle");
	fToolBar->AddAction(MSG_TOOLS_MOVE_TO, this, STLoverApplication::GetIcon("move-to", TOOLBAR_ICON_SIZE), "Move to");
	fToolBar->AddAction(MSG_TOOLS_SCALE, this, STLoverApplication::GetIcon("scale", TOOLBAR_ICON_SIZE), "Scale");
	fToolBar->AddAction(MSG_TOOLS_ROTATE, this, STLoverApplication::GetIcon("rotate", TOOLBAR_ICON_SIZE), "Rotate");
	fToolBar->AddSeparator();
	fToolBar->AddAction(MSG_TOOLS_REPAIR, this, STLoverApplication::GetIcon("tools-wizard", TOOLBAR_ICON_SIZE), "Repair");
	fToolBar->AddSeparator();
	fToolBar->AddAction(MSG_VIEWMODE_STAT_WINDOW, this, STLoverApplication::GetIcon("stat", TOOLBAR_ICON_SIZE), "Statistics");
	fToolBar->AddGlue();
	fToolBar->ResizeTo(toolBarRect.Width(), fToolBar->MinSize().height);
	fToolBar->GroupLayout()->SetInsets(0);
	AddChild(fToolBar);

	BRect viewToolBarRect = Bounds();
	viewToolBarRect.top = fToolBar->Frame().bottom + 1;
	fViewToolBar = new STLToolBar(viewToolBarRect, B_VERTICAL);
	fViewToolBar->AddAction(MSG_VIEWMODE_ZOOMIN, this, STLoverApplication::GetIcon("zoom-in", TOOLBAR_ICON_SIZE), "Zoom in");
	fViewToolBar->AddAction(MSG_VIEWMODE_ZOOMOUT, this, STLoverApplication::GetIcon("zoom-out", TOOLBAR_ICON_SIZE), "Zoom out");
	fViewToolBar->AddAction(MSG_VIEWMODE_ZOOMFIT, this, STLoverApplication::GetIcon("zoom-fit-best", TOOLBAR_ICON_SIZE), "Best fit");
	fViewToolBar->AddSeparator();
	fViewToolBar->AddAction(MSG_VIEWMODE_RESETPOS, this, STLoverApplication::GetIcon("reset", TOOLBAR_ICON_SIZE), "Reset view");
	fViewToolBar->AddSeparator();
	fViewToolBar->AddAction(MSG_VIEWMODE_AXES, this, STLoverApplication::GetIcon("axes", TOOLBAR_ICON_SIZE), "Show axes");
	fViewToolBar->AddAction(MSG_VIEWMODE_OXY, this, STLoverApplication::GetIcon("plane", TOOLBAR_ICON_SIZE), "Show plane OXY");
	fViewToolBar->AddAction(MSG_VIEWMODE_BOUNDING_BOX, this, STLoverApplication::GetIcon("bounding-box", TOOLBAR_ICON_SIZE), "Bounding box");
	fViewToolBar->AddSeparator();
	fViewToolBar->AddAction(MSG_VIEWMODE_FRONT, this, STLoverApplication::GetIcon("view-front", TOOLBAR_ICON_SIZE), "Front view");
	fViewToolBar->AddAction(MSG_VIEWMODE_RIGHT, this, STLoverApplication::GetIcon("view-right", TOOLBAR_ICON_SIZE), "Right view");
	fViewToolBar->AddAction(MSG_VIEWMODE_TOP, this, STLoverApplication::GetIcon("view-top", TOOLBAR_ICON_SIZE), "Top view");
	fViewToolBar->AddGlue();
	fViewToolBar->ResizeTo(fViewToolBar->MinSize().width, viewToolBarRect.Height());
	fViewToolBar->GroupLayout()->SetInsets(0);
	AddChild(fViewToolBar);

	BRect stlRect = Bounds();
	stlRect.top = fToolBar->Frame().bottom + 1;
	stlRect.left =fViewToolBar->Frame().right + 1;
	stlView = new STLView(stlRect, BGL_RGB | BGL_DOUBLE | BGL_DEPTH);
	AddChild(stlView);

	SetSizeLimits(600, 4096, 360, 4049);

	AddShortcut('H', B_COMMAND_KEY,	new BMessage(MSG_EASTER_EGG));

	LoadSettings();
	UpdateUI();
	Show();

	stlView->Render();

	rendererThread = spawn_thread(RenderFunction, "renderThread", B_DISPLAY_PRIORITY, (void*)stlView);
	resume_thread(rendererThread);

	SetPulseRate(1000000);
}

STLWindow::~STLWindow()
{
	SaveSettings();

	if (statWindow != NULL) {
		statWindow->Lock();
		statWindow->Quit();
	}

	status_t exitValue;
	isRenderWork = false;
	wait_for_thread(rendererThread, &exitValue);

	CloseFile();
}

void
STLWindow::UpdateUI(void)
{
	UpdateStats();
	UpdateMenuStates(IsLoaded());
	stlView->RenderUpdate();
}

void
STLWindow::LoadSettings(void)
{
	BPath path;
	if (find_directory (B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append(APP_SETTINGS_FILENAME);
		BFile file(path.Path(), B_READ_ONLY);

		if (file.InitCheck() != B_OK || file.Lock() != B_OK)
			return;

		bool _showBoundingBox = false;
		bool _showAxes = false;
		bool _showStatWindow = false;
		bool _showWireframe = false;
		bool _showOXY = false;

		file.ReadAttr("ShowAxes", B_BOOL_TYPE, 0, &_showAxes, sizeof(bool));
		file.ReadAttr("ShowOXY", B_BOOL_TYPE, 0, &_showOXY, sizeof(bool));
		file.ReadAttr("ShowBoundingBox", B_BOOL_TYPE, 0, &_showBoundingBox, sizeof(bool));
		file.ReadAttr("ShowStatWindow", B_BOOL_TYPE, 0, &_showStatWindow, sizeof(bool));
		file.ReadAttr("ShowWireframe", B_BOOL_TYPE, 0, &_showWireframe, sizeof(bool));
		file.ReadAttr("Exact", B_INT32_TYPE, 0, &exactFlag, sizeof(int32));
		file.ReadAttr("Nearby", B_INT32_TYPE, 0, &nearbyFlag, sizeof(int32));
		file.ReadAttr("RemoveUnconnected", B_INT32_TYPE, 0, &removeUnconnectedFlag, sizeof(int32));
		file.ReadAttr("FillHoles", B_INT32_TYPE, 0, &fillHolesFlag, sizeof(int32));
		file.ReadAttr("NormalDirections", B_INT32_TYPE, 0, &normalDirectionsFlag, sizeof(int32));
		file.ReadAttr("NormalValues", B_INT32_TYPE, 0, &normalValuesFlag, sizeof(int32));
		file.ReadAttr("ReverseAll", B_INT32_TYPE, 0, &reverseAllFlag, sizeof(int32));
		file.ReadAttr("Iterations", B_INT32_TYPE, 0, &iterationsValue, sizeof(int32));

		showBoundingBox = _showBoundingBox;
		stlView->ShowBoundingBox(showBoundingBox);

		showAxes = _showAxes;
		stlView->ShowAxes(showAxes);

		showOXY = _showOXY;
		stlView->ShowOXY(showOXY);

		showWireframe = _showWireframe;
		stlView->SetViewMode(_showWireframe ? MSG_VIEWMODE_WIREFRAME : MSG_VIEWMODE_SOLID);

		if (_showStatWindow)
			this->PostMessage(MSG_VIEWMODE_STAT_WINDOW);

		UpdateMenuStates(IsLoaded());

		file.Unlock();
	}
}

void
STLWindow::SaveSettings(void)
{
	BPath path;
	if (find_directory (B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append(APP_SETTINGS_FILENAME);

		BFile file(path.Path(), B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
		if (file.InitCheck() != B_OK || file.Lock() != B_OK)
			return;

		bool _showStatWindow = statWindow == NULL ? false : !statWindow->IsHidden();

		file.WriteAttr("ShowAxes", B_BOOL_TYPE, 0, &showAxes, sizeof(bool));
		file.WriteAttr("ShowOXY", B_BOOL_TYPE, 0, &showOXY, sizeof(bool));
		file.WriteAttr("ShowBoundingBox", B_BOOL_TYPE, 0, &showBoundingBox, sizeof(bool));
		file.WriteAttr("ShowStatWindow", B_BOOL_TYPE, 0, &_showStatWindow, sizeof(bool));
		file.WriteAttr("ShowWireframe", B_BOOL_TYPE, 0, &showWireframe, sizeof(bool));

		file.WriteAttr("Exact", B_INT32_TYPE, 0, &exactFlag, sizeof(int32));
		file.WriteAttr("Nearby", B_INT32_TYPE, 0, &nearbyFlag, sizeof(int32));
		file.WriteAttr("RemoveUnconnected", B_INT32_TYPE, 0, &removeUnconnectedFlag, sizeof(int32));
		file.WriteAttr("FillHoles", B_INT32_TYPE, 0, &fillHolesFlag, sizeof(int32));
		file.WriteAttr("NormalDirections", B_INT32_TYPE, 0, &normalDirectionsFlag, sizeof(int32));
		file.WriteAttr("NormalValues", B_INT32_TYPE, 0, &normalValuesFlag, sizeof(int32));
		file.WriteAttr("ReverseAll", B_INT32_TYPE, 0, &reverseAllFlag, sizeof(int32));
		file.WriteAttr("Iterations", B_INT32_TYPE, 0, &iterationsValue, sizeof(int32));

		file.Sync();
		file.Unlock();
	}
}


bool
STLWindow::QuitRequested() {
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void 
STLWindow::MessageReceived(BMessage *message)
{
	if (message->WasDropped())
		message->what = B_REFS_RECEIVED;

	switch (message->what) {
		case B_KEY_DOWN:
		case B_UNMAPPED_KEY_DOWN:
		{
			if (IsLoaded()) {
				uint32 key = message->FindInt32("key");
				uint32 modifiers = message->FindInt32("modifiers");

				float scaleFactor = stlView->ScaleFactor();
				float scaleDelta = (GetZDepth() + scaleFactor) * 0.053589838958;
				float xRotate = stlView->XRotate();
				float yRotate = stlView->YRotate();
				float rotateDelta = 5.0;

				if (modifiers & B_SHIFT_KEY) {
					scaleDelta /= 5.0;
					rotateDelta /= 5.0;
				}
				if (modifiers & B_CONTROL_KEY) {
					scaleDelta *= 2.0;
					rotateDelta = 90.0;
				}

				switch (key) {
					case 0x25: // Zoom [-]
					case 0x1C:
						scaleFactor += scaleDelta;
						break;
					case 0x3A: // Zoom [+]
					case 0x1D:
						scaleFactor -= scaleDelta;
						break;
					case 0x61: // Left
						yRotate -= rotateDelta;
						break;
					case 0x63: // Right
						yRotate += rotateDelta;
						break;
					case 0x57: // Up
						xRotate -= rotateDelta;
						break;
					case 0x62: // Down
						xRotate += rotateDelta;
						break;
					case 0x5E: // Reset position [Space]
						stlView->Reset(false, true, true);
						return;
					case 0x64: // Reset scale [0]
					case 0x1B:
						stlView->Reset(true, false, false);
						return;
				}
				stlView->SetScaleFactor(scaleFactor);
				stlView->SetXRotate(xRotate);
				stlView->SetYRotate(yRotate);
			}
			break;
		}
		case MSG_FILE_APPEND:
		case MSG_FILE_OPEN:
		{
			if (!fOpenFilePanel) {
				fOpenFilePanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL,
					B_FILE_NODE, true, NULL, NULL, false, true);
				fOpenFilePanel->SetTarget(this);
			}

			BMessage *openMsg = new BMessage(message->what == MSG_FILE_APPEND ? MSG_APPEND_REFS_RECIEVED : B_REFS_RECEIVED);
			fOpenFilePanel->SetMessage(openMsg);
			delete openMsg;

			fOpenFilePanel->Show();
			break;
		}
		case MSG_FILE_SAVE:
		{
			BPath path(fOpenedFileName);
			if (stlObject->stats.type == binary)
				stl_write_binary(stlObject, path.Path(), stlObject->stats.header);
			else
				stl_write_ascii(stlObject, path.Path(), stlObject->stats.header);
			BNode node(path.Path());
			BNodeInfo nodeInfo(&node);
			nodeInfo.SetType("application/sla");
			stlModified = false;
			UpdateMenuStates(true);
			break;
		}
		case MSG_FILE_EXPORT_STLA:
		case MSG_FILE_EXPORT_STLB:
		case MSG_FILE_EXPORT_DXF:
		case MSG_FILE_EXPORT_VRML:
		case MSG_FILE_EXPORT_OFF:
		case MSG_FILE_EXPORT_OBJ:
		{
			BMessage *fileMsg = new BMessage(*message);
			fileMsg->AddInt32("format", message->what);
			fileMsg->what = B_SAVE_REQUESTED;

			if (!fSaveFilePanel) {
				fSaveFilePanel = new BFilePanel(B_SAVE_PANEL, NULL, NULL,
					B_FILE_NODE, true, fileMsg, NULL, false, true);
				fSaveFilePanel->SetTarget(this);
			} else {
				fSaveFilePanel->SetMessage(fileMsg);
				fSaveFilePanel->SetSaveText("");
			}
			BPath openedFile(fOpenedFileName);
			BString title("Save ");
			title << openedFile.Leaf() << " to ";
			if (message->what == MSG_FILE_EXPORT_STLA)
				title << "STL (ASCII)";
			if (message->what == MSG_FILE_EXPORT_STLB)
				title << "STL (Binary)";
			if (message->what == MSG_FILE_EXPORT_DXF)
				title << "Autodesk DXF";
			if (message->what == MSG_FILE_EXPORT_VRML)
				title << "VRML";
			if (message->what == MSG_FILE_EXPORT_OFF)
				title << "Geomview OFF";
			if (message->what == MSG_FILE_EXPORT_OBJ)
				title << "Wavefront OBJ";
			title << " file as...";
			fSaveFilePanel->Window()->SetTitle(title.String());
			fSaveFilePanel->Show();
			delete fileMsg;
			break;
		}
		case MSG_FILE_CLOSE:
		{
			CloseFile();
			break;
		}
		case MSG_PULSE:
		{
			if (errorTimeCounter > 0) {
				errorTimeCounter--;
				stlView->RenderUpdate();
			}
			break;
		}
		case B_REFS_RECEIVED:
		{
			entry_ref ref;
			for (int32 i = 0; message->FindRef("refs", i, &ref) == B_OK; i++) {
				BEntry entry(&ref, true);
				if (!entry.Exists())
					continue;
				BPath path;
				if (entry.GetPath(&path) != B_OK)
					continue;
				if (i==0) {
					OpenFile(path.Path());
				} else {
					BMessage *msg = new BMessage(B_REFS_RECEIVED);
					msg->AddRef("refs", &ref);
					be_roster->Launch(APP_SIGNATURE, msg);
				}
			}
			break;
		}
		case B_SAVE_REQUESTED:
		{
			entry_ref ref;
			if (message->FindRef("directory", 0, &ref) == B_OK) {
				BEntry entry = BEntry(&ref);
				BPath path;
				entry.GetPath(&path);
				BString filename = message->FindString("name");
				path.Append(filename);
				uint32 format = message->FindInt32("format");
				BString mime("application/sla");
				switch (format) {
					case MSG_FILE_EXPORT_STLA:
						stl_write_ascii(stlObject, path.Path(), stlObject->stats.header);
						break;
					case MSG_FILE_EXPORT_STLB:
						stl_write_binary(stlObject, path.Path(), stlObject->stats.header);
						break;
					case MSG_FILE_EXPORT_DXF:
						stl_write_dxf(stlObject, (char*)path.Path(), stlObject->stats.header);
						mime.SetTo("application/dxf");
						break;
					case MSG_FILE_EXPORT_VRML:
						stl_repair(stlObject, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1);
						stl_generate_shared_vertices(stlObject);
						stl_write_vrml(stlObject, (char*)path.Path());
						mime.SetTo("text/plain");
						break;
					case MSG_FILE_EXPORT_OFF:
						stl_repair(stlObject, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1);
						stl_generate_shared_vertices(stlObject);
						stl_write_off(stlObject, (char*)path.Path());
						mime.SetTo("text/plain");
						break;
					case MSG_FILE_EXPORT_OBJ:
						stl_repair(stlObject, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1);
						stl_generate_shared_vertices(stlObject);
						stl_write_obj(stlObject, (char*)path.Path());
						mime.SetTo("text/plain");
						break;
				}
				BNode node(path.Path());
				BNodeInfo nodeInfo(&node);
				nodeInfo.SetType(mime.String());

				stlModified = false;
				UpdateUI();
			}
			break;
		}
		case MSG_APPEND_REFS_RECIEVED:
		{
			entry_ref ref;

			if (message->FindRef("refs", 0, &ref) == B_OK) {
				BEntry entry = BEntry(&ref);
				BPath path;
				entry.GetPath(&path);
				stl_open_merge(stlObject, (char*)path.Path());
				stl_repair(stlObject, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case B_CANCEL:
			break;
		case B_ABOUT_REQUESTED:
		{
			BAboutWindow* wind = new BAboutWindow(MAIN_WIN_TITLE, APP_SIGNATURE);
			wind->AddCopyright(2020, "Gerasim Troeglazov (3dEyes**)");
			wind->AddDescription(
				"This program is free software; you can redistribute it and/or modify "
				"it under the terms of the GNU General Public License as published by "
				"the Free Software Foundation; either version 2 of the License, or "
				"(at your option) any later version.\n\n"
				"This program is distributed in the hope that it will be useful, "
 				"but WITHOUT ANY WARRANTY; without even the implied warranty of "
 				"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
 				"GNU General Public License for more details.\n\n"
				"You should have received a copy of the GNU General Public License along"
 				"with this program; if not, write to the Free Software Foundation, Inc., "
 				"51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA." );
			wind->Show();
			break;
		}
		case MSG_VIEWMODE_OXY:
		{
			showOXY = !showOXY;
			stlView->ShowOXY(showOXY);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_AXES:
		{
			showAxes = !showAxes;
			stlView->ShowAxes(showAxes);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_BOUNDING_BOX:
		{
			showBoundingBox = !showBoundingBox;
			stlView->ShowBoundingBox(showBoundingBox);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_RESETPOS:
		{
			stlView->Reset();
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_ZOOMIN:
		{
			float scaleFactor = stlView->ScaleFactor();
			float scaleDelta = (GetZDepth() + scaleFactor) * 0.053589838958;
			stlView->SetScaleFactor(scaleFactor - scaleDelta);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_ZOOMOUT:
		{
			float scaleFactor = stlView->ScaleFactor();
			float scaleDelta = (GetZDepth() + scaleFactor) * 0.053589838958;
			stlView->SetScaleFactor(scaleFactor + scaleDelta);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_ZOOMFIT:
		{
			stlView->Reset(true, false, false);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_FRONT:
		{
			stlView->Reset(true, false, true);
			stlView->SetXRotate(-90);
			stlView->SetYRotate(0);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_TOP:
		{
			stlView->Reset(true, false, true);
			stlView->SetXRotate(0);
			stlView->SetYRotate(0);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_RIGHT:
		{
			stlView->Reset(true, false, true);
			stlView->SetXRotate(-90);
			stlView->SetYRotate(-90);
			UpdateUI();
			break;
		}

		case MSG_FILE_RELOAD:
		{
			OpenFile(fOpenedFileName.String());
			break;
		}
		case MSG_VIEWMODE_STAT_WINDOW:
		{
			if (statWindow == NULL) {
				BScreen screen(B_MAIN_SCREEN_ID);
				BRect rect(Frame().right + 12, Frame().top, Frame().right + 250, Frame().bottom);

				if (rect.right >= screen.Frame().Width())
					rect.OffsetTo(screen.Frame().right - rect.Width(), rect.top);

				statWindow = new STLStatWindow(rect, this);
				UpdateStats();
				statWindow->Show();
			} else {
				if (statWindow->IsHidden()) {
					UpdateStats();
					statWindow->Show();
				} else {
					statWindow->Hide();
				}
			}
			UpdateUI();
			fMenuItemStatWin->SetMarked(!statWindow->IsHidden());
			break;
		}
		case MSG_TOOLS_REPAIR:
		{
			BMessage *options = new BMessage();
			options->AddInt32("exactFlag", exactFlag);
			options->AddInt32("nearbyFlag", nearbyFlag);
			options->AddInt32("removeUnconnectedFlag", removeUnconnectedFlag);
			options->AddInt32("fillHolesFlag", fillHolesFlag);
			options->AddInt32("normalDirectionsFlag", normalDirectionsFlag);
			options->AddInt32("normalValuesFlag", normalValuesFlag);
			options->AddInt32("reverseAllFlag", reverseAllFlag);
			options->AddInt32("iterationsValue", iterationsValue);
			options->AddFloat("toleranceValue", stlObject->stats.shortest_edge);
			options->AddFloat("incrementValue", stlObject->stats.bounding_diameter / 10000.0);
			STLRepairWindow *repairDialog = new STLRepairWindow(this, MSG_TOOLS_REPAIR_DO, options);
			repairDialog->Show();
			break;
		}
		case MSG_TOOLS_REPAIR_DO:
		{
			exactFlag = message->FindInt32("exactFlag");
			nearbyFlag = message->FindInt32("nearbyFlag");
			removeUnconnectedFlag = message->FindInt32("removeUnconnectedFlag");
			fillHolesFlag = message->FindInt32("fillHolesFlag");
			normalDirectionsFlag = message->FindInt32("normalDirectionsFlag");
			normalValuesFlag = message->FindInt32("normalValuesFlag");
			reverseAllFlag = message->FindInt32("reverseAllFlag");
			iterationsValue = message->FindInt32("iterationsValue");
			float toleranceValue = message->FindInt32("toleranceValue");
			float incrementValue = message->FindInt32("incrementValue");
			if (IsLoaded()) {
				stl_repair(stlObject, 0, exactFlag, 1, toleranceValue, 1, incrementValue, nearbyFlag,
					iterationsValue, removeUnconnectedFlag, fillHolesFlag, normalDirectionsFlag,
					normalValuesFlag, reverseAllFlag, 0);
				stl_repair(stlObjectView, 0, exactFlag, 1, toleranceValue, 1, incrementValue, nearbyFlag,
					iterationsValue, removeUnconnectedFlag, fillHolesFlag, normalDirectionsFlag,
					normalValuesFlag, reverseAllFlag, 0);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_EDIT_TITLE:
		{
			STLInputWindow *input = new STLInputWindow("STL Title", 1, this, MSG_TOOLS_TITLE_SET);
			input->SetTextValue(0, "Title:", (const char*)stlObject->stats.header);
			input->Show();
			break;
		}
		case MSG_TOOLS_TITLE_SET:
		{
			const char *value = message->FindString("value");
			if (value != NULL && IsLoaded()) {
				snprintf(stlObject->stats.header, 80, value);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_SCALE:
		{
			STLInputWindow *input = new STLInputWindow("Scale", 1, this, MSG_TOOLS_SCALE_SET);
			input->SetTextValue(0, "Scale factor:", "1.0");
			input->Show();
			break;
		}
		case MSG_TOOLS_SCALE_SET:
		{
			const char *value = message->FindString("value");
			if (value != NULL && IsLoaded()) {
				float scaleFactor = atof(value);
				stl_scale(stlObject, scaleFactor);
				stl_scale(stlObjectView, scaleFactor);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_SCALE_3:
		{
			STLInputWindow *input = new STLInputWindow("Custom axis scale", 3, this, MSG_TOOLS_SCALE_SET_3);
			input->SetTextValue(0, "Scale X factor:", "1.0");
			input->SetTextValue(1, "Scale Y factor:", "1.0");
			input->SetTextValue(2, "Scale Z factor:", "1.0");
			input->Show();
			break;
		}
		case MSG_TOOLS_SCALE_SET_3:
		{
			const char *scaleX = message->FindString("value");
			const char *scaleY = message->FindString("value2");
			const char *scaleZ = message->FindString("value3");
			if (scaleX != NULL && scaleY != NULL && scaleZ != NULL && IsLoaded()) {
				float scaleVersor[3];
				scaleVersor[0] = atof(scaleX);
				scaleVersor[1] = atof(scaleY);
				scaleVersor[2] = atof(scaleZ);
				stl_scale_versor(stlObject, scaleVersor);
				stl_scale_versor(stlObjectView, scaleVersor);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_ROTATE:
		{
			STLInputWindow *input = new STLInputWindow("Rotate", 3, this, MSG_TOOLS_ROTATE_SET);
			input->SetTextValue(0, "X-axis:", "0.0");
			input->SetTextValue(1, "Y-axis:", "0.0");
			input->SetTextValue(2, "Z-axis:", "0.0");
			input->Show();
			break;
		}
		case MSG_TOOLS_ROTATE_SET:
		{
			const char *rotateX = message->FindString("value");
			const char *rotateY = message->FindString("value2");
			const char *rotateZ = message->FindString("value3");
			if (rotateX != NULL && rotateY != NULL && rotateZ != NULL && IsLoaded()) {
				float rotateXAngle = atof(rotateX);
				float rotateYAngle = atof(rotateY);
				float rotateZAngle = atof(rotateZ);
				stl_rotate_x(stlObject, rotateXAngle);
				stl_rotate_x(stlObjectView, rotateXAngle);
				stl_rotate_y(stlObject, rotateYAngle);
				stl_rotate_y(stlObjectView, rotateYAngle);
				stl_rotate_z(stlObject, rotateZAngle);
				stl_rotate_z(stlObjectView, rotateZAngle);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_MOVE_CENTER:
		{
			stl_translate(stlObject, -stlObject->stats.size.x / 2, -stlObject->stats.size.y / 2, -stlObject->stats.size.z / 2);
			stlModified = true;
			UpdateUI();
			break;
		}
		case MSG_TOOLS_MOVE_MIDDLE:
		{
			stl_translate(stlObject, -stlObject->stats.size.x / 2, -stlObject->stats.size.y / 2, 0);
			stlModified = true;
			UpdateUI();
			break;
		}
		case MSG_TOOLS_MOVE_ZERO:
		{
			stl_translate(stlObject, 0, 0, 0);
			stlModified = true;
			UpdateUI();
			break;
		}
		case MSG_TOOLS_MOVE_TO:
		{
			STLInputWindow *input = new STLInputWindow("Move to", 3, this, MSG_TOOLS_MOVE_TO_SET);
			input->SetFloatValue(0, "X:", stlObject->stats.min.x);
			input->SetFloatValue(1, "Y:", stlObject->stats.min.y);
			input->SetFloatValue(2, "Z:", stlObject->stats.min.z);
			input->Show();
			break;
		}
		case MSG_TOOLS_MOVE_TO_SET:
		{
			const char *x = message->FindString("value");
			const char *y = message->FindString("value2");
			const char *z = message->FindString("value3");
			if (x != NULL && y != NULL && z != NULL && IsLoaded()) {
				float xValue = atof(x);
				float yValue = atof(y);
				float zValue = atof(z);
				stl_translate(stlObject, xValue, yValue, zValue);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_MOVE_BY:
		{
			STLInputWindow *input = new STLInputWindow("Move by", 3, this, MSG_TOOLS_MOVE_BY_SET);
			input->SetFloatValue(0, "∆X:", 0);
			input->SetFloatValue(1, "∆Y:", 0);
			input->SetFloatValue(2, "∆Z:", 0);
			input->Show();
			break;
		}
		case MSG_TOOLS_MOVE_BY_SET:
		{
			const char *dx = message->FindString("value");
			const char *dy = message->FindString("value2");
			const char *dz = message->FindString("value3");
			if (dx != NULL && dy != NULL && dz != NULL && IsLoaded()) {
				float dxValue = atof(dx);
				float dyValue = atof(dy);
				float dzValue = atof(dz);
				stl_translate_relative(stlObject, dxValue, dyValue, dzValue);
				stlModified = true;
				UpdateUI();
			}
			break;
		}
		case MSG_TOOLS_MIRROR_XY:
		{
			stl_mirror_xy(stlObject);
			stl_mirror_xy(stlObjectView);
			stlModified = true;
			UpdateUI();
			break;
		}
		case MSG_TOOLS_MIRROR_YZ:
		{
			stl_mirror_yz(stlObject);
			stl_mirror_yz(stlObjectView);
			stlModified = true;
			UpdateUI();
			break;
		}
		case MSG_TOOLS_MIRROR_XZ:
		{
			stl_mirror_xz(stlObject);
			stl_mirror_xz(stlObjectView);
			stlModified = true;
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_SOLID:
		{
			showWireframe = false;
			stlView->SetViewMode(MSG_VIEWMODE_SOLID);
			UpdateUI();
			break;
		}
		case MSG_VIEWMODE_WIREFRAME:
		{
			showWireframe = true;
			stlView->SetViewMode(MSG_VIEWMODE_WIREFRAME);
			UpdateUI();
			break;
		}
		case MSG_EASTER_EGG:
		{
			app_info info;
			be_app->GetAppInfo(&info);
			BFile file(&info.ref, B_READ_ONLY);

			BResources res;
			if (res.SetTo(&file) != B_OK)
				break;

			size_t size;
			const void* data = res.LoadResource('rSTL', "Haiku.stl", &size);
			if (data == NULL)
				break;

			BFile eggFile("/tmp/Haiku", B_WRITE_ONLY | B_CREATE_FILE);
			eggFile.Write(data, size);

			OpenFile("/tmp/Haiku");

			break;
		}
		case MSG_POPUP_MENU:
		{
			BPoint point;
			uint32 buttons;
			stlView->GetMouse(&point, &buttons);

			BPopUpMenu* menu = new BPopUpMenu("PopUpMenu",false,false);

			BMenuItem *_menuItemSolid = new BMenuItem("Solid", new BMessage(MSG_VIEWMODE_SOLID));
			_menuItemSolid->SetMarked(!showWireframe);
			BMenuItem *_menuItemWireframe = new BMenuItem("Wireframe", new BMessage(MSG_VIEWMODE_WIREFRAME));
			_menuItemWireframe->SetMarked(showWireframe);

			menu->AddItem(_menuItemSolid);
			menu->AddItem(_menuItemWireframe);
			menu->AddSeparatorItem();

			BMenuItem *_menuItemShowAxes = new BMenuItem("Axes", new BMessage(MSG_VIEWMODE_AXES));
			_menuItemShowAxes->SetMarked(showAxes);
			BMenuItem *_menuItemShowOXY = new BMenuItem("Plane OXY", new BMessage(MSG_VIEWMODE_OXY));
			_menuItemShowOXY->SetMarked(showOXY);
			BMenuItem *_menuItemShowBox = new BMenuItem("Bounding box", new BMessage(MSG_VIEWMODE_BOUNDING_BOX));
			_menuItemShowBox->SetMarked(showBoundingBox);

			menu->AddItem(_menuItemShowAxes);
			menu->AddItem(_menuItemShowOXY);
			menu->AddItem(_menuItemShowBox);
			menu->AddSeparatorItem();
			menu->AddItem(new BMenuItem("Reset", new BMessage(MSG_VIEWMODE_RESETPOS), 'R'));
			menu->AddSeparatorItem();
			menu->AddItem(new BMenuItem("Edit title...", new BMessage(MSG_TOOLS_EDIT_TITLE)));
			menu->AddSeparatorItem();
			menu->AddItem(fMenuToolsScale);
			menu->AddItem(fMenuToolsMove);
			menu->AddItem(fMenuToolsMirror);
			menu->AddItem(new BMenuItem("Rotate...", new BMessage(MSG_TOOLS_ROTATE)));
			menu->AddSeparatorItem();
			menu->AddItem(new BMenuItem("Repair", new BMessage(MSG_TOOLS_REPAIR)));
			menu->SetTargetForItems(this);

			menu->Go(stlView->ConvertToScreen(point), true, false, true);
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void
STLWindow::UpdateMenuStates(bool show)
{
	bool locked = LockWithTimeout(0) == B_OK;
	bool statShowed = statWindow != NULL;
	if (statShowed)
		statShowed = !statWindow->IsHidden();

	fMenuItemClose->SetEnabled(show);
//	fMenuItemAppend->SetEnabled(show);
	fMenuView->SetEnabled(show);
	fMenuTools->SetEnabled(show);
	fMenuToolsMirror->SetEnabled(show);
	fMenuToolsScale->SetEnabled(show);
	fMenuToolsMove->SetEnabled(show);
	fMenuFileSaveAs->SetEnabled(show);
	fMenuItemReload->SetEnabled(show);
	fMenuItemSave->SetEnabled(show && stlModified);
	fMenuItemShowBox->SetMarked(showBoundingBox);
	fMenuItemShowAxes->SetMarked(showAxes);
	fMenuItemShowOXY->SetMarked(showOXY);
	fMenuItemSolid->SetMarked(!showWireframe);
	fMenuItemWireframe->SetMarked(showWireframe);
	fMenuItemStatWin->SetEnabled(show);
	fMenuItemStatWin->SetMarked(statShowed);

	fToolBar->SetActionEnabled(MSG_FILE_SAVE, show && stlModified);
	fToolBar->SetActionEnabled(MSG_VIEWMODE_STAT_WINDOW, show);
	fToolBar->SetActionPressed(MSG_VIEWMODE_STAT_WINDOW, statShowed);
	fToolBar->SetActionEnabled(MSG_TOOLS_EDIT_TITLE, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_MIRROR_XY, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_MIRROR_YZ, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_MIRROR_XZ, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_REPAIR, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_SCALE, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_ROTATE, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_MOVE_TO, show);
	fToolBar->SetActionEnabled(MSG_TOOLS_MOVE_MIDDLE, show);

	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_AXES, show);
	fViewToolBar->SetActionPressed(MSG_VIEWMODE_AXES, showAxes);
	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_OXY, show);
	fViewToolBar->SetActionPressed(MSG_VIEWMODE_OXY, showOXY);
	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_BOUNDING_BOX, show);
	fViewToolBar->SetActionPressed(MSG_VIEWMODE_BOUNDING_BOX, showBoundingBox);
	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_FRONT, show);
	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_TOP, show);
	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_RIGHT, show);
	fViewToolBar->SetActionEnabled(MSG_VIEWMODE_RESETPOS, show);

	if (locked)
		UnlockLooper();
}

void
STLWindow::OpenFile(const char *filename)
{	
	CloseFile();
	stlLoading = true;
	stlView->RenderUpdate();
	stlView->Render();

	stlObject = (stl_file*)malloc(sizeof(stl_file));
	memset(stlObject, 0, sizeof(stl_file));

	stlObjectView = (stl_file*)malloc(sizeof(stl_file));
	memset(stlObjectView, 0, sizeof(stl_file));

	zDepth = -5;
	maxExtent = 10;

	stl_open(stlObject, (char*)filename);
	stl_open(stlObjectView, (char*)filename);
	stlView->SetSTL(stlObject, stlObjectView);
	
	if (stl_get_error(stlObject) || stl_get_error(stlObjectView)) {
		CloseFile();
		errorTimeCounter = 3;
		return;
	}
	
	stl_fix_normal_values(stlObject);
	stl_fix_normal_values(stlObjectView);

	TransformPosition();

	BPath path(filename);
	SetTitle(path.Leaf());
	fOpenedFileName.SetTo(filename);

	stlView->LockGL();
	stlView->LockLooper();

	GLfloat Width = stlView->Bounds().Width() + 1;
	GLfloat Height =  stlView->Bounds().Height() + 1;
	glViewport(0, 0, Width, Height);
  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();

  	gluPerspective(FOV, (GLfloat)Width/(GLfloat)Height, 0.1f, (zDepth + maxExtent));

  	glMatrixMode(GL_MODELVIEW);

	errorTimeCounter = 0;
	stlView->RenderUpdate();

  	stlView->UnlockLooper();
	stlView->UnlockGL();

	stlLoading = false;
	stlModified = false;
	stlValid = true;
	UpdateUI();
}

void
STLWindow::CloseFile(void)
{
	if (IsLoaded()) {
		SetTitle(MAIN_WIN_TITLE);
		stlValid = false;

		stl_file* stl = stlObject;
		stlObject = NULL;
		stl_close(stl);
		free (stl);

		stl = stlObjectView;
		stlObjectView = NULL;
		stl_close(stl);
		free (stl);

		stlLoading = false;
		errorTimeCounter = 0;
		UpdateUI();
	}
}

void
STLWindow::UpdateStats(void)
{
	if (statWindow != NULL) {
		bool isLoaded = IsLoaded();
		if (isLoaded) {
			stl_calculate_volume(stlObject);
//			stl_calculate_surface_area(stlObject);
		}
		BPath path(fOpenedFileName);
		statWindow->SetTextValue("filename", isLoaded ? path.Leaf() : 0);
		statWindow->SetTextValue("type", isLoaded ? (stlObject->stats.type == binary ? "Binary" : "ASCII") : "");
		statWindow->SetTextValue("title", isLoaded ? stlObject->stats.header : "");

		statWindow->SetFloatValue("min-x", isLoaded ? stlObject->stats.min.x : 0);
		statWindow->SetFloatValue("min-y", isLoaded ? stlObject->stats.min.y : 0);
		statWindow->SetFloatValue("min-z", isLoaded ? stlObject->stats.min.z : 0);
		statWindow->SetFloatValue("max-x", isLoaded ? stlObject->stats.max.x : 0);
		statWindow->SetFloatValue("max-y", isLoaded ? stlObject->stats.max.y : 0);
		statWindow->SetFloatValue("max-z", isLoaded ? stlObject->stats.max.z : 0);
		statWindow->SetFloatValue("width", isLoaded ? stlObject->stats.size.x : 0);
		statWindow->SetFloatValue("length", isLoaded ? stlObject->stats.size.y : 0);
		statWindow->SetFloatValue("height", isLoaded ? stlObject->stats.size.z : 0);
		statWindow->SetFloatValue("volume", isLoaded ? stlObject->stats.volume : 0);
//		statWindow->SetFloatValue("surface", isLoaded ? stlObject->stats.surface_area : 0);
		statWindow->SetIntValue("num_facets", isLoaded ? stlObject->stats.number_of_facets : 0);
		statWindow->SetIntValue("num_disconnected_facets",
			isLoaded ? (stlObject->stats.facets_w_1_bad_edge + stlObject->stats.facets_w_2_bad_edge + 
			stlObject->stats.facets_w_3_bad_edge) : 0);
		statWindow->SetIntValue("parts", isLoaded ? stlObject->stats.number_of_parts : 0);
		statWindow->SetIntValue("degenerate", isLoaded ? stlObject->stats.degenerate_facets : 0);
		statWindow->SetIntValue("edges", isLoaded ? stlObject->stats.edges_fixed : 0);
		statWindow->SetIntValue("removed", isLoaded ? stlObject->stats.facets_removed : 0);
		statWindow->SetIntValue("added", isLoaded ? stlObject->stats.facets_added : 0);
		statWindow->SetIntValue("reversed", isLoaded ? stlObject->stats.facets_reversed : 0);
		statWindow->SetIntValue("backward", isLoaded ? stlObject->stats.backwards_edges : 0);
		statWindow->SetIntValue("normals", isLoaded ? stlObject->stats.normals_fixed : 0);
	}
}

void
STLWindow::TransformPosition()
{
	stl_translate(stlObjectView, 0, 0, 0);

	float xMaxExtent = 0;
	float yMaxExtent = 0;
	float zMaxExtent = 0;

	for (int i = 0 ; i < stlObjectView->stats.number_of_facets ; i++) {
		for (int j = 0; j < 3; j++) {
			if (stlObjectView->facet_start[i].vertex[j].x > xMaxExtent)
				xMaxExtent = stlObjectView->facet_start[i].vertex[0].x;
			if (stlObjectView->facet_start[i].vertex[j].y > yMaxExtent)
				yMaxExtent = stlObjectView->facet_start[i].vertex[0].y;
			if (stlObjectView->facet_start[i].vertex[j].z > zMaxExtent)
				zMaxExtent = stlObjectView->facet_start[i].vertex[0].z;
		}
	}

	float longerSide = xMaxExtent > yMaxExtent ? xMaxExtent : yMaxExtent;
	longerSide += (zMaxExtent * (sin(FOV * (M_PI / 180.0)) / sin((90.0 - FOV) * (M_PI / 180.0))));

	zDepth = -((longerSide / 2.0) / tanf((FOV / 2.0) * (M_PI / 180.0)));

	if ((xMaxExtent > yMaxExtent) && (xMaxExtent > zMaxExtent))
    	maxExtent = xMaxExtent;
	if ((yMaxExtent > xMaxExtent) && (yMaxExtent > zMaxExtent))
		maxExtent = yMaxExtent;
	if ((zMaxExtent > yMaxExtent) && (zMaxExtent > xMaxExtent))
		maxExtent = zMaxExtent;

	stl_translate_relative(stlObjectView, -xMaxExtent / 2.0, -yMaxExtent / 2.0, -zMaxExtent / 2.0);
}

int32
STLWindow::RenderFunction(void *data)
{
	STLView *view = (STLView*)data;
	STLWindow *window = (STLWindow*)view->Window();

	while(window->isRenderWork) {
		view->Render();
		snooze(1000000 / FPS_LIMIT);
	}

	return 0;
}
