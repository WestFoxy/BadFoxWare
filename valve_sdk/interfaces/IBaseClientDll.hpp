#pragma once

#include "../Misc/GlobalVars.hpp"
#include "../Misc/ClientClass.hpp"


enum ClientFrameStage_t
{
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END
};

// Used by RenderView
enum RenderViewInfo_t
{
    RENDERVIEW_UNSPECIFIED = 0,
    RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
    RENDERVIEW_DRAWHUD = (1 << 1),
    RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

enum ButtonCode_t1;
enum ScreenFade_t1;

class bf_read1;
class bf_write1;
struct vrect_t1;
class CEngineSprite1;

class CViewSetup1
{
public:
	__int32   x;                  //0x0000 
	__int32   x_old;              //0x0004 
	__int32   y;                  //0x0008 
	__int32   y_old;              //0x000C 
	__int32   width;              //0x0010 
	__int32   width_old;          //0x0014 
	__int32   height;             //0x0018 
	__int32   height_old;         //0x001C 
	char      pad_0x0020[0x90];   //0x0020
	float     fov;                //0x00B0 
	float     viewmodel_fov;      //0x00B4 
	Vector    origin;             //0x00B8 
	Vector    angles;             //0x00C4 
	char      pad_0x00D0[0x7C];   //0x00D0

};//Size=0x014C
class IBaseClientDLL {
public:
	virtual int				Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual int				Disconnect(void) = 0;
	virtual int				Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual void			PostInit() = 0;
	virtual void			Shutdown(void) = 0;
	virtual void			LevelInitPreEntity(char const* pMapName) = 0;
	virtual void			LevelInitPostEntity() = 0;
	virtual void			LevelShutdown(void) = 0;
	virtual ClientClass* GetAllClasses(void) = 0;
	bool DispatchMsg(int msg_type, int unk1, int unk2, void* msg_data)
	{
		return CallVFunction<bool* (__thiscall*)(PVOID, int, int, int ,void*)>(this, 38)(this, msg_type, unk1, unk2, msg_data);
	}

	/*virtual int				HudVidInit(void) = 0;
	virtual void			HudProcessInput(bool bActive) = 0;
	virtual void			HudUpdate(bool bActive) = 0;
	virtual void			HudReset(void) = 0;
	virtual void			HudText(const char* message) = 0;
	virtual void			ShouldDrawDropdownConsole(void) = 0;
	virtual void			IN_ActivateMouse(void) = 0;
	virtual void			IN_DeactivateMouse(void) = 0;
	virtual void			IN_Accumulate(void) = 0;
	virtual void			IN_ClearStates(void) = 0;
	virtual bool			IN_IsKeyDown(const char* name, bool& isdown) = 0;
	virtual int				IN_KeyEvent(int eventcode, ButtonCode_t1 keynum, const char* pszCurrentBinding) = 0;
	//virtual void xd() = 0;
	virtual void			CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool			WriteUsercmdDeltaToBuffer(int nSlot, bf_write1* buf, int from, int to, bool isnewcommand) = 0;
	virtual void			EncodeUserCmdToBuffer(int nSlot, bf_write1& buf, int slot) = 0;
	virtual void			DecodeUserCmdFromBuffer(int nSlot, bf_read1& buf, int slot) = 0;
	virtual void			View_Render(vrect_t1* rect) = 0;
	virtual void			RenderView(const CViewSetup1& view, int nClearFlags, int whatToDraw) = 0;
	virtual void			View_Fade(ScreenFade_t1* pSF) = 0;
	virtual void			SetCrosshairAngle(const QAngle& angle) = 0;
	virtual void			InitSprite(CEngineSprite1* pSprite, const char* loadname) = 0;
	virtual void			ShutdownSprite(CEngineSprite1* pSprite) = 0;
	virtual int				GetSpriteSize(void) const = 0;
	virtual void			VoiceStatus(int entindex, int iSsSlot, int bTalking) = 0;
	virtual int				PlayerAudible(int audible) = 0;
	virtual void			InstallStringTableCallback(char const* tableName) = 0;
	virtual void			FrameStageNotify(ClientFrameStage_t curStage) = 0;
	virtual bool			DispatchUserMessage(int msg_type, int unk1, int unk2, bf_read1* msg_data) = 0;*/
};