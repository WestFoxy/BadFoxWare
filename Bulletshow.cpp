#include "Bulletshow.h"
IViewRenderBeams* g_pViewRenderBeams;
IVEffects* g_pEffects;
IFEffects* g_pFEngineEffects;

enum
{
	TE_BEAMPOINTS = 0x00,		// beam effect between two points
	TE_SPRITE = 0x01,	// additive sprite, plays 1 cycle
	TE_BEAMDISK = 0x02,	// disk that expands to max radius over lifetime
	TE_BEAMCYLINDER = 0x03,		// cylinder that expands to max radius over lifetime
	TE_BEAMFOLLOW = 0x04,		// create a line of decaying beam segments until entity stops moving
	TE_BEAMRING = 0x05,		// connect a beam ring to two entities
	TE_BEAMSPLINE = 0x06,
	TE_BEAMRINGPOINT = 0x07,
	TE_BEAMLASER = 0x08,		// Fades according to viewpoint
	TE_BEAMTESLA = 0x09,
};
enum
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,		// Only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,		// Has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,		// Has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,		// When drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
	NUM_BEAM_FLAGS = 17	// KEEP THIS UPDATED!
};

void DrawDLight(int ent, Vector m_vecAbsOrigin, Vector m_vecHead)
{
	dlight_t* lpELight = g_pEffects->CL_AllocDlight(0);
	//g_pEffects->
	lpELight->color.b = 255;
	lpELight->color.g = 0;
	lpELight->color.r = 0;
	lpELight->color.exponent = 10.0f;
	lpELight->radius = 75.f;
	lpELight->decay = lpELight->radius / 5.0f;
	lpELight->key = 0;
	lpELight->m_Direction = m_vecAbsOrigin;
	lpELight->origin = m_vecHead + Vector(0, 0, 35);
	lpELight->die = g_GlobalVars->curtime + 0.05f;
}

void DrawBeam(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 9; //TE_BEAMTESLA
	beamInfo.m_pszModelName = "sprites/physbeam.vmt"; //sprites/purplelaser1.vmt
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = g_Options.esp_bullettrace_length;
	beamInfo.m_flWidth = 1.1f;
	beamInfo.m_flEndWidth = 1.1f;
	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.5f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	Beam_t* myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);
	if (myBeam)
		g_pViewRenderBeams->DrawBeam(myBeam);
}
void DrawBeamPaw(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 0; //TE_BEAMPOINTS
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	beamInfo.m_pszModelName = "sprites/glow01.vmt";
	beamInfo.m_pszHaloName = "sprites/glow01.vmt";
	beamInfo.m_flHaloScale = 3.0;
	beamInfo.m_flWidth = g_Options.misc_foot_trails_startsize;
	beamInfo.m_flEndWidth = g_Options.misc_foot_trails_endsize;
	beamInfo.m_flFadeLength = 0.5f;
	beamInfo.m_flAmplitude = 0;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.0f;
	beamInfo.m_nStartFrame = 0.0;
	beamInfo.m_flFrameRate = 0.0;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = g_Options.misc_foot_trails_segments;
	beamInfo.m_bRenderable = true;
	beamInfo.m_flLife = g_Options.misc_foot_trails_lenght;
	beamInfo.m_nFlags = 256 | 512 | 32768; //FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM
	Beam_t* myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);
	if (myBeam)
		g_pViewRenderBeams->DrawBeam(myBeam);
}